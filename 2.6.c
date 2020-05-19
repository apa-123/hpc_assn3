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
    int local_count = 0;
    int total_count = 0;
    int flip = 1 << 24;
    int rank, num_ranks, i, iter, provided;
    double x, y, z, pi;
    long n = NUM_ITER;
    // int *a;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided); // initialize

    double start_time, stop_time, elapsed_time;
    start_time = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);  // get num of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int* local_mem = malloc(1* sizeof(int));
    int* shared_mem = malloc(num_ranks*sizeof(int));
    for (int i = 0; i < num_ranks; ++i)
    {
        shared_mem[i] = 0;
    }

    MPI_Win win;
    MPI_Win_create(shared_mem, num_ranks*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

    // replace seed with 100
    srand(time(NULL) + 123456789 + rank * SEED); // Important: Multiply SEED by "rank" when you introduce MPI!
    
    flip = flip / num_ranks;

    // Calculate PI following a Monte Carlo method
    for (iter = 0; iter < flip; iter++) 
    {
        // Generate random (X,Y) points
        x = (double)random() / (double)RAND_MAX;
        y = (double)random() / (double)RAND_MAX;
        z = sqrt((x*x) + (y*y));
        
        // Check if point is in unit circle
        if (z <= 1.0)
        {
            local_count++;
        }
    }
    local_mem[0] = local_count;

    MPI_Win_fence(0, win);    
    // MPI_Put(local_mem, 1, MPI_INT, 0, 0, num_ranks, MPI_INT, win);
    MPI_Put(local_mem, 1, MPI_INT, 0, rank+sizeof(int), 1, MPI_INT, win);
    MPI_Win_fence(0, win);    
  
    printf("I am rank %d and I sent data %d \n", rank, local_count);

    if (rank == 0) {
	    for (i = 0; i < num_ranks; i++) {
            printf("Val %d in shared_mem: %d\n", i, shared_mem[i]);
	    	total_count += shared_mem[i];
	    }
    	// Estimate Pi and display the result
    	pi = ((double)total_count / (double) (flip * num_ranks)) * 4.0;
    }

    stop_time = MPI_Wtime();
    elapsed_time = stop_time - start_time;

    if (rank == 0) {
       printf("pi: %f\n", pi);
       printf("Execution Time: %f\n", elapsed_time);
//       printf("The result is %f\n", pi);
    }
    
    // MPI_Win_fence(0, win);    
    MPI_Win_free(&win);
    MPI_Finalize(); 

    return 0;
}

