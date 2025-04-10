#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <hpc_helpers.hpp>

using namespace std;

using ull = unsigned long long;

bool dynamic_schedule = false;
bool lock_free = false;
int num_threads = 16;
int task_size = 1;

ull collatz(ull n);

typedef struct {
	vector<pair<ull, ull>> ranges;
	vector<ull> max_steps;
	ull curr_steps; // only for dynamic schedule
	size_t curr_range; // only for dynamic scheduel
	mutex* max_steps_mutex;
	mutex* curr_range_mutex;// only for dynamic schedule
} thread_data;

typedef struct {
	vector<pair<ull, ull>> ranges;
	vector<atomic<ull>> max_steps; // max number of steps for each range
	vector<atomic<ull>> curr_steps; // current number of steps for each range
} lock_free_data;

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
void dynamic_collatz(thread_data& collatz_data);

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

/**
 * @brief Updates the maximum number of steps for a given range index if the provided steps are greater.
 * 
 * This function is thread-safe and ensures that the maximum steps for a specific range index
 * are updated only if the new steps value is greater than the current maximum. It uses atomic
 * operations to synchronize access to the shared data structure.
 * 
 * @param range_index The index of the range for which the maximum steps are being updated.
 * @param steps The number of steps to compare with the current maximum.
 * @param collatz_data A reference to the lock_free_data structure containing the shared data,
 *                     including the maximum steps array and the current steps array.
 */
void update_max_steps_atomic(int range_index, ull steps, lock_free_data& collatz_data);

/**
 * @brief Computes the Collatz sequence for the provided ranges using a lock-free approach.
 */
void lock_free_collatz(lock_free_data& collatz_data);

/**
 * @brief Computes the Collatz sequence for the provided ranges using a lock-free.
 */
void lock_free_compute(lock_free_data& collatz_data, int thread_id);

int main(const int argc, const char *argv[]) {
	if (argc < 2) {
		cout << "Usage: " << argv[0] << "[-d] [-n 8] [-c 16] begin0-end0 ... beginN-endN" << endl;
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
			} else if(curr_arg[1] == 'f'){
				lock_free = true;
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

	// allocate datastructure for the threads
	thread_data ranges_data = {
		ranges,                // ranges
		vector<ull>(ranges.size(), 1), // max_steps
		0,                      // curr_steps
		0,                      // curr_range
		new mutex(),            // max_steps_mutex
		new mutex()             // curr_range_mutex
	};

	lock_free_data lock_free_data = {
		ranges, 
		vector<atomic<ull>>(ranges.size()),
		vector<atomic<ull>>(ranges.size())
	};

	// this should not be necessary, but it is better to be safe than sorry
	for(int i = 0; i < ranges.size(); i++){
		lock_free_data.max_steps[i] = 0;
		lock_free_data.curr_steps[i] = 0;
	}

	if(dynamic_schedule){
		if(lock_free){
			lock_free_collatz(lock_free_data);
			for(size_t i = 0; i < lock_free_data.ranges.size(); i++){
				printf("Range [%llu, %llu]: max_steps = %llu\n", lock_free_data.ranges[i].first, lock_free_data.ranges[i].second, lock_free_data.max_steps[i].load());
			}
		} else {
			dynamic_collatz(ranges_data);
			for (size_t i = 0; i < ranges_data.ranges.size(); i++) {
				printf("Range [%llu, %llu]: max_steps = %llu\n", ranges_data.ranges[i].first, ranges_data.ranges[i].second, ranges_data.max_steps[i]);
			}
		}
	} else {
		static_collatz(ranges_data);
		for (size_t i = 0; i < ranges_data.ranges.size(); i++) {
			printf("Range [%llu, %llu]: max_steps = %llu\n", ranges_data.ranges[i].first, ranges_data.ranges[i].second, ranges_data.max_steps[i]);
		}
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

void dynamic_collatz(thread_data& collatz_data){
	TIMERSTART(total);
	vector<thread> threads(num_threads);
	for (int i = 0; i < num_threads; i++) {
		threads[i] = thread(dinamic_compute, ref(collatz_data), i);
	}
	join_threads(threads);
	TIMERSTOP(total);
}

void static_collatz(thread_data& collatz_data){
	TIMERSTART(total);
	vector<thread> threads(num_threads);
	for (int i = 0; i < num_threads; i++) {
		threads[i] = thread(static_compute, ref(collatz_data), i);
	}
	join_threads(threads);
	TIMERSTOP(total);
}

void static_compute(thread_data& collatz_data, int thread_id){
	ull curr_steps = 0; // steps for the current number
	ull max_steps = 0; // max steps for the current range
	ull end = 0; // where to stop for each range
	ull chunk_begin = 0; // where to start for each chunk
	ull chunk_offset = num_threads* task_size; // part to skip for subsequent iterations
	int in_c_index = 0; // index relative to the current chunk
	// compute all ranges
	for (size_t i = 0; i < collatz_data.ranges.size(); i++) {
		chunk_begin = thread_id * task_size + collatz_data.ranges[i].first;
		end = collatz_data.ranges[i].second;
		// compute a single range
		for(; chunk_begin <= end; chunk_begin += chunk_offset){    
			// compute a single chunk
			for (in_c_index = 0; in_c_index < task_size && (chunk_begin + in_c_index < end); in_c_index++) {    
				curr_steps = collatz(chunk_begin + in_c_index);
				max_steps = max(max_steps, curr_steps);
			}
		}
		// update max steps
		update_max_steps(i, max_steps, collatz_data);
		max_steps = 0;
	}
}

void dinamic_compute(thread_data& collatz_data, int thread_id){
	ull begin;// begin of the chunk to compute
	ull end; // end of the chunk to compute
	ull curr_in_r = 0; // current index range
	pair<ull,ull>* curr_pair;// defined for readability
	std::vector<ull> max_steps(collatz_data.ranges.size(), 0);
	while (true)
	{
		{
			unique_lock<mutex> lock(*collatz_data.curr_range_mutex);
			//if(collatz_data.curr_range > curr_in_r){
			//    collatz_data.max_steps[curr_in_r] = max(max_s, collatz_data.max_steps[curr_in_r]);
			//}
			if (collatz_data.curr_range >= collatz_data.ranges.size()) {
				break; // No more tasks to process
			}
			curr_in_r = collatz_data.curr_range;
			curr_pair = &(collatz_data.ranges[collatz_data.curr_range]);
			begin = curr_pair->first + collatz_data.curr_steps;
			end = min(begin + task_size - 1, curr_pair->second);
			collatz_data.curr_steps += task_size;
			if (collatz_data.curr_steps + curr_pair->first >= curr_pair->second) {
				collatz_data.curr_range++;
				collatz_data.curr_steps = 0;
			}
		}
		
		// compute collatz
		for (; begin <= end; begin++){
			max_steps[curr_in_r] = max(max_steps[curr_in_r], collatz(begin));
		}
	}
	// // update max steps
	collatz_data.max_steps_mutex->lock();
	for (size_t i = 0; i < collatz_data.ranges.size(); i++) {
		collatz_data.max_steps[i] = max(max_steps[i], collatz_data.max_steps[i]);
	}
	collatz_data.max_steps_mutex->unlock();
}

void join_threads(vector<thread>& threads){
	for (size_t i = 0; i < threads.size(); i++) {
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

void lock_free_collatz(lock_free_data& collatz_data){
	TIMERSTART(total);
	vector<thread> threads(num_threads);
	for (int i = 0; i < num_threads; i++) {
		threads[i] = thread(lock_free_compute, ref(collatz_data), i);
	}
	join_threads(threads);
	TIMERSTOP(total);
}

void lock_free_compute(lock_free_data& collatz_data, int thread_id){
	size_t chunk_index = 0;
	ull begin_cr = collatz_data.ranges[chunk_index].first;; // begin of current range
	ull end_cr = collatz_data.ranges[chunk_index].second;; // end of current range
	ull curr_step; // step from which to start the chunk
	ull curr_max = 0; // current max steps
	ull end_task;
	while(true){
		
		curr_step = begin_cr + collatz_data.curr_steps[chunk_index].fetch_add(task_size);

		if(curr_step > end_cr){ // check if the range is finished
			update_max_steps_atomic(chunk_index, curr_max, collatz_data);
			chunk_index++;
			if(chunk_index >= collatz_data.ranges.size()) break;				
			curr_max = 0;
			begin_cr = collatz_data.ranges[chunk_index].first;
			end_cr = collatz_data.ranges[chunk_index].second;
			continue;
		}

		end_task = min(curr_step + task_size, end_cr);
		for(; curr_step < end_task; curr_step++){
			curr_max = max(curr_max, collatz(curr_step));
		}

	}
}

void update_max_steps_atomic(int range_index, ull steps, lock_free_data& collatz_data){
	ull curr_max = collatz_data.max_steps[range_index];
	while (
		curr_max < steps && 
		!collatz_data.max_steps[range_index].compare_exchange_weak(curr_max, steps)
	);
}