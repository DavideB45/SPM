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
	if (argc < 2) {
		cout << "Usage: " << argv[0] << "begin0-end0 ... beginN-endN" << endl;
		return 1;
	}
	vector<pair<ull, ull>> ranges;

	for(int i = 1; i < argc; i++) {
		// this for was written by ChatGpt
		string range = argv[i];
		size_t pos = range.find("-");
		if (pos == string::npos) {
			cout << "Invalid range: " << range << endl;
			return 1;
		}
		ranges.push_back({stoull(range.substr(0, pos)), stoull(range.substr(pos+1))});
		if(ranges.back().first > ranges.back().second) {
			cout << "Invalid range: " << range << endl;
			return 1;
		}
	}

	ull max_steps = 0;
	ull curr_steps = 0;
	TIMERSTART(total);
	for (pair<ull, ull> range : ranges) {
		max_steps = 0;
		for (ull n = range.first; n <= range.second; n++) {
			curr_steps = collatz(n);
			max_steps = max(max_steps, curr_steps);
		}
		printf("Range [%llu, %llu]: max_steps = %llu\n", range.first, range.second, max_steps);
	}
	TIMERSTOP(total);
	return 0;
}