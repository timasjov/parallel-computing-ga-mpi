/**
 * Multiplication of two square matrices with randomly generated
 * contents
 */

#define   NDIM         2
#define   TOTALELEMS   5000

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ga.h"
#include "macdecls.h"
#include <mpi.h>

double a[TOTALELEMS][TOTALELEMS], b[TOTALELEMS][TOTALELEMS];
double c[TOTALELEMS][TOTALELEMS], btrns[TOTALELEMS][TOTALELEMS];

void verify(int g_a, int g_b, int g_c, int *lo, int *hi, int *ld);
void matrix_multiply() {
    
    int dims[NDIM], chunk[NDIM], ld[NDIM];
    int lo[NDIM], hi[NDIM], lo1[NDIM], hi1[NDIM];
    int lo2[NDIM], hi2[NDIM], lo3[NDIM], hi3[NDIM];
    int g_a, g_b, g_c;
    int i, j, k;

    /* Find local processor ID and the number of processors */
    int me=GA_Nodeid(), nprocs=GA_Nnodes();
    
    /* Configure array dimensions */
    for(i=0; i<NDIM; i++) {
       dims[i]  = TOTALELEMS;
       ld[i]    = dims[i];
       chunk[i] = TOTALELEMS/nprocs-1; /*minimum block size on each process*/
    }
 
    /* Create a global array g_a and duplicate it to get g_b and g_c*/
    g_a = NGA_Create(C_DBL, NDIM, dims, "array A", chunk);
    if (!g_a) GA_Error("create failed: A", NDIM);
    if (me==0) printf("  Created Array A\n");
    
    g_b = GA_Duplicate(g_a, "array B");
    g_c = GA_Duplicate(g_a, "array C");
    if (!g_b || !g_c) GA_Error("duplicate failed",NDIM);
    if (me==0) printf("  Created Arrays B and C\n");
 
    /* Initialize data in matrices a and b */
    if(me==0)printf("  Initializing matrix A and B\n");
    for(i=0; i<dims[0]; i++) {
       for(j=0; j<dims[1]; j++) {
          a[i][j] = i+j;
          b[i][j] = i*j;
       }
    }

    /*  Copy data to global arrays g_a and g_b */
    lo1[0] = 0;
    lo1[1] = 0;
    hi1[0] = dims[0]-1;
    hi1[1] = dims[1]-1;
    if (me==0) {
       NGA_Put(g_a, lo1, hi1, a, ld);
       NGA_Put(g_b, lo1, hi1, b, ld);
    }
    
    /*  Synchronize all processors to make sure everyone has data */
    GA_Sync();

    /* Determine which block of data is locally owned. Note that
       the same block is locally owned for all GAs. */
    NGA_Distribution(g_c, me, lo, hi);
    
    /* Get the blocks from g_a and g_b needed to compute this block in
       g_c and copy them into the local buffers a and b. */
    lo2[0] = lo[0];
    lo2[1] = 0;
    hi2[0] = hi[0];
    hi2[1] = dims[0]-1;
    NGA_Get(g_a, lo2, hi2, a, ld);
    
    lo3[0] = 0;
    lo3[1] = lo[1];
    hi3[0] = dims[1]-1;
    hi3[1] = hi[1];
    NGA_Get(g_b, lo3, hi3, b, ld);

    /* Do local matrix multiplication and store the result in local
       buffer c. Start by evaluating the transpose of b. */
    for(i=0; i < hi3[0]-lo3[0]+1; i++)
       for(j=0; j < hi3[1]-lo3[1]+1; j++) 
          btrns[j][i] = b[i][j];

    /* Multiply a and b to get c */
    for(i=0; i < hi[0] - lo[0] + 1; i++) {
       for(j=0; j < hi[1] - lo[1] + 1; j++) {
          c[i][j] = 0.0;
          for(k=0; k<dims[0]; k++)
             c[i][j] = c[i][j] + a[i][k]*btrns[j][k];
       }
    }
    
    /* Copy c back to g_c */
    NGA_Put(g_c, lo, hi, c, ld);

    verify(g_a, g_b, g_c, lo1, hi1, ld);
    
    /* Deallocate arrays */
    GA_Destroy(g_a);
    GA_Destroy(g_b);
    GA_Destroy(g_c);
}

/*
 * Check to see if inversion is correct.
 */
#define TOLERANCE 0.1
void verify(int g_a, int g_b, int g_c, int *lo, int *hi, int *ld) {

    double rchk, alpha=1.0, beta=0.0;
    int g_chk, me=GA_Nodeid();

    g_chk = GA_Duplicate(g_a, "array Check");
    if(!g_chk) GA_Error("duplicate failed",NDIM);
    GA_Sync();

    GA_Dgemm('n', 'n', TOTALELEMS, TOTALELEMS, TOTALELEMS, 1.0, g_a,
             g_b, 0.0, g_chk);

    GA_Sync();
    
    alpha=1.0, beta=-1.0;
    GA_Add(&alpha, g_c, &beta, g_chk, g_chk);
    rchk = GA_Ddot(g_chk, g_chk);
    
    if (me==0) {
       printf("Normed difference in matrices: %12.4e\n", rchk);
       if(rchk < -TOLERANCE || rchk > TOLERANCE)
          GA_Error("Matrix multiply verify failed",0);
       else
          printf("Matrix Mutiply OK\n");
    }
    
    GA_Destroy(g_chk);
}


int main(int argc, char **argv) {
    int heap=3000000, stack=3000000;
    int me, nprocs;
    
    /* Initialize MPI */
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
 
   matrix_multiply();
    
    if(me==0)printf("\nTerminating ..\n");
    GA_Terminate();    
    MPI_Finalize();    
}