#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

void print_process_array();

int main(void) {
    int my_rank, comm_sz;
    const int ARR_SIZE = 5;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    srand(time(NULL) * my_rank);

    int i, elem, randnums[ARR_SIZE];
    
    for (i = 0; i < ARR_SIZE; i++) {
            randnums[i] = rand() % 100;
    }
    for (i = 0; i < comm_sz; i++) {
        if (my_rank == i) {
            printf("Rank: %d -> Numbers:    ", i);
            for (elem = 0; elem < ARR_SIZE; elem++) {
                printf("%d  ", randnums[elem]);
            }
            printf("\n");
        }
    }

    int temp_max_val, max_val;
    MPI_Reduce(&randnums[0], &max_val, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    for (i = 1; i < ARR_SIZE; i++) {
        MPI_Reduce(&randnums[i], &temp_max_val, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
        if (temp_max_val > max_val) {
            max_val = temp_max_val;
        }
    }
    if (my_rank == 0) {
        printf("Max value: %d \n", max_val);
    }

    MPI_Finalize();
    return 0;
}