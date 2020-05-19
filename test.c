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
    int* local_mem = malloc(1 * sizeof(int));
    int* shared_mem = malloc(N*sizeof(int));
    for (int i = 0; i < N; ++i)
    {
        shared_mem[i] = 0;
    }
    local_mem[0] = proc_id;

    MPI_Win win;
    MPI_Win_create(shared_mem, N*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    // MPI_Win_allocate(1000*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &local_count, &win);

    printf("HELLO\n");

    MPI_Win_fence(0, win);    
    MPI_Put(local_mem, N, MPI_INT, 0, 0, N, MPI_INT, win);
    MPI_Win_fence(0, win);    
        for (int i = 0; i < num_procs; i++) {
        printf("Val %d in shared_mem rank: %d %d\n", i, shared_mem[i], proc_id);
    }


    printf("I am rank %d and I got data\n", proc_id);

    if (proc_id == 0) {
	    int counts[num_procs - 1];
        MPI_Get(&counts, num_procs - 1, MPI_INT, 0, 0, 1, MPI_INT, win);


	    // total_count += local_count;
	    for (int i = 0; i < num_procs - 1; i++) {
	    	total_count += counts[i];
	    }
    	    // Estimate Pi and display the result
    	pi = ((double)total_count / (double) (flip * num_procs)) * 4.0;
    }

    stop_time = MPI_Wtime();
    elapsed_time = stop_time - start_time;

    if (proc_id == 0) {
       printf("pi: %f\n", pi);
       printf("Execution Time: %f\n", elapsed_time);
//       printf("The result is %f\n", pi);
    }
    MPI_Win_free(&win);
    MPI_Finalize(); 

    return 0;
}

