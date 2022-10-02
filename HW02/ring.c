/* 
 * COMP5055 Homework 2
 * Roy F. Cruz Candelaria
 */

/*  DOCUMENTATION
 *  
 *    File:
 *       ring.c
 *
 *    Purpose:
 *       Implementation ring message passing among p
 *       processors
 *         
 *    Compile:  
 *       mpicc -g -o ring.c ring
 * 
 *    Run:      
 *       mpiexec -n <number of processes> ring
 *
 *    Input:
 *       n/a
 * 
 *    Output:
 *       Message at each stage of the ring shaped transfer
 *       between the processes.
 *
 *    Errors:
 *       If any arguments are passed to the execution of
 *       ring, the program aborts and a error message is
 *       printed.
 *
 *    Notes:     
 *       n/a
 * 
 *    Github: 
 *       https://github.com/roy-cruz/COMP5055-HWs/tree/main/HW02
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

void w2c(int comm_sz, int my_rank);
void err_check(int p_err, char msg[], MPI_Comm comm);

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int comm_sz, my_rank;
    int err = 0;
    
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (argc == 1) {
        w2c(comm_sz, my_rank);
    } else {
        err = 1;
        err_check(
            err, 
            "Incorrect execution. Usage: \n Write to console: mpiexec -n <number of processes> ring \n",
            MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

/*-------------------------------------------------------------------
 *  Function:   
 *      w2c (i.e. write to console)
 * 
 *  Purpose:
 *      Initializes a string variable in core 0 which is then sent
 *      to core 1, which sends it to core 2, etc., until the string
 *      reaches core p-1, at which point it is sent back to core 0.
 *      At each of these steps, the recieving core appends to the
 *      message and prints the appended version of the message.
 *             
 *  Arguments:
 *      comm_sz: size of communicator
 *      my_rank: rank of processor
 *
 *  Note: n/a
 *    
 */
void w2c(
        int comm_sz,    /* in */
        int my_rank     /* in */) {

    char my_msg[100], msg[100];
    sprintf(my_msg, "Rank %d", my_rank);

    if (my_rank == 0) { // Rank 0 initializes message to be sent around
        sprintf(msg, "%s", my_msg);
    } else { // Other processes wait to recieve message from process my_rank-1. After recieving, they append their contribution to the message
        MPI_Recv(msg, 100, MPI_CHAR, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%s\n", msg);
        sprintf(msg, "%s %s", msg, my_msg);
    }

    // Chain sending/recieving starts.
    if (my_rank < comm_sz - 1) {
        MPI_Send(msg, 100, MPI_CHAR, my_rank+1, 0, MPI_COMM_WORLD);
    } else {
        MPI_Send(msg, 100, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    if (my_rank == 0) {
        MPI_Recv(msg, 100, MPI_CHAR, comm_sz - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%s\n", msg);
        printf("Communication complete!\n");
    }
} /* w2c */

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
            fprintf(stderr, "ERROR: %s\n\n", msg);
            fflush(stderr);
        }
        MPI_Finalize();
        exit(-1);
    }
} /* err_check */

