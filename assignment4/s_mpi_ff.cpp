#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <ff/ff.hpp>
#include <ff/farm.hpp>
#include "ff/node.hpp"
// My stuff
#include <structures.hpp>
#include <parse_argv.hpp>
#include <config.hpp>
#include <hpc_helpers.hpp>
#include <mpi_helper.hpp>

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

struct Emitter: ff_monode_t<task> {

	
	size_t begin = 0;
	size_t tot_tasks = ARRAY_SIZE/TASK_SIZE + (ARRAY_SIZE % TASK_SIZE != 0 ? 1 : 0);
	size_t count = 0;
	size_t current_size = TASK_SIZE;

	task *svc(task *in) {
		if (in == nullptr) { // first call
			tot_tasks = ARRAY_SIZE / TASK_SIZE + (ARRAY_SIZE % TASK_SIZE != 0 ? 1 : 0);
			for (size_t i = 0; i < tot_tasks; i++){
				task* t = new task(begin, std::min(begin + TASK_SIZE, ARRAY_SIZE), 0);
				if (!ff_send_out(t, -1, 0)){
                    // If send fails, we can handle it here
                    sort_records(records + t->start_index, t->end_index - t->start_index);
                    count++; // Decrease the total tasks count
                    delete t; // Clean up the task
                }
				
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
            task* t = new task(merge_start, merge_end, current_size);
			if(!ff_send_out(t, -1, 0)) {
                // do the merge in place if send fails
                size_t mid = merge_start + current_size;
                std::inplace_merge(
                    records + merge_start, 
                    records + mid, 
                    records + merge_end,
                    [](const Record& a, const Record& b) {
                        return a.key < b.key;
                    }
                );
                delete t; // Clean up the task
            } else {
			    tot_tasks++;
            }
			merge_start = merge_end;
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

int main(int argc, char* argv[]) {
	int error = parse_argv(argc, argv);
	if (error != 0) {
		printf("Error parsing arguments at index %d\n", error);
		return error;
	}

	int rank, size;
	if(!initMPI(&argc, &argv, &rank, &size)) return -1;
	
	//TODO: substitute with a read from a file to speedup testing
	Record* generated_ = nullptr;
	if (rank == 0){
		generated_ = random_generate(ARRAY_SIZE);
		//print_records(records, ARRAY_SIZE, false);
		//printf("\n");
	}

	TIMERSTART(sort_records);
	unsigned long received_size = 0;
	MPI_Request request;
	i_distribute_records(
		generated_, 
		ARRAY_SIZE, 
		rank, 
		size, 
		&records,
		&received_size,
		&request
	);
	MPI_Status status;
	//printf("Rank %d received %lu records\n", rank, received_size);
	//sort_records(local_records, received_size);
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
	farm.set_scheduling_ondemand(2);
	
	int num_computers = size; // number of computers still working in the cluster
	unsigned long chunk_size = ARRAY_SIZE / num_computers; // expected received dimention
	unsigned long current_size = received_size; // current size of the records in the buffer
	Record* new_records = nullptr;
	int modulo = 2;
	int offset = 1; // offset for the next rank to receive from
	ARRAY_SIZE = received_size; 
	MPI_Wait(&request, &status);
	//printf("%d Starting work\n", rank);
	
	farm.run_and_wait_end();
	//printf("%d Sorting completed", rank);
	while (num_computers > 1) {
		if(rank % modulo == 0) {
			// receive 
			new_records = receive_records(rank + offset, chunk_size, &received_size);
		} else {
			// send to rank -1
			send_records(rank - offset, records, current_size);
			free_records_quick(records, received_size);
			MPI_Finalize();
			return 0;
		}
		// merge local_records with new_records
		Record* merged_records = nullptr;
		if (!std_merge_records(records, current_size, new_records, received_size, &merged_records)) {
			fprintf(stderr, "Merge operation failed\n");
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		}
		records = merged_records; current_size += received_size; 
		chunk_size *= 2; num_computers /= 2;	modulo *= 2; offset *= 2;
		//printf("Rank %d merged records, new size: %lu\n", rank, current_size);
	}
	
	TIMERSTOP(sort_records);
	
	//print_records(records, current_size, false);
	MPI_Finalize();
	return 0;
}