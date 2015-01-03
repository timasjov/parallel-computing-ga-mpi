/**
 * transpose of 1-d array.
 * E,g: (1 2 3 4 5 6 7 8 9 10) => (10 9 8 7 6 5 4 3 2 1)
 */
#define   TOTALELEMS   1000
#define   MAXPROC      128

#include <stdio.h>
#include <math.h>
#include "ga.h"
#include "macdecls.h"
#include <mpi.h>

void verify(int g_a, int g_b);

void TRANSPOSE1D() {
    
    int ndim, dims[1], chunk[1], ld[1], lo[1], hi[1];
    int lo1[1], hi1[1], lo2[1], hi2[1];
    int g_a, g_b, a[MAXPROC*TOTALELEMS],b[MAXPROC*TOTALELEMS];
    int nelem, i, q;    
    int me, nprocs;
    
    /* Find local processor ID and number of processors */
    me = GA_Nodeid();
    nprocs = GA_Nnodes();
	
	/* Configure array dimensions. Force an unequal data distribution */
	ndim     = 1; /* 1-d transpose */
	dims[0]  = nprocs*TOTALELEMS + nprocs/2;
	ld[0]    = dims[0];
	chunk[0] = TOTALELEMS; /* minimum data on each process */
 
	/* create a global array g_a and duplicate it to get g_b */
	g_a = NGA_Create(C_INT, 1, dims, "array A", chunk);
	if (!g_a) GA_Error("create failed: A", 0);
	
	g_b = GA_Duplicate(g_a, "array B");
	if (! g_b) GA_Error("duplicate failed", 0);
	
	/* initialize data in g_a */
	if (me==0) {
	   for(i=0; i<dims[0]; i++) a[i] = i;

	   lo[0]  = 0;
	   hi[0] = dims[0]-1;
	   NGA_Put(g_a, lo, hi, a, ld);
	}

	/* Synchronize all processors to guarantee that everyone has data
	   before proceeding to the next step. */
	GA_Sync();

	/* Start initial phase of inversion by inverting the data held locally on
	   each processor. Start by finding out which data each processor owns. */
	NGA_Distribution(g_a, me, lo1, hi1);

	/* Get locally held data and copy it into local buffer a  */
	NGA_Get(g_a, lo1, hi1, a, ld);
	
	/* Invert data locally */
	nelem = hi1[0] - lo1[0] + 1;
	for (i=0; i<nelem; i++) b[i] = a[nelem-1-i];
	
	/* Invert data globally by copying locally inverted blocks into
	 * their inverted positions in the GA */
	lo2[0] = dims[0] - hi1[0] -1;
	hi2[0] = dims[0] - lo1[0] -1;
	NGA_Put(g_b,lo2,hi2,b,ld);

	/* Synchronize all processors to make sure inversion is complete */
	GA_Sync();
	
	
	
	/* Deallocate arrays */
	GA_Destroy(g_a);
	GA_Destroy(g_b);

}

int main(int argc, char **argv) {
    int heap=300000, stack=300000;
    int me, nprocs, i;
    double start, end;
	
    /* Initialize Message Passing library */
    MPI_Init(&argc, &argv);

    /* Initialize GA */
    GA_Initialize();
    
    /* Initialize Memory Allocator (MA) */
    if(! MA_init(C_DBL, stack, heap) ) GA_Error("MA_init failed",stack+heap);

    me     = GA_Nodeid();
    nprocs = GA_Nnodes();
    if(me==0) {
       printf("\nUsing %d processes\n\n", nprocs); fflush(stdout);
    }
     
	start = MPI_Wtime();
	for(i =0; i< 100; i++) {
		TRANSPOSE1D();
	}
	end = MPI_Wtime();
	if(me==0) printf("  Time=%2.5e secs\n\n",end-start); 
    
    if(me==0)printf("\nTerminating ..\n");
    GA_Terminate();
    MPI_Finalize();    
}
