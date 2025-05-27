#include <stdlib.h>
#include <stdio.h>
#include <parse_argv.hpp>

struct Record {
	unsigned long key;
	char rpayload[64];
};

int main(int argc, char** argv) {
	int error = parse_argv(argc, argv);
	if (error) {
		fprintf(stderr, "Error parsing command line arguments\n");
		return EXIT_FAILURE;
	} else {
		printf("Array size: %u\n", ARRAY_SIZE);
		printf("Record payload size: %u\n", RECORD_PAYLOAD);
		printf("Number of threads: %u\n", NUM_THREADS);
	}
	
}