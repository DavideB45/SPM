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

/**
 * @brief Joins all threads in the provided vector.
 * 
 * This function is a wrapper around the join() method of the std::thread class.
 * It joins all threads in the provided vector, effectively waiting for all threads
 * to finish their execution.
 * 
 * @param threads A reference to the vector containing the threads to join.
 */
void join_threads(vector<thread>& threads);

/**
 * @brief Updates the maximum number of steps for a given range index if the provided steps are greater.
 * 
 * This function is thread-safe and ensures that the maximum steps for a specific range index
 * are updated only if the new steps value is greater than the current maximum. It uses a mutex
 * to synchronize access to the shared data structure.
 * 
 * @param range_index The index of the range for which the maximum steps are being updated.
 * @param steps The number of steps to compare with the current maximum.
 * @param collatz_data A reference to the thread_data structure containing the shared data,
 *                     including the maximum steps array and the mutex for synchronization.
 */
void update_max_steps(int range_index, ull steps, thread_data& collatz_data);

/**
 * @brief Computes the Collatz sequence for the provided ranges using a dinamic schedule.
 */
void dinamic_collatz(thread_data& collatz_data);

/**
 * @brief Computes the Collatz sequence for the provided ranges to be used by the spawned threads.
 */
void dinamic_compute(thread_data& collatz_data, int thread_id);

/**
 * @brief Computes the Collatz sequence for the provided ranges using a static schedule.
 */
void static_collatz(thread_data& collatz_data);

/**
 * @brief Computes the Collatz sequence for the provided ranges to be used by the spawned threads.
 */
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
    ull curr_steps = 0; // steps for the current number
    ull max_steps = 0; // max steps for the current range
    ull begin = 0; // where to start for each range
    ull end = 0; // where to stop for each range
    ull chunk_begin = 0; // where to start for each chunk
    ull chunk_offset = num_threads* task_size; // part to skip for subsequent iterations
    ull in_c_index = 0; // index relative to the current chunk
    // compute all ranges
    for (int i = 0; i < collatz_data.ranges->size(); i++) {
        chunk_begin = thread_id * task_size + collatz_data.ranges->at(i).first;
        end = collatz_data.ranges->at(i).second;
        // compute a single range
        for(chunk_begin; chunk_begin <= end; chunk_begin += chunk_offset){    
            // compute a single chunk
            for (in_c_index = 0; in_c_index < task_size; in_c_index++) {    
                curr_steps = collatz(chunk_begin + in_c_index);
                max_steps = max(max_steps, curr_steps);
            }
        }
        // update max steps
        update_max_steps(i, max_steps, collatz_data);
        max_steps = 0;
    }
}

void join_threads(vector<thread>& threads){
    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
}

void update_max_steps(int range_index, ull steps, thread_data& collatz_data){
    collatz_data.max_steps_mutex->lock();
    if(collatz_data.max_steps[range_index] < steps){
        collatz_data.max_steps[range_index] = steps;
    }
    collatz_data.max_steps_mutex->unlock();
}