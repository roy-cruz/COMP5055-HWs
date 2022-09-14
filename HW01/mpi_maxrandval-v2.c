/*
    Note: We must pass a whole array to MPI_Reduce
    Program must return an array of max values.
    Usage:
        mpiexec -n <number of processes> mpi_maxrandval <number of random ints per process> <max value for ints>
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

void rand_array(int ARR_SIZE, int* my_randnums, int my_rank, int comm_sz, int max_rand_int);
int multiprocess_max(int ARR_SIZE, int* my_randnums);

int main(int argc, char *argv[]) {
    int my_rank, comm_sz;
    const int ARR_SIZE = atoi(argv[1]);
    int max_val, my_randnums[ARR_SIZE];

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    rand_array(ARR_SIZE, my_randnums, my_rank, comm_sz, atoi(argv[1]));
    max_val = multiprocess_max(ARR_SIZE, my_randnums);

    if (my_rank == 0) {
        printf("Max value: %d \n", max_val);
    }

    MPI_Finalize();
    return 0;
}

void rand_array(int ARR_SIZE, int *my_randnums, int my_rank, int comm_sz, int max_rand_int) {
    /*
        Creates one array of ARR_SIZE random numbers for each process and has each process print it.
    */
    int i, elem;
    srand(time(NULL) * my_rank);
    for (i = 0; i < ARR_SIZE; i++) { // Generating array of random integers.
        my_randnums[i] = rand() % (max_rand_int + 1);
    }

    for (i = 0; i < comm_sz; i++) { // Each process prints its respective array.
        if (my_rank == i) {
            printf("Rank: %d", i);
            printf(";   Numbers: ");
            for (elem = 0; elem < ARR_SIZE; elem++) {
                printf("%d  ", my_randnums[elem]);
            }
            printf("\n");
        }
    }
    MPI_Barrier(MPI_COMM_WORLD); // Wait until all processes have generated and printed their arrays.
}

int multiprocess_max(int ARR_SIZE, int* my_randnums) {
    /*
        Finds and returns the maximum value among all the elements stored in arrays in each process.
    */
    int temp_max_val, max_val;
    for (int i = 0; i < ARR_SIZE; i++) {
        if (i == 0) {
            MPI_Reduce(&my_randnums[i], &max_val, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD); // Initializing max_val for future comparisons.
            continue;
        }
        MPI_Reduce(&my_randnums[i], &temp_max_val, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
        if (temp_max_val > max_val) {
            max_val = temp_max_val;
        }
    }
    return max_val;
}