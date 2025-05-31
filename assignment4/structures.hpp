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
		for (size_t j = 0; j < RECORD_PAYLOAD - 1; j++) {
			records[i].rpayload[j] = 'A' + (rand() % 26); // Random character A-Z
		}
        records[i].rpayload[RECORD_PAYLOAD - 1] = '\0'; // Null-terminate the string
	}
	return records;
}

void free_records(Record* records, size_t size) {
    for (size_t i = 0; i < size; i++) {
        free(records[i].rpayload);
    }
    free(records);
}

void free_records_quick(Record* records, size_t size) {
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

inline bool std_merge_records(Record* arr1, size_t size1, Record* arr2, size_t size2, Record** result){
    if (arr1 == nullptr || arr2 == nullptr) {
        fprintf(stderr, "One of the arrays is null\n");
        return false;
    }
    *result = (Record*)malloc((size1 + size2) * sizeof(Record));
    if (*result == nullptr) {
        fprintf(stderr, "Memory allocation failed for merged records\n");
        return false;
    }
    std::merge(
        arr1, arr1 + size1, 
        arr2, arr2 + size2, 
        *result, 
        [](const Record& a, const Record& b) {
            return a.key < b.key;
        }
    );
    free_records_quick(arr1, size1);
    free_records_quick(arr2, size2);
    return true;
}

inline void print_records(const Record* records, size_t size, bool payoload_address = false) {
    for (size_t i = 0; i < size; i++) {
        if (payoload_address)
            printf("Key: %lu, Payload Address: %p\n", records[i].key, (void*)records[i].rpayload);
        else
            printf("Key: %lu, Payload: %s\n", records[i].key , records[i].rpayload);
    }
}

inline void save_records_to(const Record* records, size_t size, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing: %s\n", filename);
        return;
    }
    for (size_t i = 0; i < size; i++) {
        fprintf(file, "%lu,%s\n", records[i].key, records[i].rpayload);
    }
    fclose(file);
}

inline void load_records_from(Record*& records, size_t& size, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for reading: %s\n", filename);
        return;
    }
    
    size = 0;
    records = (Record*)malloc(ARRAY_SIZE * sizeof(Record));
    if (records == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, "%lu,%256[^\n]\n", &records[size].key, records[size].rpayload) == 2) {
        records[size].rpayload = (char*)malloc(RECORD_PAYLOAD * sizeof(char));
        if (records[size].rpayload == NULL) {
            fprintf(stderr, "Memory allocation for record payload failed\n");
            fclose(file);
            exit(EXIT_FAILURE);
        }
        size++;
    }
    
    fclose(file);
}

#endif