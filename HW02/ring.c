/**
 * ring.c
 *
 * This program prints a hello message at each process. Modify it so that it
 * communicates in a ring fashion like so:
 *
 * ┌────┐     ┌────┐
 * │ R0 ├────>│ R1 │
 * └────┘     └────┘
 *    ^          │
 *    │          │
 *    │          V
 * ┌────┐     ┌────┐
 * │ R3 │<────│ R2 │
 * └────┘     └────┘
 *
 * Communication will start at Rank 0, with each subsequent rank listening for a
 * message and then sending the message on once it is received. Messages are
 * appended to one another, like:
 *
 * Rank0
 * Rank1 Rank0
 * Rank2 Rank1 Rank0
 * Rank3 Rank2 Rank1 Rank0
 * Communication complete!
 *
 * We talked about how we can't control the order in which strings print when we
 * use MPI. Is this true for this assignment?
 *
 * Compile:  mpicc -g -Wall -o ring ring.c
 * Run:      mpirun -n <num_procs> ./ring
 * Run:      mpirun -n <num_procs> --hostfile host_file.txt ./ring
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

void w2c(int comm_sz, int my_rank);
void w2f(int comm_sz, int my_rank, FILE* ofile);
void err_check(int p_err, char msg[], MPI_Comm comm);

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int comm_sz, my_rank;
    int err = 0;
    
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (argc == 1) {
        w2c(comm_sz, my_rank);
    } else if ((argc == 3) && !(strcmp(argv[1], "--hostfile"))) {
        FILE* ofile = fopen(argv[2], "a");
        if (ofile == NULL)
            err = 1;
        err_check(err, "Output file does not exist.", MPI_COMM_WORLD);
        w2f(comm_sz, my_rank, ofile);
    } else {
        err = 1;
        err_check(
            err, 
            "Incorrect execution. Usage: \n Write to console: mpiexec -n <number of processes> ring \n Write to file: mpiexec -n <number of processes> --hostfile <file name> ring",
            MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

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
}

void w2f(
        int comm_sz,    /* in */ 
        int my_rank,    /* in */
        FILE* ofile       /* in */) {

    char my_msg[100], msg[100];
    sprintf(my_msg, "Rank %d", my_rank);

    if (my_rank == 0) { // Rank 0 initializes message to be sent around
        sprintf(msg, "%s", my_msg);
    } else { // Other processes wait to recieve message from process my_rank-1. After recieving, they append their contribution to the message
        MPI_Recv(msg, 100, MPI_CHAR, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        fprintf(ofile, "%s\n", msg);
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
        fprintf(ofile, "%s\n", msg);
        fprintf(ofile, "Communication complete!\n");
    }
} /* w2f */


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

