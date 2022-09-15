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

#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int comm_sz, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    char my_msg[1000], recvd_msg[1000];
    int msg_size = 0;
    sprintf(my_msg, "Rank%d", my_rank);

    if ((my_rank == 0) && comm_sz > 1) {
        printf(my_msg);
        MPI_Send(my_msg, strlen(my_msg), MPI_CHAR, 1, 0, MPI_COMM_WORLD);
    } else {
        printf(my_msg);
        printf("\n");
        printf("Process done!");
        return 0;
    }

    for (int i = 1; i <= comm_sz; i++) 
        if (i == my_rank) {
            strcat(local_msg, " ");
            MPI_Recv(recvd_msg, 1000, MPI_CHAR, i-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            strcat(local_msg, recvd_msg);
            MPI_Send(local_msg, 1000, MPI_CHAR, i + 1)
        } else {
            continue;
        }
    }
    MPI_Finalize();
    return 0;
}

/* We need to modify this file so that we send point-to-point messages
    * between processes. We'll use MPI_Send and MPI_Recv... */

/* MPI_Send example:
    * char buffer[1000];
    * sprintf(buffer, "Hello world from %d\n", rank);
    * MPI_Send(buffer, 1000, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    *     Where i is the rank to send to
    *
    * MPI_Recv example:
    * char buffer[1000];
    * MPI_Recv(buffer, 1000, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    *    Where i is the rank to receive from
    */

/* Clean up */