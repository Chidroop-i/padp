#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>

#define K 4

int num_threads;
long num_points;

typedef struct
{
    int x, y;
} vect;

vect cluster[K] = {
    {75, 25}, 
    {25, 25}, 
    {25, 75}, 
    {75, 75}
};

int cluster_count[K];

void populate_points(vect *points) {

    srand(time(NULL));
    for (long i=0; i<num_points; i++) {
        
        points[i].x = rand()%100;
        points[i].y = rand()%100;
    }
}

double get_distance(int x1, int y1, int x2, int y2) {
    int x = x2-x1, y = y2-y1;
    return sqrt(x*x + y*y);
}

void classify_points(vect *points) {

     // Initialize cluster_count
    for (int i=0; i<K; i++)
        cluster_count[i] = 0;

    #pragma omp parallel for num_threads(num_threads)
    for (long i=0; i<num_points; i++) {
        double min_dist = 100, cur_dist;
        int cluster_index;
        for (int j=0; j<K; j++) {
            cur_dist = get_distance(
                            points[i].x, points[i].y,
                            cluster[j].x, cluster[j].y
                       );
            //To identify the cluster closest to the selected point
            if (cur_dist < min_dist) {
                min_dist = cur_dist;
                cluster_index = j;
            }
        }
        // printf("Point (%d , %d) belongs to cluster %d\n" , points[i*2 + 0], points[i*2 +1], cluster_index); 
        #pragma omp atomic
        cluster_count[cluster_index]++;
    }
}

int main() {
    printf("Enter number of points : ");
    scanf("%ld",&num_points);
    printf("Enter number of threads : ");
    scanf("%d",&num_threads);

    vect *points = (vect *)malloc(sizeof(vect)*num_points); 

    populate_points(points);

    double t1 = omp_get_wtime();
    classify_points(points);
    double t2 = omp_get_wtime();
    
    double t = (t2 - t1) * 1000;
   
    long total = 0 ;
    for(int i = 0; i<K ; i++ )
        {   
            printf("Cluster %d : %d\n",i,cluster_count[i]);
            total+= cluster_count[i];
        }
    printf("Total : %ld\nNo.of Points : %ld\nNo. of thread : %d\nTime Taken: %lf\n",total, num_points, num_threads ,t);
    free(points);
}