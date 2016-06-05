/********************************************************************
*
* OpenMP
*
* Example: Hello World in OpenMP
*
* Author: Francisco Gago Villanueva
*********************************************************************/

#include <stdio.h>
#include <omp.h>

int main ()  {
    int nthreads, tid;
    omp_set_num_threads(2);

    //Parallel region with the number of threads
    //Each threads have own copy of variable
    #pragma omp parallel private(tid)                          
    {
        tid = omp_get_thread_num(); //ID of thread
        nthreads = omp_get_num_threads(); //Number of thread

        printf("Thread %d of %d threads: Hello!\n", tid, nthreads);

    }//End of Parallel region
}
