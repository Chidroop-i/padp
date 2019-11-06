#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

double func(double x)
{
    return 1 / (1 + x * x);
}

double integrate(double begin, double end, double width)
{
    double accum = 0.0;
    double x = begin;
    while(x <= end)
    {
        accum += func(x) * width;
        x += width;
    }
    return accum;
}

int main(int argc, char** argv)
{
    int rank, num_process;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_process);

    if(num_process <= 1)
    {
        printf("Need more processes!\n");
        MPI_Finalize();
        return 0;
    }

    // const double pi = 3.1415926;
    double begin = 0.0, end = 1.0, width , result = 0.0;
    
    int n = 100000;
    // make n exactly divisible by (num_process - 1)
    n = n / (num_process - 1);
    n = n * (num_process - 1);
    width = (end - begin) / n;

    if(rank == 0) // master
    {
        double start = MPI_Wtime();
        int slave_n = n / (num_process - 1);

        for(int i = 1; i < num_process; i++)
        {
            // calculate begins and ends for each slave, and send
            double slave_begin, slave_end;
            slave_begin = (i - 1) * slave_n * width;
            slave_end   = (i + 0) * slave_n * width;

            // int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
            MPI_Send(&slave_begin , 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
            MPI_Send(&slave_end   , 1, MPI_DOUBLE, i, 2, MPI_COMM_WORLD);
            //printf("master sent to %d\n", i);
        }

        for(int i = 1; i < num_process; i++)
        {
            double slave_result;
            MPI_Status status;

            // int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status * status)
            MPI_Recv(&slave_result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            result += slave_result;
            //printf("master recvd from %d\n", status.MPI_SOURCE);
        }
        double end = MPI_Wtime();
        printf("result %lf\n", 4 * result);  //4 * pi/4 
        printf("total time %lf\n", end - start);
    }
    else // slaves [1 ... num_process - 1]
    {
        double slave_begin, slave_end;
        MPI_Status status;

        // int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status * status)
        MPI_Recv(&slave_begin, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&slave_end  , 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &status);
        //printf("slave %d recvd from master\n", rank);
        
        double start = MPI_Wtime();
        double slave_result = integrate(slave_begin, slave_end, width);
        double end = MPI_Wtime();
        printf("slave %d took time: %lf\n", rank, end - start);       
        
        // int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
        MPI_Send(&slave_result, 1, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);
        //printf("slave %d sent to master\n", rank);
    }

    MPI_Finalize();
    return 0;
}