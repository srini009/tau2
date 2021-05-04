/******************************************************************************
*   OpenMp Example - Matrix Multiply - C Version
*   Demonstrates a matrix multiply using OpenMP. 
*
*   Modified from here:
*   https://computing.llnl.gov/tutorials/openMP/samples/C/omp_mm.c
*
*   For  PAPI_FP_INS, the exclusive count for the event: 
*   for (null) [OpenMP location: file:matmult.c ]
*   should be  2E+06 / Number of Threads 
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "matmult_initialize.h"

#ifdef TAU_MPI
int provided;
#include <mpi.h>
/* NOTE: MPI is just used to spawn multiple copies of the kernel to different ranks.
This is not a parallel implementation */
#endif /* TAU_MPI */

#define ITERATIONS 100

#ifndef MATRIX_SIZE
#define MATRIX_SIZE 256
#endif

#define NRA MATRIX_SIZE                 /* number of rows in matrix A */
#define NCA MATRIX_SIZE                 /* number of columns in matrix A */
#define NCB MATRIX_SIZE                 /* number of columns in matrix B */

double** allocateMatrix(int rows, int cols) {
  int i;
  double **matrix = (double**)malloc((sizeof(double*)) * rows);
  for (i=0; i<rows; i++) {
    matrix[i] = (double*)malloc((sizeof(double)) * cols);
  }
  return matrix;
}

void freeMatrix(double** matrix, int rows, int cols) {
  int i;
  for (i=0; i<rows; i++) {
    free(matrix[i]);
  }
  free(matrix);
}

#ifdef APP_USE_INLINE_MULTIPLY
__inline double multiply(double a, double b) {
	return a * b;
}
#endif /* APP_USE_INLINE_MULTIPLY */

#if 0
// cols_a and rows_b are the same value
void compute_nested(double **a, double **b, double **c, int rows_a, int cols_a, int cols_b) {
  int i,j,k;
  double tmp = 0.0;
//num_threads(2)
#pragma omp parallel private(i) shared(a,b,c) 
  {
    /*** Do matrix multiply sharing iterations on outer loop ***/
    /*** Display who does which iterations for demonstration purposes ***/
#pragma omp for nowait schedule(dynamic,1)
    for (i=0; i<rows_a; i++) {
//num_threads(2)
#pragma omp parallel private(i,j,k) shared(a,b,c) 
      {
#pragma omp for nowait schedule(dynamic,1)
        for (k=0; k<cols_a; k++) {
          for(j=0; j<cols_b; j++) {
#ifdef APP_USE_INLINE_MULTIPLY
              c[i][j] += multiply(a[i][k], b[k][j]);
#else 
              tmp = a[i][k];
			  tmp = tmp * b[k][j];
              c[i][j] += tmp;
#endif 
            }
          }
      }
    }
  }   /*** End of parallel region ***/
}
#endif

// cols_a and rows_b are the same value
void compute(double **a, double **b, double **c, int rows_a, int cols_a, int cols_b) {
  int i,j,k;
#pragma omp parallel private(i,j,k) shared(a,b,c)
  {
    /*** Do matrix multiply sharing iterations on outer loop ***/
    /*** Display who does which iterations for demonstration purposes ***/
#pragma omp for schedule(dynamic) nowait
    for (i=0; i<rows_a; i++) {
      for(j=0; j<cols_b; j++) {
        for (k=0; k<cols_a; k++) {
#ifdef APP_USE_INLINE_MULTIPLY
          c[i][j] += multiply(a[i][k], b[k][j]);
#else /* APP_USE_INLINE_MULTIPLY */
          c[i][j] += a[i][k] * b[k][j];
#endif /* APP_USE_INLINE_MULTIPLY */
        }
      }
    }
  }   /*** End of parallel region ***/
}

void compute_interchange(double **a, double **b, double **c, int rows_a, int cols_a, int cols_b) {
  int i,j,k;
#pragma omp parallel private(i,j,k) shared(a,b,c)
  {
    /*** Do matrix multiply sharing iterations on outer loop ***/
    /*** Display who does which iterations for demonstration purposes ***/
#pragma omp for schedule(dynamic) nowait
    for (i=0; i<rows_a; i++) {
      for (k=0; k<cols_a; k++) {
        for(j=0; j<cols_b; j++) {
#ifdef APP_USE_INLINE_MULTIPLY
          c[i][j] += multiply(a[i][k], b[k][j]);
#else /* APP_USE_INLINE_MULTIPLY */
          c[i][j] += a[i][k] * b[k][j];
#endif /* APP_USE_INLINE_MULTIPLY */
        }
      }
    }
  }   /*** End of parallel region ***/
}

double do_work(void) {
  double **a,           /* matrix A to be multiplied */
  **b,           /* matrix B to be multiplied */
  **c;           /* result matrix C */
  a = allocateMatrix(NRA, NCA);
  b = allocateMatrix(NCA, NCB);
  c = allocateMatrix(NRA, NCB);  

/*** Spawn a parallel region explicitly scoping all variables ***/

  initialize(a, NRA, NCA);
  initialize(b, NCA, NCB);
  initialize(c, NRA, NCB);

#ifdef TAU_MPI
  //MPI_Barrier(MPI_COMM_WORLD);
#endif /* TAU_MPI */

  compute(a, b, c, NRA, NCA, NCB);

#ifdef TAU_MPI
  //MPI_Barrier(MPI_COMM_WORLD);
#endif /* TAU_MPI */

  compute_interchange(a, b, c, NRA, NCA, NCB);

  double result = c[0][1];

  freeMatrix(a, NRA, NCA);
  freeMatrix(b, NCA, NCB);
  freeMatrix(c, NCA, NCB);

#ifdef TAU_MPI
  //MPI_Barrier(MPI_COMM_WORLD);
#endif /* TAU_MPI */

  return result;
}

//int Tau_dump(void);

int main (int argc, char *argv[]) 
{

#ifdef TAU_MPI
  int rc = MPI_SUCCESS;
  int rank = 0;
  int comm_size = 0;
  rc = MPI_Init(&argc, &argv); 
  if (rc != MPI_SUCCESS) {
    char *errorstring;
    int length = 0;
    MPI_Error_string(rc, errorstring, &length);
    printf("Error: MPI_Init failed, rc = %d\n%s\n", rc, errorstring);
    exit(1);
  }
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif /* TAU_MPI */

#ifdef TAU_MPI
    // create a communicator
    /* The code above only works with 4 or more processes!! */
    if (comm_size >=4 ) {
      MPI_Group group_world, odd_group, even_group, diff_group, 
                union_group, inter_group, re_group, ri_group;
      int j, Neven, Nodd, members[8], ierr;

      MPI_Comm_group(MPI_COMM_WORLD, &group_world);
      MPI_Comm world_comm;
      MPI_Comm_create(MPI_COMM_WORLD, group_world, &world_comm);

      Neven = (comm_size+1)/2;    /* processes of MPI_COMM_WORLD are divided */
      Nodd = comm_size - Neven;   /* into odd- and even-numbered groups */
      for (j=0; j < Neven; j++) {   /* "members" determines members of even_group */
        members[j] = 2*j;
      };
    
      MPI_Group_incl(group_world, Neven, members, &even_group);
      MPI_Group_excl(group_world, Neven, members, &odd_group);
      MPI_Comm even_comm;
      MPI_Comm odd_comm;
      MPI_Comm_create(MPI_COMM_WORLD, even_group, &even_comm);
      MPI_Comm_create(MPI_COMM_WORLD, odd_group, &odd_comm);
      MPI_Group_difference(group_world, even_group, &diff_group);
      MPI_Group_intersection(group_world, odd_group, &inter_group);
      MPI_Group_union(group_world, odd_group, &union_group);
      int range[2][3] = {{0,1,1},{2,3,1}};
      MPI_Group_range_excl(group_world, 2, range, &re_group);
      MPI_Group_range_incl(group_world, 2, range, &ri_group);
      int ranks[2] = {0,1};
      int ranks_out[2] = {0};
      MPI_Group_translate_ranks(group_world, 2, ranks, union_group, ranks_out);
    } 

#endif /* TAU_MPI */

/* On thread 0: */
  int i;
  for (i = 0 ; i < ITERATIONS ; i++) {
    if(rank == 0 && i % 100 == 0) { printf("Iteration %d\n", i); }
    do_work();
    //Tau_dump();
  }

#ifdef TAU_MPI
  MPI_Finalize();
#endif /* TAU_MPI */
  printf ("Done.\n");

  return 0;
}

