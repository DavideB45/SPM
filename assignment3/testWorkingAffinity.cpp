#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int place = omp_get_place_num();     // Available in OpenMP 4.0+
        int proc = omp_get_proc_bind();      // Just for completeness

        #pragma omp critical
        {
            printf("Thread %d is running on place %d (proc_bind=%d)\n", tid, place, proc);
        }
    }
    return 0;
}
