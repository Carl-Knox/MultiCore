/*
 ============================================================================
 Name: mcFermat.c
 Author: Carl T. Knox
 Version:
 Copyright: Your copyright notice
 Description : Multi Core Fermat in C using gmp (GNU Multi-Precission math)
 ============================================================================
 mpicc -o "mcf" mcFermat.c -lgmp						// compile & link
 mpirun -np 64 mcf										// run
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
	int rank;											// rank this process
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);				// get process rank

	/* Assign Data ************************************************************/
	mpz_t n;		mpz_init_set_ui(n, 0);				// set to a Bignumber
	mpz_t a;		mpz_init_set_ui(a, 0);				// set to a Bignumber
	mpz_t b;		mpz_init_set_ui(b, 0);				// set to a Bignumber
	mpz_t l;		mpz_init_set_ui(l, 0);				// set to a Bignumber
	mpz_t p;		mpz_init_set_ui(p, 0);				// set to a Bignumber
	mpz_t q;		mpz_init_set_ui(q, 0);				// set to a Bignumber
	int limit = 230;									// maximum length
	char buff[limit];									// input string
	int tag = 0;										// tag for messages
	int source = 0;										// rank of sender
	MPI_Status status;									// status for received
	char* rtn;											// fgets return status

	/* Input data & distribute it *********************************************/
	if(rank == 0)										// the control node
	{	printf("\n rank %d input\n", rank);				// prompt input
		rtn = fgets(buff, limit, stdin);				// alternate input
		int dest;
		for(dest = 1; dest < cores; dest++)				// send to each node
		{
			MPI_Send(buff, limit, MPI_CHAR,
			dest, tag, MPI_COMM_WORLD);
		} // end for
		printf("\n[Multi Core Fermat] Processing with %d cores\n", cores);
	}else{												// a compute node
		MPI_Recv(buff, limit, MPI_CHAR,
		source, tag, MPI_COMM_WORLD, &status);			// receive buff
	} // end if/else
	mpz_set_str(n, buff, 10);							// string to integer n











	/* Initialize starting point 'a' ******************************************/
	mpz_sqrt(a, n);										// a = integer √n

	/* Algorithm **************************************************************/
	mpz_add_ui(a, a, rank + 1);							// adjust for rank
	mpz_mul(l, a, a);									// l = a²
	mpz_sub(l, l, n);									// l = n compliment
	while(0 == mpz_perfect_square_p(l))					// ! perfect square
	{	mpz_add_ui(a, a, cores);						// step cores
		mpz_mul(l, a, a);								// l = a²
		mpz_sub(l, l, n);								// l = n compliment
	}
	mpz_sqrt(b, l);										// assign b = √l
	mpz_add(p, a, b);									// define p
	mpz_sub(q, a, b);									// define q

	/* Output *****************************************************************/
	printf("rank %d\nfactor p = \n", rank);				// core reports
	mpz_out_str(stdout, BASE, p);	printf("\n");		// output p
	printf("factor q =\n");
	mpz_out_str(stdout, BASE, q);	printf("\n");		// output q
	
	/* Clean up ***************************************************************/
	MPI_Finalize();										// shut down MPI
	return 0;
} // end main
