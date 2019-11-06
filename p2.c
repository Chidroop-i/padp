#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>

typedef struct
{
	int x , y;
}matrix;

int  main()
{
	matrix m, n , res;
	printf("Enter dimensions matrix m\n");
	scanf("%d %d",&m.x,&m.y);

	printf("Enter dimensions matrix n\n");
	scanf("%d %d",&n.x,&n.y);

	if(m.y != n.x)
	{
		printf("dimension error\n");
		return 0;
	}

	res.x = m.x; res.y = n.y;
	int *matM = (int*)malloc(m.x * m.y * sizeof(int));
	int *matN = (int*)malloc(n.x * n.y * sizeof(int));
	int *matRes =  (int*)malloc(res.x * res.y * sizeof(int));
	int i,j,k;
	srand(time(0));
	for( i = 0; i < m.x; i++)
		for(j = 0; j < m.y; j++)
			matM[ i*m.y + j ] = rand()%50;

	for(i = 0; i < n.x; i++)
		for(j = 0; j < n.y; j++)
			matN[ i*n.y + j ] = rand()%50;
	
	double start, end;
	for(int num_threads = 1; num_threads <= 8 ; num_threads = num_threads*2)
	{
		omp_set_num_threads(num_threads);
		start = omp_get_wtime();

		#pragma omp parallel for private(j,k) //local copy of j, k made for each thread
		for(i = 0; i < m.x; i++) //select a row of matM
		{
			for(j = 0; j < n.y; j++) // select a column of matN
			{
				matRes[ i*res.y + j ] = 0; 
				
				for(k = 0; k < n.x; k++) // m.y == n.x
					matRes[ i*res.y + j ] += matM[ i*m.y + k ] * matN[ k*n.y + j];
			}
		}

		end = omp_get_wtime();
		printf("%d threads Time : %f\n",num_threads, end-start);
	}


	/*
	printf("Matrix m\n");
	for( i = 0; i < m.x; i++)
		{
			for(j = 0; j < m.y; j++)
				printf("%d ",matM[ i*m.y + j ]);
			printf("\n");
		}
	printf("\n\n");
	printf("Matrix n\n");
	for(i = 0; i < n.x; i++)
		{
			for(j = 0; j < n.y; j++)
				printf("%d ", matN[ i*n.y + j ]);
			printf("\n");
		}
	printf("\n\n");
	printf("Matrix res\n");
	for(i = 0; i < res.x; i++)
		{
			for(j = 0; j < res.y; j++)
				printf("%d ",matRes[ i*res.y + j ]);
			printf("\n");
		}
	*/
	
}
