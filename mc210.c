/*
 ============================================================================
 Name        : mc210.c
 Author      : Carl T. Knox
 Version     :
 Copyright   : Your copyright notice
 Description : 48 core version of a 210 spoke factoring MPI Wheel C
 ============================================================================
 mpicc -o "mc210" mc210.c -lgmp
 mpirun -np 48 mc210
 */
#include <stdio.h>
#include <stdlib.h>
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

	/* Assign Data ************************************************************/
	mpz_t n;		mpz_init_set_ui(n, 0);				// set to a Bignumber
	mpz_t x;		mpz_init_set_ui(x, 0);				// set to a Bignumber
	mpz_t ZERO;		mpz_init_set_ui(ZERO, 0);			// set to a Bignumber
	mpz_t TEMP;		mpz_init_set_ui(TEMP, 0);			// set to a Bignumber
//	int temp = 0;										// working register
	int limit = 230;									// maximum length
	char buff[limit];									// input string
	int tag = 0;    									// tag for messages
	int source = 0;										// rank of sender
	MPI_Status status;   								// status for received
	char* rtn;											// fgets return status
	int wheel[] = {1,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,	// 18
			79, 83, 89, 97, 101, 103, 107, 109, 113, 121, 127, 131, 137,	// 13
			139, 143, 149, 151, 157, 163, 167, 169, 173, 179, 181, 187,		// 12
			191, 193, 197, 199, 209 };					// wheel primes		//	5
			/* 42 Primes + 6 non-Primes(1,121,143,169,187,209) */

   	/* Input data & distribute it *********************************************/
	if(rank == 0) 										// the control node
	{	printf("\n rank %d input\n", rank);				// prompt input
		rtn = fgets(buff, limit, stdin);				// alternate input
		int dest;
		for(dest = 1; dest < cores; dest++) 			// send to each node
		{	MPI_Send(buff, limit, MPI_CHAR,
				dest, tag, MPI_COMM_WORLD);
		} // end for
		printf("\n[wheel mc210] Processing with %d cores\n", cores);
	}else{												// a compute node
		MPI_Recv(buff, limit, MPI_CHAR,
			source, tag, MPI_COMM_WORLD, &status);		// receive buff
	} // end if/else
	mpz_set_str(n, buff, 10);							// move buff to 'n'






   	/* Initialize starting point 'x' ******************************************/
	mpz_sqrt(x, n);										// x = iSqrt(n);
		// x *= 1/iSqrt(ratio = 1.6)
	mpz_mul_ui(TEMP, x, 79);  mpz_div_ui(x, TEMP, 100);	// x = (x * 79)/100
		// Adjust x to first spoke of the wheel
	mpz_mod_ui(TEMP, x, 210);							// x -= x % 210;
	mpz_sub(x, x, TEMP);								// adjust (x % 210) = 0
		// Assign rank to 1 of 52 spokes in wheel 210)
	mpz_add_ui(x, x, wheel[rank]);						// x += wheel[rank];

   	/* Algorithm **************************************************************/
	mpz_add_ui(x, x, 210);								// loop entry correction
	do
	{	mpz_sub_ui(x, x, 210);							// step = 210
		mpz_mod(TEMP, n, x);							// test
	} while(mpz_cmp_ui(TEMP, 0));						// continue step test

	/* Output *****************************************************************/
	printf("rank %d factor = ", rank);					// each core reports
	mpz_out_str(stdout, BASE, x);		printf("\n");

	/* Clean up ***************************************************************/
	MPI_Finalize();										// shut down MPI
	return 0;
} // end main

