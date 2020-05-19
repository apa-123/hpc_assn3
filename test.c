#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define SEED     921
#define NUM_ITER 1000000000

int main(int argc, char* argv[])
{
    int proc_id, num_procs, provided;
    int local_count, total_count;
    int flip = 1 << 24;
    double x, y, z, pi;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided); // initialize

    double start_time, stop_time, elapsed_time;
    start_time = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);  // get num of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    int N = 4;
    int* local_mem = malloc(N* sizeof(int));
    int* shared_mem = malloc(num_procs*N*sizeof(int));
    for (int i = 0; i < N; ++i)
    {
        local_mem[i] = proc_id * N + i;
        shared_mem[i] = 0;
    }

    MPI_Win win;
    MPI_Win_create(shared_mem, N*num_procs*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    // MPI_Win_allocate(1000*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &local_count, &win);

    MPI_Win_fence(0, win);    
    // MPI_Put(local_mem, N, MPI_INT, (proc_id+1)%num_procs, 0, N, MPI_INT, win);
    MPI_Put(local_mem, N, MPI_INT, 0, proc_id*sizeof(int) + N, N, MPI_INT, win);

    MPI_Win_fence(0, win);    
    
    printf("mpi rank %d got data: ", proc_id);
    for (int i = 0; i < N*num_procs; i++) {
        printf("%d ", shared_mem[i]);
    }
    printf("\n");

    stop_time = MPI_Wtime();
    elapsed_time = stop_time - start_time;

    if (proc_id == 0) {
       printf("Execution Time: %f\n", elapsed_time);
//       printf("The result is %f\n", pi);
    }
    MPI_Win_free(&win);
    MPI_Finalize(); 

    return 0;
}

