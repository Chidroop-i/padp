#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    int rank, num_proc;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

    // ring of procs from [0 ... num_procs - 1]
    int sizes[5] = {100, 1000, 10000, 100000, 1000000};

    //change the index manually and calculate tmin , tmax and tavg for different N
    int n = sizes[0];
    double* buff = (double*) malloc(n * sizeof(double));

    if(rank == 0)
    {   
        // master starts the send
        MPI_Status status;
        double start = MPI_Wtime();
        MPI_Send(buff, n, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD);
        printf("master sent\n");
        MPI_Recv(buff, n, MPI_DOUBLE, num_proc - 1, 1, MPI_COMM_WORLD, &status);
        printf("master recieved\n");
        double end = MPI_Wtime();
        printf("took %lf time\n", end - start);
    }
    else
    {
        // slave rank
        MPI_Status status;
        MPI_Recv(buff, n, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &status);
        printf("slave %d recieved\n", rank);
        MPI_Send(buff, n, MPI_DOUBLE, (rank + 1) % num_proc, 1, MPI_COMM_WORLD);
        printf("slave %d sent\n", rank);
    }
            
    free(buff);
    
    MPI_Finalize();
    return 0;
}