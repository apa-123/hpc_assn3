#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
        int rank, size, i, provided;
        size = 4;
//        MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
        MPI_Init(NULL, NULL);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        printf("Hello World from rank %d from %d processes!\n", rank, size);

        MPI_Finalize();
        return 0;
}
