/* 
 * COMP5055 Homework 1
 * Roy F. Cruz Candelaria
 */

/*  DOCUMENTATION
 *  
 *  File:
 *      mpi_maxrandval.c
 *
 *  Purpose:
 *      Find the maximum of each of the elements of a set of randomly
 *      generated arrays of random numbers, where each one of the arrays
 *      is stored in each of the processes.
 *
 *  Compile:  
 *      mpicc -g -o mpi_maxrandval.c mpi_maxrandval
 * 
 *  Run:      
 *      mpiexec -n <number of processes> mpi_maxrandval <size of random int array for each process> <max value for ints>
 *
 *  Input:
 *      Size of array of random integers
 *      Max value the maximum integers can have
 * 
 *  Output:
 *      Arrays of random integers generated by the processes.
 *      Array of maximum values among the arrays stored in each of the processes.
 *
 *  Errors:
 *      If an error is detected (incorrect number of input arguments), the
 *      program prints a message and all processes quit.
 *
 *  Notes:     
 *      n/a
 * 
 *  Github: 
 *      https://github.com/roy-cruz/COMP5055-HWs/blob/main/HW01/mpi_maxrandval.c
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

void rand_array(int ARR_SIZE, int* my_randnums, int my_rank, int comm_sz, int max_rand_int); // Generates random array
void printiarr(int* array, int array_size);
void err_check(int local_ok, char message[], MPI_Comm comm);

int main(int argc, char *argv[]) {
    int my_rank, comm_sz;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int err = 0;
    if (!(argc == 3)) {
        err = 1;
    }
    err_check(err,  
        "Incorrect number of arguments.\n Usage: mpiexec -n <number of processes> mpi_maxrandval <size of random int array for each process> <max value for ints>",
        MPI_COMM_WORLD);

    const int ARR_SIZE = atoi(argv[1]);
    const int MAX_VAL = atoi(argv[2]);
    int max_val[ARR_SIZE], my_randnums[ARR_SIZE];
    
    rand_array(ARR_SIZE, my_randnums, my_rank, comm_sz, MAX_VAL);
    MPI_Reduce(my_randnums, max_val, ARR_SIZE, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("Row max val: ");
        printiarr(max_val, ARR_SIZE);
    }

    MPI_Finalize();
    return 0;
}

/*-------------------------------------------------------------------
 *  Function:   
 *      rand_array
 * 
 *  Purpose:
 *      Has each process produce an array of random numbers of size
 *      ARR_SIZE and with max value of max_rand_int.
 *             
 *  Arguments:
 *      ARR_SIZE: size of the array of random numbers; set by 
 *          the user
 *      my_randnums: pointer to an array in which the random will be
 *          stored
 *      my_rank: rank of the process executing this function
 *      comm_sz: size of communicator
 *      max_rand_int: maximum size of the random integers
 *
 *  Note: n/a
 *    
 */
void rand_array(
        int ARR_SIZE,       /* in */
        int *my_randnums,   /* out */
        int my_rank,        /* in */
        int comm_sz,        /* in */
        int max_rand_int    /* in */) {

    int i, elem;
    srand(time(NULL) * my_rank);
    for (i = 0; i < ARR_SIZE; i++) { 
        my_randnums[i] = rand() % (max_rand_int + 1);
    }

    for (i = 0; i < comm_sz; i++) { 
        if (my_rank == i) {
            printf("Rank:   %d", i);
            printf(";   Numbers:    ");
            printiarr(my_randnums, ARR_SIZE);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
} /* rand_array */

/*-------------------------------------------------------------------
 *  Function:   
 *      printiarr
 * 
 *  Purpose:
 *      Prints an array of integers of arbitrary size.
 *             
 *  Arguments:
 *      array: pointer to the array to be printed
 *      array_size: size of the array to be printed
 *
 *  Note: n/a
 *    
 */
void printiarr(
        int* array,     /* in */
        int array_size  /* in */){

    for(int i = 0; i < array_size; i++) {
        printf("%d  ", array[i]);
    }
    printf("\n");
} /* printiarr */

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