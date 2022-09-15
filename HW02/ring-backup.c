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

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    /* Total number of processes in this MPI communicator */
    int comm_sz;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    /* Get the rank of this process */
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Hostname */
    char hostname[MPI_MAX_PROCESSOR_NAME];
    int name_sz;
    MPI_Get_processor_name(hostname, &name_sz);

    printf("Hello from %s, rank %d (of %d).\n", hostname, rank, comm_sz);

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
    MPI_Finalize();
    return 0;
}
