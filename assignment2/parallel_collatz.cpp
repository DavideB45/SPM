#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <hpc_helpers.hpp>

using namespace std;

using ull = unsigned long long;

bool dynamic_schedule = false;
int num_threads = 16;
int task_size = 1;

ull collatz(ull n);

typedef struct {
    vector<pair<ull, ull>>* ranges;
    ull* max_steps;
    ull curr_steps; // only for dynamic schedule
    mutex* max_steps_mutex;
} thread_data;

void join_threads(vector<thread>& threads);

void dinamic_collatz(thread_data& collatz_data);

void dinamic_compute(thread_data& collatz_data, int thread_id);

void static_collatz(thread_data& collatz_data);

void static_compute(thread_data& collatz_data, int thread_id);

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

    thread_data ranges_data = {
        &ranges,                // ranges
        new ull(ranges.size()), // max_steps
        0,                      // curr_steps
        new mutex()             // max_steps_mutex
    };

    for (int i = 0; i < ranges.size(); i++) {
        ranges_data.max_steps[i] = 1;
    }


    if(dynamic_schedule){
        dinamic_collatz(ranges_data);
    } else {
        static_collatz(ranges_data);
    }
    return 0;
}

ull collatz(ull n) {
    ull steps = 0;
    while (n != 1) {
        n = n % 2 == 0 ? n / 2 : 3 * n + 1;
        steps++;
    }
    return steps;
}

void dinamic_collatz(thread_data& collatz_data){
    TIMERSTART(total);
    //TODO: Implement dynamic schedule
    TIMERSTOP(total);
}

void static_collatz(thread_data& collatz_data){
    TIMERSTART(total);
    vector<thread> threads(num_threads);
    for (int i = 0; i < num_threads; i++) {
        threads[i] = thread(static_compute, collatz_data, i);
    }
    join_threads(threads);
    TIMERSTOP(total);
}

void static_compute(thread_data& collatz_data, int thread_id){
    ull curr_steps = 0;
    for (int i = 0; i < collatz_data.ranges->size(); i++) {
        ull begin = collatz_data.ranges->at(i).first;
        ull end = collatz_data.ranges->at(i).second;
        
        
    }
}

void join_threads(vector<thread>& threads){
    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
}