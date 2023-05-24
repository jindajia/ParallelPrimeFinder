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
   MPI_Init (&argc, &argv);

   /* Start the timer */

   MPI_Comm_rank (MPI_COMM_WORLD, &id);
   MPI_Comm_size (MPI_COMM_WORLD, &p);
   MPI_Barrier(MPI_COMM_WORLD);
   elapsed_time = -MPI_Wtime();

   if (argc != 2) {
      if (!id) printf ("Command line: %s <m>\n", argv[0]);
      MPI_Finalize();
      exit (1);
   }

   n = atoll(argv[1]);

   /* Figure out this process's share of the array, as
      well as the integers represented by the first and
      last array elements */

   /* 
      We want to delete Even numbers.
      So we only have (n-1)/2 elements.
      */

   proc0_size = 1+(n-1)/2/p*2;

   if ((proc0_size) < (long long) sqrt((double) n)) {
      if (!id) printf ("Too many processes\n");
      MPI_Finalize();
      exit (1);
   }

   low_value = 3;
   high_value = (long long) sqrt((double) n) - ((long long) sqrt((double) n) + 1) % 2;
   parent_size = (high_value - low_value)/2 + 1;

   /* Allocate parent_primes to seive */
   parent_primes = (char *) malloc (parent_size);

   if (parent_primes == NULL) {
      printf ("Cannot allocate enough memory\n");
      MPI_Finalize();
      exit (1);
   }

   for (i = 0; i < parent_size; i++) parent_primes[i] = 0;

   /* 
      Sequentially mark parent_primes 
      low_value for parent_primes is 3
      high_value for parent_primes is (n-1)/2/p*2 + 1
   */
   prime = 3;
   index = 0;
   do{
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
      while (parent_primes[++index]);
      prime = index * 2 + 3;
   } while (prime * prime <= high_value);


   /* Allocate this process's share of the array. */

   low_value = 3 + id*((n-1)/2)/p*2;
   high_value = 1 + (id+1)*((n-1)/2)/p*2;
   size = (high_value - low_value)/2 + 1;
   index = 0;
   prime = index * 2 + 3;

   marked = (char *) malloc (size);

   if (marked == NULL) {
      printf ("Cannot allocate enough memory\n");
      MPI_Finalize();
      exit (1);
   }

   for (i = 0; i < size; i++) marked[i] = 0;

   do {
      if (prime * prime > low_value)
         first = (prime * prime - low_value) / 2;
      else {
         if (!(low_value % prime)) first = 0;
         else {
            if ( (prime - (low_value) % prime) % 2 == 0) {
               first = (prime - (low_value) % prime) / 2;
            } else {
               first = (prime * 2 - (low_value) % prime) / 2;
            }
         }
      }
      for (i = first; i < size; i += prime) marked[i] = 1;
      //find next prime to seive
      while (parent_primes[++index]);
      prime = index * 2 + 3;
   } while (prime * prime <= n);
   count = 0;
   for (i = 0; i < size; i++)
      if (!marked[i]) count++;
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
