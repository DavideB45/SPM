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
/* Author of the parallelizzation: Davide Borghini
*/

#include <config.hpp>
#include <cmdline.hpp>
#include <utility.hpp>
#include <atomic>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return -1;
    }
    // parse command line arguments and set some global variables
    long start=parseCommandLine(argc, argv);
    if (start<0) return -1;

    std::atomic<bool> success(true);
    #pragma omp parallel num_threads(NUM_THREADS)
    {
    #pragma omp single
    {
    # pragma omp task shared(success)
    {
    while(argv[start]) {
        size_t filesize=0;
        if (isDirectory(argv[start], filesize)) {
            walkDirPar(argv[start], COMP, success);
        } else {
            #pragma omp task shared(success)
            {
                bool local_success = doWork(argv[start], filesize, COMP);
                success.store(success.load() & local_success, std::memory_order_relaxed);
            }
        }
        start++;
    }
    }// end of task
    }// end of single
    }// end of parallel region
    if (!success) {
        printf("Exiting with (some) Error(s)\n");
        return -1;
    }
    printf("Exiting with Success\n");
    return 0;
}
 