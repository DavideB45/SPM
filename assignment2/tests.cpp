#include <iostream>
#include <string>
#include <vector>
#include <hpc_helpers.hpp>

using namespace std;

using ull = unsigned long long;

ull collatz(ull n) {
	ull steps = 0;
	while (n != 1) {
		n = (n & 1) ? 3 * n + 1 : n / 2;
		steps++;
	}
	return steps;
}


int main(const int argc, const char *argv[]) {
	
	pair<ull, ull> range = {50000000, 100000000};


	ull max_steps = 0;
	TIMERSTART(total);
    for (ull n = range.first; n <= range.second; n++) {
        max_steps = max(max_steps, collatz(n));
    }
	TIMERSTOP(total);
    printf("Range [%llu, %llu]: max_steps = %llu\n", range.first, range.second, max_steps);
    // compute the total number of steps required to compute the collatz sequence for all numbers in the range
    ull total_steps = 0;
    for (ull n = range.first; n <= range.second; n++) {
        total_steps += collatz(n);
    }
    printf("Total steps: %llu\n", total_steps);
    // compute the average number of steps required to compute the collatz sequence for all numbers in the range
    double avg_steps = (double)total_steps / (range.second - range.first + 1);
    printf("Average steps: %f\n", avg_steps);
	return 0;
}