#include <iostream>
#include <string.h> //for memset
#include <math.h>
#include <omp.h>

using namespace std;

long mark_multiples(bool prime[], long multiple, long low, long high) {
    long i;
    for (i=low; i <= high; i += multiple) {
        prime[i] = false;
    }
    return i;
}

long cacheUnfriendly(long N) {

    long count = 0;
    bool *prime = new bool[N+1];
    memset(prime , true, N+1);

    for (long p=2; p*p <= N; p++) {
        // If prime[p] is not changed, then it is a prime
        if (prime[p] == true) 
            mark_multiples(prime, p, 2*p, N); // Update all multiples of p
    }

    for (long p=2; p <= N; p++) {
            if (prime[p] == true)
                count++;
        }

    return count;
}

long cacheFriendly(long N) {

    bool* prime = new bool[N+1];
    memset(prime , true, N+1);
    long count = 0;
    long M = (long)sqrt(N);
    long* factor  = new long[M];
    long* striker = new long[M];
    long next_factor = 0;

    for (long p = 2; p*p <= N; p++) // Compute all primes smaller than or equal to sqrt N using simple sieve
    {
        if (prime[p] == true) 
        {
            striker[next_factor] = mark_multiples(prime, p, 2*p, M); //returns the last multiple of p that is just greater than M
            factor[next_factor++] = p;
            count++;
        }
    }

    // Chop input to Windows of size sqrt(N) -> M+1 to 2M , 2M+1 to 3M so on
    for (long window = M+1; window <= N; window += M) {
        
        long limit = min(window + M - 1, N);   // window limit not exceeding N
        for (long p = 0; p < next_factor; p++)   // Walk through the current window
            striker[p] = mark_multiples(prime, factor[p], striker[p], limit);
        
        for (long p=window; p<=limit; p++) {
            if (prime[p] == true)
                count++;
        }
    }
    return count;
}

long ParallelSieve(long N , int num_threads) {

    
    long count = 0, next_factor = 0;
    long M = (long) sqrt(N);
    bool * prime = new bool[N + 1];
    memset(prime, true, N);
    long * factor = new long[M];
    long * striker = new long[M];

    omp_set_num_threads(num_threads);
    #pragma omp parallel 
        {
            #pragma omp single 
            {
                for (long p = 2; p*p <= N; p++)
                if (prime[p] == true) 
                {
                    striker[next_factor] = mark_multiples(prime, p, 2*p, M);
                    factor[next_factor++] = p;
                    count++;
                }
            }

            #pragma omp for reduction(+:count) 
            for (long window = M + 1; window <= N; window += M) 
            {
                long limit = min(window + M - 1, N);
                
                for (long p = 0; p < next_factor; p++) 
                    striker[p] = mark_multiples(prime, factor[p], striker[p], limit);
                
                for (long p = window; p <= limit; p++)
                    if (prime[p] == true)
                         count++;
            }
        }

    delete[] striker;
    delete[] factor;
    delete[] prime;
    return count;



    // long count = 0;
    // long M = (long) sqrt (N);
    // long* factor  = new long[M];
    // long next_factor = 0;

    // omp_set_num_threads(num_threads);
    
    // #pragma omp parallel 
    // {
    //     bool* prime = new bool[M+1];
    //     long* striker = new long[M];
    //     #pragma omp single // Only one of the threads will get primes from 2 to M
    //     {
    //         memset(prime, true, M);
    //         for (long p=2; p*p <= N; p++) 
    //         {
    //             if (prime[p] == true) 
    //             {
    //                 mark_multiples(prime, p, 2*p, M);
    //                 factor[next_factor++] = p;
    //                 count++;
    //             }
    //         }
    //     }

    //     long base = -1;

    //     #pragma omp for reduction(+:count)
    //     for (long window = M+1; window <= N; window += M) {
    //         memset(prime, true, M);
    //         if (base != window) {
    //             base = window;

    //             // Make Striker independent of previous values - Compute from base
    //             for(long k=0; k < next_factor; k++ ) {
    //                 striker[k] = (base + factor[k] - 1) / factor[k] * factor[k] - base;
    //             }
    //         }

    //         long limit = min(window + M -1, N) - base;
    //         for(long k=0; k<next_factor; k++ ) {
    //             striker[k] = mark_multiples(prime, factor[k], striker[k], limit) - M;
    //         }

    //         for (long p=0; p <= limit; p++) {
    //             if (prime[p] == true) {
    //                 count++;
    //             }
    //         }
    //         base += M;
    //     }
    // }
    // return count;
}

int main() {
    long N, count = 0;
    double t1,t2;
    printf("Enter N : ");
    scanf("%ld", &N);

    cout << "\nCache Unfriendly Sieve \n";
    t1 = omp_get_wtime();
    count = cacheUnfriendly(N);
    t2 = omp_get_wtime();
    cout <<"primes : " << count << " Time: " << (t2 - t1) * 1000 << "ms" << "\n\n";
    
    cout << "Cache Friendly Sieve \n" ; 
    t1 = omp_get_wtime();
    count = cacheFriendly(N);
    t2 = omp_get_wtime();
    cout << "primes : " << count << " Time: " << (t2 - t1) * 1000 << "ms" << "\n\n";

    //give large value of N to overcome thread creation overhead
    cout << "Parallel Sieve \n";
    for(int num_threads = 1; num_threads <= 8; num_threads = num_threads*2 )
    {
        t1 = omp_get_wtime();
        count = ParallelSieve(N , num_threads);
        t2 = omp_get_wtime();
        cout << "Threads : " << num_threads <<" primes : " << count << " Time: " << (t2 - t1) * 1000 << "ms" << "\n";
    }

}
