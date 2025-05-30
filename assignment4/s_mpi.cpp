#include <mpi.h>
#include <iostream>
#include <unistd.h>
// My stuff
#include <structures.hpp>
#include <parse_argv.hpp>
#include <config.hpp>
#include <hpc_helpers.hpp>

Record* records = nullptr;

int main(int argc, char* argv[]) {
	int error = parse_argv(argc, argv);
	if (error != 0) {
		fprintf(stderr, "Error parsing arguments at index %d\n", error);
		return error;
	}

	//TODO: substitute with a read from a file to speedup testing
	//records = random_generate(ARRAY_SIZE);

	int provided;
	TIMERSTART(mpiinit)
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	TIMERSTOP(mpiinit);
	
	if (provided < MPI_THREAD_MULTIPLE) {
		std::cerr << "MPI does not support required threading level." << std::endl;
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	TIMERSTART(mpiworld)
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	if (rank == 0) {
		TIMERSTOP(mpiworld);
	}

	if (rank == 1){
		sleep(1);
	}
	

	std::cout << "Hello world from rank " << rank << " out of " << size << " processes!" << std::endl;
	
	MPI_Finalize();
	return 0;
}