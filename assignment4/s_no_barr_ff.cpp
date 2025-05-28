#include <parse_argv.hpp>
#include <hpc_helpers.hpp>
#include <structures.hpp>
#include <ff/ff.hpp>
#include <ff/farm.hpp>
#include "ff/node.hpp"
#include <vector>

using namespace ff;

Record* records = nullptr;

// how to use this pair"
struct task {
	size_t start_index;
	size_t end_index; // if negative, full sort; if positive, merge
	size_t current_size; // additional field to track the current size

	// Constructor
	task(size_t start, size_t end, size_t size = 0)
		: start_index(start), end_index(end), current_size(size) {}
};


struct Worker: ff_node_t<task> {
	task *svc(task *in) {
		if (in->current_size == 0) {
			// Full sort task
			sort_records(records + in->start_index, in->end_index - in->start_index);
			return in;
		} else {
			// Merge task
			size_t mid = in->start_index + in->current_size;
			
			// Merge [in->start_index, mid) and [mid, in->end_index) in place
			std::inplace_merge(
				records + in->start_index, 
				records + mid, 
				records + in->end_index,
				[](const Record& a, const Record& b) {
					return a.key < b.key;
				}
			);
			return in;
		}
		
	}
};

// generates the numbers
struct Emitter: ff_monode_t<task> {

	
	size_t tot_tasks = ARRAY_SIZE/TASK_SIZE + (ARRAY_SIZE % TASK_SIZE != 0 ? 1 : 0);
	size_t count = 0;
	size_t current_size = TASK_SIZE;
	int* mergeable = new int[ARRAY_SIZE/2 + 1]; // Array to keep track of mergeable tasks

	task *svc(task *in) {
		if (in == nullptr) { // first call
			size_t begin = 0;
			while (begin < ARRAY_SIZE) {
				ff_send_out(new task(begin, std::min(begin + TASK_SIZE, ARRAY_SIZE), 0));
				begin += TASK_SIZE;
			}
			for (size_t i = 0; i < ARRAY_SIZE/2 + 1; ++i) {
				mergeable[i] = 0; // Initialize mergeable array
			}
			return GO_ON; // no more tasks
		}

		// check if it was the last one (start is the first element; and 2 times the size is equal or greater than the array size)
		if(in->start_index == 0 && in->end_index >= ARRAY_SIZE) {
			return EOS; // No more merging needed
		}

		// received a task from workers
		// set it in mergeable
		size_t sorted_size = in->current_size ? in->current_size*2 : TASK_SIZE; // If current_size is 0, use TASK_SIZE
		unsigned int index = (in->start_index / sorted_size) / 2 ;
		// printf("Emitter received task: start_index=%zu, end_index=%zu, current_size=%zu, index=%u\n",
		//   in->start_index, in->end_index, in->current_size, index);
		if (mergeable[index] < (int) sorted_size/TASK_SIZE) { // 1, 2, 3 (level of merge)
			// Update the mergeable size for this task
			mergeable[index] = sorted_size/TASK_SIZE;
			// printf("Waiting for the other part\n");
			delete in; // delete the task we just processed
			return GO_ON; // keep me alive
		} else {
			// gthe other half is ready
			// Merge the two halves
			in->start_index = index * sorted_size*2;
			if(in->current_size == 0) {
				in->current_size = TASK_SIZE;
			} else {
				in->current_size = sorted_size; // Double the chunk size
			}
			in->end_index = std::min(in->start_index + 2 * in->current_size, ARRAY_SIZE);
			// printf("Merging task: start_index=%zu, end_index=%zu, current_size=%zu\n",
			// 	  in->start_index, in->end_index, in->current_size);
			return in;
		}
	}
};


int main(int argc, char** argv) {
	int error = parse_argv(argc, argv);
	if (error != 0) {
		fprintf(stderr, "Error parsing arguments at index %d\n", error);
		return error;
	}

	records = random_generate(ARRAY_SIZE);

	TIMERSTART(sort_records);
	Emitter emitter;
	ff_farm farm;
	farm.add_emitter(&emitter);
	std::vector<ff_node*> workers;
	for (unsigned long i = 0; i < NUM_THREADS; ++i) {
		Worker* worker = new Worker();
		workers.push_back(worker);
	}
	farm.add_workers(workers);

	farm.remove_collector(); // No collector needed for this example
	farm.wrap_around();
	farm.set_scheduling_ondemand(); // Use ondemand scheduling
	
	if (farm.run_and_wait_end() < 0) {
		fprintf(stderr, "Error running farm\n");
		return -1;
	}

	TIMERSTOP(sort_records);

	// print_records(records, ARRAY_SIZE);
	free_records(records, ARRAY_SIZE);
}