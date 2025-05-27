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
		} else {
			fprintf(stderr, "Unknown argument: %s\n", arg.c_str());
			return i;
		}
	}

	return 0;
}

#endif // PARSE_ARGV_HPP