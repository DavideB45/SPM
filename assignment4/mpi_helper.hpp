#include <mpi.h>
#include <structures.hpp>


bool initMPI(int *argc, char** argv[], int* rank, int* size){
    int provided;
    // May need to use serialized
    MPI_Init_thread(argc, argv, MPI_THREAD_FUNNELED, &provided);
    if (provided < MPI_THREAD_FUNNELED){
        std::cerr << "MPI does not support required threading level.\n";
		return 0;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
    MPI_Comm_size(MPI_COMM_WORLD, size);
    return 1;
}


Record* distribute_records(Record* records, int size, int rank, int world_size, unsigned long* received_size) {
    if (rank == 0) {
        int chunk_size = size / world_size;
        MPI_Scatter(
            &(records[size % world_size]), // offset the buffer
            chunk_size * sizeof(Record), 
            MPI_BYTE,
            MPI_IN_PLACE, // receive buffer for root process
            chunk_size * sizeof(Record),
            MPI_BYTE,
            0,
            MPI_COMM_WORLD
        );
        *received_size = chunk_size + size % world_size;
        return records;
    } else {
        int chunk_size = size / world_size;
        Record* records = (Record*) malloc(chunk_size * sizeof(Record));
        MPI_Scatter(
            nullptr, // Address of the send buffer (not used for non-root processes)
            0, // send count (not used for non-root processes)
            0, // Datatype of send buffer elements (handle, significant only at root). 
            records, // REceive buffer
            chunk_size * sizeof(Record), //Number of elements in receive buffer (integer).
            MPI_BYTE, //Datatype of receive buffer elements (handle). 
            0, // Root process rank
            MPI_COMM_WORLD  // Communicator
        );
        *received_size = chunk_size;
        return records;
    }
}

void i_distribute_records(Record* records, int size, int rank, int world_size, Record** receive_buffer, unsigned long* received_size, MPI_Request* request) {
    if (rank == 0) {
        int chunk_size = size / world_size;
        *receive_buffer = (Record*) malloc((chunk_size + size % world_size) * sizeof(Record));
        MPI_Iscatter(
            &(records[size % world_size]), // offset the buffer
            chunk_size * sizeof(Record), 
            MPI_BYTE,
            *receive_buffer, // receive buffer for root process
            chunk_size * sizeof(Record),
            MPI_BYTE,
            0,
            MPI_COMM_WORLD,
            request
        );
        *received_size = chunk_size + size % world_size;
    } else {
        int chunk_size = size / world_size;
        *receive_buffer = (Record*) malloc(chunk_size * sizeof(Record));
        MPI_Iscatter(
            nullptr, // Address of the send buffer (not used for non-root processes)
            0, // send count (not used for non-root processes)
            0, // Datatype of send buffer elements (handle, significant only at root). 
            *receive_buffer, // REceive buffer
            chunk_size * sizeof(Record), // max Number of elements in receive buffer (integer).
            MPI_BYTE, //Datatype of receive buffer elements (handle). 
            0, // Root process rank
            MPI_COMM_WORLD,  // Communicator
            request
        );
        *received_size = chunk_size;
    }
}

void i_distribute_inplace(Record* to_distribute, int size, int rank, int world_size, Record* receive_buffer, unsigned long * received_size, MPI_Request* request) {
    if (rank == 0) {
        int chunk_size = size / world_size;
        MPI_Iscatter(
            &(to_distribute[size % world_size]), // offset the buffer
            chunk_size * sizeof(Record), 
            MPI_BYTE,
            receive_buffer, // receive buffer for root process
            chunk_size* sizeof(Record),
            MPI_BYTE,
            0,
            MPI_COMM_WORLD,
            request
        );
        *received_size = chunk_size + size % world_size;
        // fix for the remaining records
        for (int i = 0; i < size % world_size; i++) {
            receive_buffer[chunk_size + i].key = to_distribute[i].key;
            receive_buffer[chunk_size + i].rpayload = to_distribute[i].rpayload;
        }
    } else {
        int chunk_size = size / world_size;
        MPI_Iscatter(
            nullptr, // Address of the send buffer (not used for non-root processes)
            0, // send count (not used for non-root processes)
            0, // Datatype of send buffer elements (handle, significant only at root). 
            receive_buffer, // REceive buffer
            chunk_size * sizeof(Record), // max Number of elements in receive buffer (integer).
            MPI_BYTE, //Datatype of receive buffer elements (handle). 
            0, // Root process rank
            MPI_COMM_WORLD,  // Communicator
            request
        );
        *received_size = chunk_size;
    }
}

void wait_for_distribute_records(MPI_Request* request, int* received_size) {
    MPI_Status status;
    MPI_Wait(request, &status);
    MPI_Get_count(&status, MPI_BYTE, received_size);
    *received_size /= sizeof(Record); // Convert byte count to record count
}
            


Record* receive_records(int from, unsigned long expected_size, unsigned long* received_size) {
    Record* records = (Record*) malloc(expected_size * sizeof(Record));
    if (records == nullptr) {
        fprintf(stderr, "Memory allocation failed for records\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    
    MPI_Status status;
    MPI_Recv(
        records, // Receive buffer
        expected_size * sizeof(Record), // Number of bytes to receive (at most) 
        MPI_BYTE, // MPI datatype of the receive buffer elements
        from, // Source rank (from which to receive)
        0, // Message tag (not used here, )
        MPI_COMM_WORLD, // Communicator 
        &status // Status object to get information about the received message
    );
    MPI_Get_count(&status, MPI_BYTE, (int*)received_size);
    *received_size /= sizeof(Record); // Convert byte count to record count
    return records;
}

void send_records(int to, Record* records, unsigned long size) {
    MPI_Send(
        records, // Send buffer
        size * sizeof(Record), // Number of bytes to send
        MPI_BYTE, // MPI datatype of the send buffer elements
        to, // Destination rank
        0, // Message tag (not used here)
        MPI_COMM_WORLD // Communicator
    );
}
