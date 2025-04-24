/*
 * miniz source code: https://github.com/richgel999/miniz
 * https://code.google.com/archive/p/miniz/
 * 
 * This is a reworked version of the example3.c file distributed with the miniz.c.
 * --------------------
 * example3.c - Demonstrates how to use miniz.c's deflate() and inflate() functions for simple file compression.
 * Public domain, May 15 2011, Rich Geldreich, richgel99@gmail.com. See "unlicense" statement at the end of tinfl.c.
 * For simplicity, this example is limited to files smaller than 4GB, but this is not a limitation of miniz.c.
 * -------------------
 *
 */
/* Author: Massimo Torquati <massimo.torquati@unipi.it>
 * This code is a mix of POSIX C code and some C++ library call.
 */

#include <config.hpp>
#include <cmdline.hpp>
#include <utility.hpp>
#include <vector>
#include <atomic>

#include <hpc_helpers.hpp>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		usage(argv[0]);
		return -1;
	}
	// parse command line arguments and set some global variables
	long start=parseCommandLine(argc, argv);
	if (start<0) return -1;

	bool success = true;
	
	TIMERSTART(total);
	// define all the files to process
	std::vector<std::pair<char*, size_t>> files;
	for (int i = start; i < argc; ++i) {
		size_t filesize=0;
		if(isDirectory(argv[i], filesize)) {
			// recursively add files to the list
			// TODO: implement a function to add files to the list
			success &= expandDir(argv[i], files);
		} else {
			files.emplace_back(argv[i], filesize);
		}
	}
	# pragma omp parallel for num_threads(NUM_THREADS) reduction(&& : success)
	for(size_t i = 0; i < files.size(); ++i) {
		success &= doWorkNoBlock(files[i].first, files[i].second, COMP);
	}
	TIMERSTOP(total);

	if (!success) {
		printf("Exiting with (some) Error(s)\n");
		return -1;
	}
	printf("Exiting with Success\n");
	return 0;
}
 