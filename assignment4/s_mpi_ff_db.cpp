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
// GLOBAL VARIABLES
Record* records = nullptr;			// global records array
unsigned long received_size = 0;	// last received size
unsigned long curr_rec_size = 0;	// current size of the records in the buffer
int rank, c_size;					// MPI rank and size

// how to use this pair"
struct task {
	size_t start_index;
	size_t end_index; // if negative, full sort; if positive, merge
	size_t current_size; // additional field to track the current size

	task(size_t start, size_t end, size_t size = 0)
		: start_index(start), end_index(end), current_size(size) {}
};


struct Worker: ff_node_t<task> {
	task *svc(task *in) {
		if (in->current_size == 0) {
			sort_records(records + in->start_index, in->end_index - in->start_index);
			return in;
		} else {
			size_t mid = in->start_index + in->current_size;			
			std::inplace_merge(records + in->start_index, records + mid, records + in->end_index,
				[](const Record& a, const Record& b) {
					return a.key < b.key;
				}
			);
			return in;
		}	
	}
};

struct Emitter: ff_monode_t<task> {
	
	size_t begin = curr_rec_size - received_size;
	size_t ordered_size = curr_rec_size - received_size; // size of the ordered records
	size_t tot_tasks = received_size / TASK_SIZE + (received_size % TASK_SIZE != 0 ? 1 : 0);
	size_t count = 0;
	size_t curr_t_size = TASK_SIZE;

	task *svc(task *in) {
		if (in == nullptr) { // first calls
			for (size_t i = 0; i < tot_tasks; i++){
				task* t = new task(begin, std::min(begin + TASK_SIZE, curr_rec_size), 0);
				if (!ff_send_out(t, -1, 0)){
                    sort_records(records + t->start_index, t->end_index - t->start_index);
                    count++; // Decrease the total tasks count
                    delete t; // Clean up the task
                }
				begin = begin + TASK_SIZE;
				if (begin >= curr_rec_size) {
					return GO_ON; // no more tasks
				}
			}
		}
		
		delete in; // delete the task we just processed
		if(++count < tot_tasks) {
			return GO_ON; // keep me alive
		}
		// Merge phase
		size_t merge_start = ordered_size;
		tot_tasks = 0;
		while (merge_start + curr_t_size < curr_rec_size) {
			size_t merge_end = std::min(merge_start + 2 * curr_t_size, curr_rec_size);
            task* t = new task(merge_start, merge_end, curr_t_size);
			if(!ff_send_out(t, -1, 0)) {
                std::inplace_merge(records + merge_start, records + merge_start + curr_t_size, records + merge_end,
                    [](const Record& a, const Record& b) {
                        return a.key < b.key;
                    }
                );
                delete t; // Clean up the task
            } else { tot_tasks++; }
			merge_start = merge_end;

		}
		curr_t_size *= 2; // Double the chunk size
		if (curr_t_size >= received_size) {
			return EOS; // No more merging needed
		}
		count = 0; // Reset count for the next round of merging
		return GO_ON; // Keep the emitter alive for the next round of merging
	}
};

ff_farm* create_farm() {
	Emitter* emitter = new Emitter();
	ff_farm* farm = new ff_farm();
	farm->add_emitter(emitter);
	std::vector<ff_node*> workers;
	for (unsigned long i = 0; i < NUM_THREADS; ++i) {
		Worker* worker = new Worker();
		workers.push_back(worker);
	}
	farm->add_workers(workers);
	farm->remove_collector(); // No collector needed for this example
	farm->wrap_around();
	farm->set_scheduling_ondemand(2);
	return farm;
}

int main(int argc, char* argv[]) {
	int error = parse_argv(argc, argv);
	if (error != 0) {
		printf("Error parsing arguments at index %d\n", error);
		return error;
	}

	if(!initMPI(&argc, &argv, &rank, &c_size)) return -1;
	
	//TODO: substitute with a read from a file to speedup testing
	Record* generated_ = nullptr;
	if (rank == 0) generated_ = random_generate(ARRAY_SIZE);

	/* Compute upperbound to needed size for a single node
	 number of sends is ARRAY_SIZE / (MPI_BUFF_SIZE * c_size) +0/1
	 each time (except the last one) we will receive MPI_BUFF_SIZE records
	 the number of received records is ARRAY_SIZE / c_size
	 if the last one is not precise, we will split the rest of the records
	 and the remaining of the remaining (a small number) will be left to the root
	 ---- That's it for the first part of the code
	 the second part is the merge phase, where we will merge the records
	 we will receive from un upper rank (number of rank +1, +2, +4, etc.)
	 each time teh number of records will be doubled
	 this is not impossible to compute but hard, probably we can avoid it
	*/
	size_t full_size = (ARRAY_SIZE / c_size);
	if (ARRAY_SIZE % MPI_BUFF_SIZE != 0) {
		full_size += (ARRAY_SIZE % MPI_BUFF_SIZE) / c_size;
		if (rank == 0) {
			full_size += (ARRAY_SIZE % MPI_BUFF_SIZE) % c_size; // add the rest of the records
		}
	}
	records = (Record*)malloc(full_size * sizeof(Record));

	TIMERSTART(sort_records);
	unsigned long buffering_size = 0;
	MPI_Request request;
	i_distribute_inplace(generated_, MPI_BUFF_SIZE*c_size, rank, c_size, records, &buffering_size, &request);
	MPI_Status status;


	// while waiting for the records to be received we can initialize some stuff
	int num_computers = c_size; // number of computers still working in the cluster
	unsigned long chunk_size = ARRAY_SIZE / num_computers; // expected received dimention (can be optimized)
	Record* new_records = nullptr;
	int modulo = 2;
	int offset = 1; // offset for the next rank to receive from
	ff_farm* farm;
	size_t total_sended = MPI_BUFF_SIZE * c_size; // total number of records sent in the first round

	
	do {
		MPI_Wait(&request, &status);
		curr_rec_size += buffering_size; // update current size
		received_size = buffering_size;
		i_distribute_inplace(
			generated_ + total_sended,
			std::min(MPI_BUFF_SIZE * c_size, (unsigned int) (ARRAY_SIZE - total_sended)),
			rank,
			c_size,
			&(records[curr_rec_size]),
			&buffering_size,
			&request
		);
		total_sended += MPI_BUFF_SIZE * c_size;
		//printf("\033[1;%dmR[%d] about to sort %lu\033[0m\n", 31 + rank, rank, received_size);
		farm = create_farm();
		farm->run_and_wait_end();
		delete farm;
		std::inplace_merge(records, records + curr_rec_size - received_size, records + curr_rec_size,
			[](const Record& a, const Record& b) {
				return a.key < b.key;
			}
		);
	} while (total_sended < ARRAY_SIZE);
	farm = create_farm();
	MPI_Wait(&request, &status);
	curr_rec_size += buffering_size;
	received_size = buffering_size;
	farm->run_and_wait_end();
	delete farm;

	printf("\033[1;%dmRank %d: received %lu records\033[0m\n", 31 + rank, rank, curr_rec_size);
	
	while (num_computers > 1) {
		if(rank % modulo == 0) {
			// receive 
			new_records = receive_records(rank + offset, chunk_size, &received_size);
		} else {
			// send to rank -1
			send_records(rank - offset, records, curr_rec_size);
			free_records_quick(records, received_size);
			MPI_Finalize();
			return 0;
		}
		// merge local_records with new_records
		Record* merged_records = nullptr;
		if (!std_merge_records(records, curr_rec_size, new_records, received_size, &merged_records)) {
			fprintf(stderr, "Merge operation failed\n");
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		}
		records = merged_records; curr_rec_size += received_size; 
		chunk_size *= 2; num_computers /= 2;	modulo *= 2; offset *= 2;
		//printf("Rank %d merged records, new size: %lu\n", rank, curr_rec_size);
	}
	
	TIMERSTOP(sort_records);
	
	//print_records(records, curr_rec_size, false);
	MPI_Finalize();
	return 0;
}