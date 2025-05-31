#include <iostream>
#include <unistd.h>
#include <stdio.h>
// My stuff
#include <structures.hpp>
#include <parse_argv.hpp>
#include <config.hpp>
#include <hpc_helpers.hpp>
#include <mpi_helper.hpp>

Record* records = nullptr;

int main(int argc, char* argv[]) {
	int error = parse_argv(argc, argv);
	if (error != 0) {
		printf("Error parsing arguments at index %d\n", error);
		return error;
	}

	int rank, size;
	if(!initMPI(&argc, &argv, &rank, &size)) return -1;
	
	//TODO: substitute with a read from a file to speedup testing
	if (rank == 0){
		records = random_generate(ARRAY_SIZE);
		//print_records(records, ARRAY_SIZE, false);
		//printf("\n");
	}

	TIMERSTART(sort_records);
	unsigned long received_size = 0;
	unsigned int remaining = ARRAY_SIZE;
	// quanti?
	// (dimensione totale circa) = arraysize/size 
	// buffer = totale/dimensione + 1 (il +1 e' per sicurezza)
	Record** all_records = (Record**) malloc(((ARRAY_SIZE/size)/MPI_BUFF_SIZE + 1)*sizeof(Record*));
	all_records[0] = distribute_records(
		records, 
		std::min(MPI_BUFF_SIZE*size, (unsigned int) ARRAY_SIZE), 
		rank, 
		size, 
		&received_size
	);
	remaining -= std::min(MPI_BUFF_SIZE*size, (unsigned int) ARRAY_SIZE);
	MPI_Request request;
	MPI_Status status;
	int total_receive = 0;
	int to_send = 0;
	while (remaining > 0){
		if(!rank)
			printf("\033[1;32mRank %d: Distributing records, remaining to distribute: %u\033[0m\n", rank, remaining);
		to_send = std::min(MPI_BUFF_SIZE*size, remaining);
		i_distribute_records(
			records + ARRAY_SIZE - remaining,
			to_send,
			rank,
			size,
			&(all_records[total_receive + 1]),
			&received_size,
			&request
		);
		if(rank)
			printf("\033[1;%dmRank %d: received: %lu\033[0m\n", 32 + rank % 7, rank, received_size);
		remaining -= to_send;
		sort_records(all_records[total_receive], received_size);
		total_receive++;
		MPI_Wait(&request, &status);
	}
	sort_records(all_records[total_receive], received_size);
	MPI_Finalize();
	return 0;
	//merge the records
	for(int i = 0; i < total_receive; i++){

	}
	
	Record* local_records = nullptr;
	int num_computers = size; // number of computers still working in the cluster
	unsigned long chunk_size = ARRAY_SIZE / num_computers; // expected received dimention
	unsigned long current_size = received_size; // current size of the records in the buffer
	Record* new_records = nullptr;
	int modulo = 2;
	int offset = 1; // offset for the next rank to receive from
	while (num_computers > 1) {
		if(rank % modulo == 0) {
			// receive 
			new_records = receive_records(rank + offset, chunk_size, &received_size);
		} else {
			// send to rank -1
			send_records(rank - offset, local_records, current_size);
			free_records_quick(local_records, received_size);
			MPI_Finalize();
			return 0;
		}
		// merge local_records with new_records
		Record* merged_records = nullptr;
		if (!std_merge_records(local_records, current_size, new_records, received_size, &merged_records)) {
			fprintf(stderr, "Merge operation failed\n");
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		}
		local_records = merged_records; current_size += received_size; 
		chunk_size *= 2; num_computers /= 2;	modulo *= 2; offset *= 2;
		//printf("Rank %d merged records, new size: %lu\n", rank, current_size);
	}
	TIMERSTOP(sort_records);
	
	//print_records(local_records, current_size, false);

	MPI_Finalize();
	return 0;
}