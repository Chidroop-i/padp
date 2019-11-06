#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//ssh student@172.16.18.206
//password: student

#define size 500

int main(){
    int mat1[size][size], mat2[size][size], res[size][size];
    int i, j, k, temp;

    for(i=0; i<size; i++)
        for(j=0; j<size; j++){
            mat1[i][j] = rand()%10;
            mat2[i][j] = rand()%10;
        }
    
    clock_t t = clock();
    #pragma acc data copyin(mat1, mat2) copy(res)
    #pragma acc kernels
    #pragma acc loop tile(32, 32)
    for(i=0; i<size; i++){
        for(j=0; j<size; j++){
            temp=0;
            #pragma acc loop reduction(+:temp)
            for(k=0; k<size; k++){
                temp += mat1[i][k] * mat2[k][j];
            }
            res[i][j] = temp;
        }
    }
    t = clock() - t;
    double wtime = (double)t/CLOCKS_PER_SEC;
    printf("Time taken %g seconds\n",wtime);
    return 0;
}