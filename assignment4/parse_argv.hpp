#if !defined(PARSE_ARGV_HPP)
#define PARSE_ARGV_HPP

#include <cstdio>
#include <string>
#include <config.hpp>

int parse_argv(int argc, char* argv[]) {

	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];

		if (arg == "-s" && i + 1 < argc) {
			ARRAY_SIZE = std::stoull(argv[++i]);
			if (ARRAY_SIZE == 0) {
				fprintf(stderr, "Error: Array size must be greater than 0\n");
				return i;
			}
		} else if (arg == "-r" && i + 1 < argc) {
			RECORD_PAYLOAD = std::stoull(argv[++i]);
			if (RECORD_PAYLOAD == 0) {
				fprintf(stderr, "Error: Record payload size must be greater than 0\n");
				return i;
			}
		} else if (arg == "-t" && i + 1 < argc) {
			NUM_THREADS = std::stoull(argv[++i]);
			if (NUM_THREADS == 0) {
				fprintf(stderr, "Error: Number of threads must be greater than 0\n");
				return i;
			}
		} else if (arg == "-k" && i + 1 < argc) {
			TASK_SIZE = std::stoull(argv[++i]);
			if (TASK_SIZE == 0) {
				fprintf(stderr, "Error: Task size must be greater than 0\n");
				fprintf(stderr, "Or smaller than zero to divide equally between threads\n");
				return i;
			}
			if (TASK_SIZE < 0) {
				TASK_SIZE = ARRAY_SIZE / NUM_THREADS + (ARRAY_SIZE % NUM_THREADS != 0 ? 1 : 0);
			}
			
		} else if (arg == "-h" || arg == "--help") {
			printf("Usage: %s [options]\n", argv[0]);
			printf("Options:\n");
			printf("  -s <size>       Set the size of the array (default: %lu)\n", ARRAY_SIZE);
			printf("  -r <payload>    Set the size of the record payload (default: %lu)\n", RECORD_PAYLOAD);
			printf("  -t <threads>    Set the number of threads (default: %lu)\n", NUM_THREADS);
			printf("  -k <task_size>  Set the task size for parallel processing (default: %lu)\n", TASK_SIZE);
			printf("  -h, --help      Show this help message\n");
			exit(0);
		} else {
			fprintf(stderr, "Unknown argument: %s\n", arg.c_str());
			return i;
		}
	}

	return 0;
}

#endif // PARSE_ARGV_HPP