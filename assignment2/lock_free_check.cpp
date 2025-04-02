#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>

using namespace std;

using ull = unsigned long long;


int main(){
    atomic<ull> a(0);

    thread t1([&a](){
        for (int i = 0; i < 1000000; i++){
            a++;
        }
    });

    thread t2([&a](){
        for (int i = 0; i < 1000000; i++){
            a++;
        }
    });

    t1.join();
    t2.join();

    cout << "Is lock free: " << (atomic_is_lock_free(&a) ? "true" : "false") << endl;

    cout << a << endl;
}