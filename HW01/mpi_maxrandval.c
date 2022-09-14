/*
    Usage:
        mpiexec -n <number of processes> mpi_maxrandval <size of random int array for each process> <max value for ints>
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

void rand_array(int ARR_SIZE, int* my_randnums, int my_rank, int comm_sz, int max_rand_int); // Generates random array
void printiarr(int* array, int array_size);

int main(int argc, char *argv[]) {
    int my_rank, comm_sz;
    const int ARR_SIZE = atoi(argv[1]);
    const int MAX_VAL = atoi(argv[2]);
    int max_val[ARR_SIZE], my_randnums[ARR_SIZE];

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    rand_array(ARR_SIZE, my_randnums, my_rank, comm_sz, MAX_VAL);
    MPI_Reduce(my_randnums, max_val, ARR_SIZE, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("Row max val: ");
        printiarr(max_val, ARR_SIZE);
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
            printf("Rank:   %d", i);
            printf(";   Numbers:    ");
            printiarr(my_randnums, ARR_SIZE);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD); // Wait until all processes have generated and printed their arrays.
}

void printiarr(int* array, int array_size){
    for(int i = 0; i < array_size; i++) {
        printf("%d  ", array[i]);
    }
    printf("\n");
}
