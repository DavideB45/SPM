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

	
	size_t begin = 0;
	size_t tot_tasks = ARRAY_SIZE/TASK_SIZE + (ARRAY_SIZE % TASK_SIZE != 0 ? 1 : 0);
	size_t count = 0;
	size_t current_size = TASK_SIZE;

	//TODO: define a datastrucure to keep track of the merges to be done

	task *svc(task *in) {
		if (in == nullptr) { // first call
			for (size_t i = 0; i < tot_tasks; i++){
				
				ff_send_out(new task(begin, std::min(begin + TASK_SIZE, ARRAY_SIZE), 0));
				
				begin = begin + TASK_SIZE;
				if (begin >= ARRAY_SIZE) {
					return GO_ON; // no more tasks
				}
			}
		}
		delete in; // delete the task we just processed
		// check hash collision
		// someting coming back from Workers
		if(++count < tot_tasks) {
			return GO_ON; // keep me alive
		}
		// all tasks are done
		// can merge more
		size_t merge_start = 0;
		tot_tasks = 0;
		while (merge_start + current_size < ARRAY_SIZE) {
			size_t merge_end = std::min(merge_start + 2 * current_size, ARRAY_SIZE);
			// Merge [merge_start, mid) and [mid, merge_end)
			//printf("Merging from %zu to %zu with current size %zu\n", merge_start, merge_end, current_size);
			ff_send_out(new task(merge_start, merge_end, current_size));
			merge_start = merge_end;
			tot_tasks++;
		}
		current_size *= 2; // Double the chunk size
		if (current_size >= ARRAY_SIZE) {
			return EOS; // No more merging needed
		}
		//tot_tasks = ARRAY_SIZE / current_size;
		count = 0; // Reset count for the next round of merging

		return GO_ON; // Keep the emitter alive for the next round of merging
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

	//print_records(records, ARRAY_SIZE);
	free_records(records, ARRAY_SIZE);
}