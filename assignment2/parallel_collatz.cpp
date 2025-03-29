#include <iostream>
#include <string>
#include <vector>
#include <hpc_helpers.hpp>

using namespace std;

using ull = unsigned long long;

ull collatz(ull n) {
    ull steps = 0;
    while (n != 1) {
        n = n % 2 == 0 ? n / 2 : 3 * n + 1;
        steps++;
    }
    return steps;
}

bool dynamic_schedule = false;
int num_threads = 1;
int task_size = 1;

int main(const int argc, const char *argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << "begin0-end0 ... beginN-endN" << endl;
        return 1;
    }
    vector<pair<ull, ull>> ranges;
    for(int i = 1; i < argc; i++) {
        // this for was written by ChatGpt
        string curr_arg = argv[i];
        size_t pos = curr_arg.find("-");
        if (pos == string::npos) {
            cout << "Invalid range: " << curr_arg << endl;
            return 1;
        }
        if(pos == 0){
            if(curr_arg[1] == 'd'){
                dynamic_schedule = true;
            } else if(curr_arg[1] == 'n'){
                /* num threads */
                i++;
                num_threads = stoi(argv[i]);
            } else if(curr_arg[1] == 'c'){
                /* chunk size */
                i++;
                task_size = stoi(argv[i]);
            } else {
                cout << "Invalid argument: " << curr_arg << endl;
                return 1;
            }
            continue;
        }
        ranges.push_back({stoull(curr_arg.substr(0, pos)), stoull(curr_arg.substr(pos+1))});
        if(ranges.back().first > ranges.back().second) {
            cout << "Invalid range: " << curr_arg << endl;
            return 1;
        }
    }

    printf("Dynamic schedule: %s\n", dynamic_schedule ? "true" : "false");
    printf("Number of threads: %d\n", num_threads);
    printf("Task size: %d\n", task_size);


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