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

    cout << "Is lock free: " << (atomic_is_lock_free(&a) ? "true" : "false") << endl;

    ull b = 0;
    b = a.fetch_add(1);
    cout << b << endl;
    cout << a << endl;


}