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

	unsigned long received_size = 0;
	Record* local_records = distribute_records(records, ARRAY_SIZE, rank, size, &received_size);
	//printf("Rank %d received %lu records\n", rank, received_size);
	sort_records(local_records, received_size);
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
		Record* merged_records = (Record*) malloc((current_size + received_size) * sizeof(Record));
		if (merged_records == nullptr) {
			fprintf(stderr, "Memory allocation failed for merged records\n");
			MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
		}
		std::merge(local_records, local_records + current_size, new_records, new_records + received_size, merged_records, [](const Record& a, const Record& b) {
			return a.key < b.key;
		});
		free_records_quick(local_records, current_size);
		free_records_quick(new_records, received_size);
		local_records = merged_records;
		current_size += received_size; // update the current size
		chunk_size *= 2; // double the chunk size for the next iteration
		num_computers /= 2; // halve the number of computers still working
		modulo *= 2; // double the modulo for the next iteration
		offset *= 2; // double the offset for the next rank to receive from
		//printf("Rank %d merged records, new size: %lu\n", rank, current_size);
	}
	
	
	print_records(local_records, current_size, false);

	MPI_Finalize();
	return 0;
}