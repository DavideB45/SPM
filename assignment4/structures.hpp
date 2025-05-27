#if !defined(STRUCTURES_HPP)
#define STRUCTURES_HPP

#include <stdlib.h>
#include <algorithm>
#include <config.hpp>

struct Record {
	unsigned long key;
	char* rpayload;
};

/*
 * Generate an array of records with random keys.
 * The size of the array is defined by ARRAY_SIZE.
 * Each record's key is a random number between 0 and ARRAY_SIZE * 2.
 */
Record* random_generate(size_t size, unsigned int seed = 0) {
    srand(seed);
	Record* records = (Record*)malloc(ARRAY_SIZE * sizeof(Record));
	if (records == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	for (size_t i = 0; i < size; i++) {
		records[i].key = rand() % (ARRAY_SIZE * 2);
		records[i].rpayload = (char*)malloc(RECORD_PAYLOAD * sizeof(char));
		if (records[i].rpayload == NULL) {
			fprintf(stderr, "Memory allocation for record payload failed\n");
			exit(EXIT_FAILURE);
		}
		// Fill the payload with random data
		for (size_t j = 0; j < RECORD_PAYLOAD; j++) {
			records[i].rpayload[j] = 'A' + (rand() % 26); // Random character A-Z
		}
	}
	return records;
}

void free_records(Record* records, size_t size) {
    for (size_t i = 0; i < size; i++) {
        free(records[i].rpayload);
    }
    free(records);
}

inline bool compare_records(const Record& a, const Record& b) {
    return a.key < b.key;
}

inline void sort_records(Record* records, size_t size) {
    std::stable_sort(records, records + size, [](const Record& a, const Record& b) {
        return a.key < b.key;
    });
}

inline void print_records(const Record* records, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("Key: %lu, Payload: %s\n", records[i].key , records[i].rpayload);
    }
}

#endif