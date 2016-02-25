// Demonstrate use of GSL pseudorandom number generator
// to calculate pi
// Author: Dr. Jonathan Senning
// Modified by: Jordan Hunt
// $Smake: g++ -Wall -O2 -o %F %f wtime.c -lgsl -lgslcblas

#include <iostream>
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include <gsl/gsl_rng.h>
#include <cmath>
#include <pthread.h>
#include <semaphore.h>
#include "wtime.h"

#include <iomanip>
#include <cstdlib>
#include <list>
#include "randomInt.h"





//============================================================================
// Global (shared) variables -- shared between all threads
//============================================================================

sem_t updateTotal;

//----------------------------------------------------------------------------
// Set up PRNG, generate the desired number of values, estimate pi
// based on the values, and shutdown the PRNG
//
// Input:
//    const long numSamples  - number of samples
//	  bool quiet - specifies how much info to print
// 
// Output:
//    output to stdout

void estimatePi( const long numSamples, bool quiet, int numThreads )
{
   // Set up random number generator using PID and system time as seed
   double t1 = wtime();
   gsl_rng* rng = gsl_rng_alloc( gsl_rng_default );
   gsl_rng_set( rng, 100 * getpid() + time( NULL ) );
   double x, y;
   double count = 0;
   // Get samples and compute estimate of Pi
   for ( int i = 0; i < numSamples; i++ )
   {
	   //printf( "%20.16f\n", gsl_rng_uniform( rng ) );
	   x = gsl_rng_uniform( rng );
	   y = gsl_rng_uniform( rng );
	   if ( (x * x + y * y) < 1 ) 
	   {
			count++;
	   }
       //printf( "%20.16f\n", gsl_rng_uniform( rng ) );
   }
   double estimate_of_pi = (4 * count) / numSamples;
   double error = std::abs(estimate_of_pi - M_PI);
   double t2 = wtime();
   double time = t2 - t1;
   if (quiet) 
   {
      printf( "%12.10f %10.3e %10.6f %ld\n", estimate_of_pi, error, time, numSamples); 
   } else
   {   
      printf( "Pi: %12.10f, error: %10.3e, seconds: %g, samples: %ld\n", estimate_of_pi, error, time, numSamples);
   }
   // Clean up
   gsl_rng_free( rng );
}

//----------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
   long numSamples = 10L;     // default number of samples to show
   int numThreads = 1; 		  // default number of threads
   int c;
   bool quiet = false;
   // Process command line
   while ( ( c = getopt( argc, argv, "n:qt:" ) ) != -1 )
   {
       switch( c )
       {
           case 'n':
               numSamples = atol( optarg );
               if ( numSamples <= 0 )
               {
                   fprintf( stderr, "number of samples must be positive\n" );
                   fprintf( stderr, "got: %ld\n", numSamples );
                   exit( EXIT_FAILURE );
               }
               break;
		   case 'q':
               quiet = true;
               break;
		   case 't':
			   numThreads = atol( optarg );
			   if ( numThreads <= 0 ) 
			   {
					fprintf( stderr, "number of threads must be positive\n" );
					fprintf( stderr, "got: %d\n", numThreads );
					exit( EXIT_FAILURE );
			   }
			   break;
           default:
               fprintf( stderr, "default usage: %s [-n NUM_SAMPLES]\n", argv[0] );
               exit( EXIT_FAILURE );
       }
   }

    // print the estimate of pi based on the input number of samples
   estimatePi( numSamples, quiet, numThreads );

   // All done
   return 0;
}
