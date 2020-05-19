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
    long local_count = 0;
    long total_count = 0;
    int flip = 1 << 24;
    int reducedniter;
    int rank, num_ranks, i, iter, provided;
    double x, y, z, pi;
    long n = NUM_ITER;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided); // initialize

    double start_time, stop_time, elapsed_time;
    start_time = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);  // get num of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(NULL) + 123456789 + rank * SEED); // Important: Multiply SEED by "rank" when you introduce MPI!

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
    	MPI_Reduce(&local_count, &total_count, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    stop_time = MPI_Wtime();
    elapsed_time = stop_time - start_time;

	if (rank == 0) {
    		// Estimate Pi and display the result
    		pi = ((double)total_count / (double) (flip * num_ranks)) * 4.0;
       		printf("pi: %f\n", pi);
       		printf("Execution Time: %f\n", elapsed_time);
	}

    MPI_Finalize(); 

    return 0;
}
