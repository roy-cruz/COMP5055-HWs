/* 
 * COMP5055 Homework 2
 * Roy F. Cruz Candelaria
 */

/*  DOCUMENTATION
 *  
 *    File:
 *       sum.c
 *
 *    Purpose:
 *       Implementation of tree-structured sum in MPI.
 *
 *    Compile:  
 *       mpicc -g -o sum.c sum
 * 
 *    Run:      
 *       mpiexec -n <number of processes> sum
 *
 *    Input:
 *       n/a
 * 
 *    Output:
 *       Random integers generated by each of the processes.
 *       Sum of the random numbers generated.
 *
 *    Errors:
 *       n/a
 *
 *    Notes:     
 *       n/a
 * 
 *    Github: 
 *       https://github.com/roy-cruz/COMP5055-HWs/tree/main/HW02
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

const int MAX_CONTRIB = 10;

int Global_sum(int my_contrib, int my_rank, int comm_size);
void err_check(int p_err, char msg[], MPI_Comm comm);

int main(int argc, char* argv[]) {
   MPI_Init(&argc, &argv);

   int comm_size, my_rank;
   MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   int err = 0;

   int my_contrib;
   int sum = 0;

   double pow2_test = log(comm_size)/log(2);
   if (floorf(pow2_test) != pow2_test)
      err = 1;
   err_check(err, "Number of processes must be a power of 2.\n\n", MPI_COMM_WORLD);

   srandom(time(NULL) * my_rank);
   my_contrib = random() % MAX_CONTRIB;
   printf("Proc %d > my_contrib = %d\n", my_rank, my_contrib);

   sum = Global_sum(my_contrib, my_rank, comm_size);

   if (my_rank == 0)
      printf("Proc %d > global sum = %d\n", my_rank, sum);

   MPI_Finalize();
   return 0;
}

/*-------------------------------------------------------------------
 *    Function:   
 *       Global_sum
 * 
 *    Purpose:
 *       Sums numbers stored in each of the processes using a
 *       tree-structured sum.
 *             
 *    Arguments:
 *       my_contrib: contribution of a process to the total sum
 *       my_rank: rank of the process
 *       comm_size: size of communicator
 *
 *    Note:
 *       Due to the design of the implementation the number of 
 *       processes inolved must be equal to a power of 2.
 *    
 */

int Global_sum(
      int my_contrib,   /* in */ 
      int my_rank,      /* in */
      int comm_size     /* in */) {
   int sum = my_contrib;
   int temp;
   int done = 0;
   unsigned int partner;
   int stages = (int)(log(comm_size)/log(2));

   for (int i = 0; i < stages; i++) {

      partner = my_rank ^ (int)(pow(2, i)); 

      if (!(done)) {
         if (my_rank > partner){
            MPI_Send(&sum, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            done = 1;
         } else {
            MPI_Recv(&temp, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum = sum + temp;
         }
      }   
   }
   return sum;
}  /* Global_sum */

/*-------------------------------------------------------------------
 *  Function:   
 *      err_check
 * 
 *  Purpose:
 *      Checks for errors and terminates the program if one is
 *      detected in any of the processes.
 *             
 *  Arguments:
 *      p_err: error flag
 *      msg: error message to be printed if an error is detected
 *      comm: communicator
 *
 *  Note: n/a
 *    
 */
void err_check(
        int p_err,      /* in */
        char msg[],     /* in */
        MPI_Comm comm   /* in */) {

    int err;
    MPI_Allreduce(&p_err, &err, 1, MPI_INT, MPI_MAX, comm);
    if (err) {
        int my_rank;
        MPI_Comm_rank(comm, &my_rank);
        if (my_rank == 0) {
            fprintf(stderr, "ERROR: %s", msg);
            fflush(stderr);
        }
        MPI_Finalize();
        exit(-1);
    }
} /* err_check */

