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
    int rank, num_ranks, i, iter, provided, send_rank;
    double x, y, z, pi;
    long n = NUM_ITER;
    int height, lo;
    MPI_Status status;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided); // initialize

    double start_time, stop_time, elapsed_time;
    start_time = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);  // get num of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

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

    lo = log(num_ranks)/log(2);
    height = pow(2, lo - 1);
    
    while(height > 0)
    {
        if ((rank >= height) && (rank < (height << 1))) {
            MPI_Send(&local_count, 1, MPI_INT, rank-height, 0, MPI_COMM_WORLD);
        }
        else if (rank < height) {
            MPI_Recv(&total_count, 1, MPI_INT, rank+height, 0, MPI_COMM_WORLD, &status);
            local_count = local_count + total_count;
        }
        height = height >> 1;
    }

    if (rank == 0) {
        pi = ((double)local_count / (double) (flip * num_ranks)) * 4.0;
    }

    stop_time = MPI_Wtime();
    elapsed_time = stop_time - start_time;

    if (rank == 0) {
       printf("pi: %f\n", pi);
       printf("Execution Time: %f\n", elapsed_time);
//       printf("The result is %f\n", pi);
    }
   
    MPI_Finalize(); 

    return 0;
}
