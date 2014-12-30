/**
 * Multiplication of two square matrices with randomly generated
 * contents
 */

#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>

#define TOTALELEMS 1000        /* size of the matrix */
#define MASTER 0               /* taskid of first task */
#define FROM_MASTER 1          /* setting a message type */
#define FROM_WORKER 2          /* setting a message type */

int main (int argc, char *argv[]){
int	numtasks,              		/* number of tasks in partition */
	taskid,                		/* a task identifier */
	numworkers,            		/* number of worker tasks */
	source,                		/* task id of message source */
	dest,                  		/* task id of message destination */
	mtype,                 		/* message type */
	rows,                  		/* rows of matrix A sent to each worker */
	averow, extra, offset, 		/* used to determine rows sent to each worker */
	i, j, k, rc;           		/* misc */
double	a[TOTALELEMS][TOTALELEMS],      /* matrix A to be multiplied */
	b[TOTALELEMS][TOTALELEMS],      /* matrix B to be multiplied */
	c[TOTALELEMS][TOTALELEMS];      /* result matrix C */
MPI_Status status;

MPI_Init(&argc,&argv);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
if (numtasks < 2 ) {
  printf("Need at least two MPI tasks. Quitting...\n");
  MPI_Abort(MPI_COMM_WORLD, rc);
  exit(1);
  }
numworkers = numtasks-1;


/**************************** master task ************************************/
   if (taskid == MASTER) {
      printf("mpi_mm has started with %d tasks.\n",numtasks);
      for (i=0; i<TOTALELEMS; i++) {
         for (j=0; j<TOTALELEMS; j++) {
            b[i][j]= i*j;
            a[i][j]= i+j;
      }
      /* Send matrix data to the worker tasks */
      averow = TOTALELEMS/numworkers;
      extra = TOTALELEMS%numworkers;
      offset = 0;
      mtype = FROM_MASTER;
      for (dest=1; dest<=numworkers; dest++)
      {
         rows = (dest <= extra) ? averow+1 : averow;   	
         printf("Sending %d rows to task %d offset=%d\n",rows,dest,offset);
         MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
         MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
         MPI_Send(&a[offset][0], rows*TOTALELEMS, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
         MPI_Send(&b, TOTALELEMS*TOTALELEMS, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
         offset = offset + rows;
      }

      /* Receive results from worker tasks */
      mtype = FROM_WORKER;
      for (i=1; i<=numworkers; i++)
      {
         source = i;
         MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
         MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
         MPI_Recv(&c[offset][0], rows*TOTALELEMS, MPI_DOUBLE, source, mtype, MPI_COMM_WORLD, &status);
         printf("Received results from task %d\n",source);
      }

      printf ("Done.\n");
   }


/**************************** worker task ************************************/
   if (taskid > MASTER) {
      mtype = FROM_MASTER;
      MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&a, rows*TOTALELEMS, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&b, TOTALELEMS*TOTALELEMS, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);

      for (k=0; k<TOTALELEMS; k++) {
         for (i=0; i<rows; i++) {
            c[i][k] = 0.0;
            for (j=0; j<TOTALELEMS; j++) {
               c[i][k] = c[i][k] + a[i][j] * b[j][k];
            }
         }
      }
      mtype = FROM_WORKER;
      MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
      MPI_Send(&c, rows*TOTALELEMS, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
   }
   MPI_Finalize();
}
