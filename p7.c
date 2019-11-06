#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/*
mpicc mpi_multitask.c -lm
mpirun -n 3 ./a.out
*/
 
/*
 * MPI_Init()
 * MPI_Finalize()
 * MPI_Comm_size()
 * MPI_Comm_rank()
 * MPI_Wtime()
 * MPI_Send(void* buff, int size, int tag, int )
 * MPI_Recv()
 * 
 *  MPI_Send(
        void* data,
        int count,
        MPI_Datatype datatype,
        int destination,
        int tag,
        MPI_Comm communicator)
    MPI_Recv(
        void* data,
        int count,
        MPI_Datatype datatype,
        int source,
        int tag,
        MPI_Comm communicator,
        MPI_Status* status)
 * 
*/

int isPrime(int num)
{
    int sqrt_num = (int) sqrt(num);
    for(int i = 2; i <= sqrt_num; i++)
    {
        if(num % i == 0)
            return 0;
    }
    return 1;
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0) // master
    {
        double start = MPI_Wtime();
        int input_to_p1 = 100000;
        int input_to_p2 = 200000;
        MPI_Send(&input_to_p1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
        MPI_Send(&input_to_p2, 1, MPI_INT, 2, 2, MPI_COMM_WORLD);

        int output_from_p1;
        int output_from_p2;
        MPI_Status status;
        MPI_Recv(&output_from_p1, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&output_from_p2, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        double end = MPI_Wtime();
        printf("output_from_p1: %d\n", output_from_p1);
        printf("output_from_p2: %d\n", output_from_p2);
        printf("total time taken %lf\n", end - start);
    }
    else if(rank == 1)
    {
        // slave 1 finds num primes in [2 ... inpout from master]
        int from_master;
        MPI_Status status;
        MPI_Recv(&from_master, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        double start = MPI_Wtime();
        
        int num_primes = 0;
        for(int i = 2;i <= from_master;++i)
            if(isPrime(i))
                num_primes++;
    
        double end = MPI_Wtime();

        printf("slave 1 took %lf time\n", end - start);
        MPI_Send(&num_primes, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);

    }
    else if(rank == 2)
    {
        // slave 2 does some other work (need not be finding primes)
        int from_master = -1;
        MPI_Status status;
        MPI_Recv(&from_master, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        double start = MPI_Wtime();

        int num_primes = 0;
        for(int i = 2; i <= from_master; i++)
        {
            if(isPrime(i))
                num_primes++;
        }
        
        double end = MPI_Wtime();
        printf("slave 2 took %lf time\n", end - start);
        MPI_Send(&num_primes, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}