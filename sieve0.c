/*
 *   Sieve of Eratosthenes
 *
 *   Programmed by Michael J. Quinn
 *
 *   Last modification: 7 September 2001
 */

#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN(a,b)  ((a)<(b)?(a):(b))

typedef long long LLong;

int main (int argc, char *argv[])
{
   LLong  count;        /* Local prime count */
   double elapsed_time; /* Parallel execution time */
   LLong  first;        /* Index of first multiple */
   LLong  global_count; /* Global prime count */
   LLong  high_value;   /* Highest value on this proc */
   LLong  i;
   int    id;           /* Process ID number */
   int    index;        /* Index of current prime */
   LLong  low_value;    /* Lowest value on this proc */
   char  *marked;       /* Portion of 2,...,'n' */
   LLong  n;            /* Sieving from 2, ..., 'n' */
   int    p;            /* Number of processes */
   LLong  proc0_size;   /* Size of proc 0's subarray */
   LLong  prime;        /* Current prime */
   LLong  size;         /* Elements in 'marked' */
   char  *parent_primes;/* Primes to sieve */
   LLong  parent_size;  /* Parent primes size*/
   int    block_size;   /* Block size for a cache */
   LLong  outer_i;      /* Designed for increase cache hit, control outer loop index */
   LLong  low_block_value; /* Designed for increase cache hit, current blcok low_value */
   LLong  high_block_value;/* Designed for increase cache hit, current blcok low_value */  
   LLong *primes_cache;       /* Designed for increase cache hit, because iterating find next prime is not efficient */
   LLong  prcache_size;
   LLong  sqrt_N;
   MPI_Init (&argc, &argv);

   /* Start the timer */

   MPI_Comm_rank (MPI_COMM_WORLD, &id);
   MPI_Comm_size (MPI_COMM_WORLD, &p);
   MPI_Barrier(MPI_COMM_WORLD);
   elapsed_time = -MPI_Wtime();

   if (argc != 3) {
      if (!id) printf ("Command line: %s <m>\n", argv[0]);
      MPI_Finalize();
      exit (1);
   }

   n = atoll(argv[1]);
   block_size = atoi(argv[1]);
   /* Figure out this process's share of the array, as
      well as the integers represented by the first and
      last array elements */

   /* 
      We want to delete Even numbers.
      So we only have (n-1)/2 elements.
      */
   

   sqrt_N = (long long) sqrt((double) n);
   low_value = 3;
   high_value = sqrt_N - (sqrt_N + 1) % 2;
   parent_size = (high_value - low_value) / 2 + 1;

   /* Bail out if all the primes used for sieving are
      not all held by process 0 */

   proc0_size = 1+(n-1)/2/p*2;

   if ((proc0_size) < (long long) sqrt((double) n)) {
      if (!id) printf ("Too many processes\n");
      MPI_Finalize();
      exit (1);
   }

   /* Allocate parent_primes to seive */
   parent_primes = (char *) malloc (parent_size);

   if (parent_primes == NULL) {
      printf ("Cannot allocate enough memory\n");
      MPI_Finalize();
      exit (1);
   }

   for (i = 0; i < parent_size; i++) parent_primes[i] = 0;

   printf("rank = %d parent_size = %lld low_value = %lld high_value = %lld \n",id, parent_size, low_value, high_value);
   fflush(stdout);

   /* 
      sequential mark parent_primes 
      low_value for parent_primes is 3
      high_value for parent_primes is (n-1)/2/p*2 + 1
   */
   prime = 3;
   index = 0;
   while (index < parent_size)
   {
      prime = index * 2 + 3;
      index++;
      if (prime * prime > low_value)
         first = ((prime * prime) - low_value) / 2;
      else {
            if ( (prime - (low_value) % prime) % 2 == 0) {
               first = (prime - (low_value) % prime) / 2;
            } else {
               first = (prime * 2 - (low_value) % prime) / 2;
            }
      }
      for (i = first; i < parent_size; i += prime) parent_primes[i] = 1;
      while (index < parent_size && parent_primes[index]) {
         index++;
      }
   }

   count = 0;
   for (i = 0; i < parent_size; i++) {
      if (!parent_primes[i])   count++;
   }

   printf("rank = %d mark finished count = %lld \n",id, count);
   fflush(stdout);

   prcache_size = count;
   primes_cache = (LLong *) malloc (count);
   if (primes_cache == NULL) {
      printf ("Cannot allocate enough memory\n");
      MPI_Finalize();
      exit (1);
   }
   count = 0;
   for (i = 0; i < parent_size; i++) {
      if (!parent_primes[i])   primes_cache[count++] = i * 2 + 3;
   }

   /* Allocate this process's share of the array. */

   low_value = 3 + id*((n-1)/2)/p*2;
   high_value = 1 + (id+1)*((n-1)/2)/p*2;
   size = (high_value - low_value)/2 + 1;

   marked = (char *) malloc (size);

   if (marked == NULL) {
      printf ("Cannot allocate enough memory\n");
      MPI_Finalize();
      exit (1);
   }
   printf("rank = %d size = %lld \n",id, size);
   fflush(stdout);

   for (i = 0; i < size; i++) marked[i] = 0;
   printf("rank = %d marked finish \n",id);
   fflush(stdout);
   for (outer_i = 0, low_block_value = outer_i * block_size * 2 + low_value; low_block_value <= high_value; ++outer_i,low_block_value = outer_i * block_size * 2 + low_value) {

      high_block_value = MIN(high_value, low_block_value + (block_size - 1) * 2);
      printf("rank = %d low_block_value = %lld high_block_value = %lld outer_i = %lld \n",id, low_block_value, high_block_value, outer_i);
      fflush(stdout);
      index = 0;
      while(index < prcache_size) {
         prime = primes_cache[index++];
         if (prime * prime > high_block_value) break;
         if (prime * prime > low_block_value)
            first = (prime * prime - low_block_value) / 2;
         else {
            if (!(low_block_value % prime)) first = 0;
            else {
               if ( (prime - (low_block_value) % prime) % 2 == 0) {
                  first = (prime - (low_block_value) % prime) / 2;
               } else {
                  first = (prime * 2 - (low_block_value) % prime) / 2;
               }
            }
         }

         for (i = first + outer_i * block_size; i*2 + low_value <= high_block_value; i += prime) {
            marked[i] = 1;
         };
      }
   }


   count = 0;
   for (i = 0; i < size; i++)
      if (!marked[i]) count++;
   printf("rank = %d low_value = %lld high_value = %lld count = %lld \n",id, low_value, high_value, count);
   fflush(stdout);
   if (p > 1) MPI_Reduce (&count, &global_count, 1, MPI_LONG_LONG, MPI_SUM,
      0, MPI_COMM_WORLD);

   /* Stop the timer */

   elapsed_time += MPI_Wtime();


   /* Print the results */

   if (!id) {
      global_count++;
      printf ("There are %lld primes less than or equal to %lld\n",
         global_count, n);
      printf ("SIEVE (%d) %10.6f\n", p, elapsed_time);
   }
   MPI_Finalize ();
   return 0;
}
