/*
 ============================================================================
 Name        : mcpr.c
 Author      : Carl T. Knox
 Version     :
 Copyright   : Your copyright notice
 Description : Multi Core Polard Rho Floyd in C w/ OpemMPI & GNU gmp library
 ============================================================================
 mpicc -o "mcpr" mcpr.c -lgmp
 mpirun -np 64 mcpr
 */
#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include "gmp.h"
#define BASE 10

int main()
{	/* Start MPI **************************************************************/
	MPI_Init(NULL, NULL);								// start up MPI
	int cores;											// number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &cores);				// get # of processes
	int rank; 											// rank this process
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);				// get process rank

	/* Declare Variables & Assign Data ****************************************/
	mpz_t n;		mpz_init_set_ui(n, 0);				// set to a Bignumber
	mpz_t f;		mpz_init_set_ui(f, 0);				// set to a Bignumber
	mpz_t seed;		mpz_init_set_ui(seed, 1);			// set to a Bignumber
	mpz_t t;		mpz_init_set_ui(t, 0);				// tortus
	mpz_t h;		mpz_init_set_ui(h, 0);				// hare
	mpz_t z;		mpz_init_set_ui(z, 0);				// set to a Bignumber
	int limit = 230;									// maximum length
	char buff[limit];									// input string
	int tag = 0;    									// tag for messages
	int source = 0;										// rank of sender
	MPI_Status status;   								// status for received
	int w = 2;											// horse power
	char* rtn;											// fgets return status

   	/* Input data & distribute it *********************************************/
	if(rank == 0) 										// the control node
	{	printf("\n rank %d input\n", rank);				// prompt input
		rtn = fgets(buff, limit, stdin);				// alternate input
		int dest;
		for(dest = 1; dest < cores; dest++) 			// send to each node
		{	MPI_Send(buff, limit, MPI_CHAR,
				dest, tag, MPI_COMM_WORLD);
		} // end for
		printf("\n[Multi Core Polard Rho Floyd ]\n  Processing with %d cores\n", cores);
	}else{												// a compute node
		MPI_Recv(buff, limit, MPI_CHAR,
			source, tag, MPI_COMM_WORLD, &status);		// receive buff
	} // end if/else

	mpz_set_str(n, buff, 10);							// move buff to 'n'








   	/* Initialize starting variables ******************************************/
/*	int i;	// assign seed to rank
	for(i=0; i<=rank; i++)
	{	do // calculate seed
		{	mpz_mul_ui(seed, seed, w);					// seed * w
			mpz_add_ui(seed, seed, 1);					// seed + 1
		} while(mpz_cmp(seed, n) < 0);
		mpz_mod(seed, seed, n);							// seed % n
	}
*/	mpz_init_set_ui(t, rank);
	mpz_init_set_ui(h, rank);

   	/* Algorithm **************************************************************/
	do
	{	mpz_mul_ui(t, t, w);		// tortus			// x * w
		mpz_add_ui(t, t, 1);							// x + 1
		mpz_mod(t, t, n);								// %n

		mpz_mul_ui(h, h, w);		// hare	1 of 2		// y * w
		mpz_add_ui(h, h, 1);							// y + 1
		mpz_mod(h, h, n);								// %n

		mpz_mul_ui(h, h, w);		// hare	2 of 2		// y * w
		mpz_add_ui(h, h, 1);							// y + 1
		mpz_mod(h, h, n);								// %n

		mpz_sub(z, h, t);			// test				// z = h - t
		mpz_abs(z, z);									// abs z
		mpz_gcd(f,z, n);								// f = gcd z n
	} while(mpz_cmp_ui(f, 1) == 0);

	/* Output *****************************************************************/
	printf("rank %d\t factor = ", rank);				// core reports
	mpz_out_str(stdout, BASE, f);		printf("\n");	// output p

	/* Clean up ***************************************************************/
	MPI_Finalize();										// shut down MPI
	return 0;
} // end main
