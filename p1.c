#include<omp.h>
#include<stdio.h>
#include<stdlib.h>

void main()
{
	int n, i, count = 0;
	double z, pi, x,y;

	printf("Enter number of iterations : ");
	scanf("%d", &n);
	srand(n);

    for(int num_threads = 1; num_threads <=8; num_threads = num_threads*2)
    {
        count = 0;
        omp_set_num_threads(num_threads);
        #pragma omp parallel for
        for ( i = 0; i < n; i++) 
        {
            x = (double)rand()/RAND_MAX;
            y = (double)rand()/RAND_MAX;
            z = x*x + y*y;
            if (z<=1)
             count++;
        }
        pi=(double)count/n*4; //imp to cast to double else integer division gives 0 as result
        
        printf("%d threads -> pi estimate : %lf \n", num_threads, pi);
    }
}