#include <parse_argv.hpp>
#include <hpc_helpers.hpp>
#include <structures.hpp>

int main(int argc, char** argv) {
	int error = parse_argv(argc, argv);
	if (error != 0) {
		fprintf(stderr, "Error parsing arguments at index %d\n", error);
		return error;
	}

	Record* records = random_generate(ARRAY_SIZE);

	TIMERSTART(sort_records);
	sort_records(records, ARRAY_SIZE);
	TIMERSTOP(sort_records);

	print_records(records, ARRAY_SIZE);
	free_records(records, ARRAY_SIZE);
}