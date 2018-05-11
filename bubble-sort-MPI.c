#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define N 50000
#define MAX 9999

double gettime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((double) (tv.tv_sec) + (double) (tv.tv_usec) * 0.001 * 0.001);
}

int array[N];

int swaps(int front, int back, int max) {
    int last_index;
    while (front < back) {
        while (array[front] <= max && front < back) front++;
        while (array[back] > max && front < back) back--;
        if (front >= back) break;
        int tmp = array[front];
        array[front] = array[back];
        array[back] = tmp;
        last_index = front;
    }

    return last_index;
}

void sort(int beginning, int ending) {
    for (int i = beginning; i < ending; i++) {
        for (int j = beginning; j < ending; j++) {
            if (array[j] > array[j + 1]) {
                int tmp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = tmp;
            }
        }
    }
}

void print_hundred(int beginning, int rank) {
    for (int i = beginning; i < beginning + 100; i++) {
        printf("%5d", array[i]);
//        break;
    }
    printf("\nrank:%d is sorted\n\n", rank);
}

int main(int argc, char *argv[]) {

    int rank, size, first_index = 0, last_index = 0;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int min = (MAX + rank) / size * rank;
    int max = (rank == size - 1) ? MAX : (MAX + rank + 1) / size * (rank + 1);

    if (rank == 0) {
        double st = gettime();
        for (int i = 0; i < N; i++) {
            array[i] = rand() % MAX + 1;
        }

        last_index = swaps(first_index, N - 1, max);

        last_index++;
        MPI_Send(&last_index, 1, MPI_INT, rank + 1, 10, MPI_COMM_WORLD);
        MPI_Send(&array[last_index], N - last_index, MPI_INT, rank + 1, 20, MPI_COMM_WORLD);
        last_index--;

        sort(first_index, last_index);

        MPI_Recv(&array[last_index + 1], N - last_index - 1, MPI_INT, rank + 1, 30, MPI_COMM_WORLD, &status);

        double et = gettime();
        printf("%lf\n", et - st);

    } else {
        MPI_Recv(&last_index, 1, MPI_INT, rank - 1, 10, MPI_COMM_WORLD, &status);
        MPI_Recv(&array[last_index], N - last_index, MPI_INT, rank - 1, 20, MPI_COMM_WORLD, &status);

        int first_index = last_index;
        last_index = swaps(last_index, N - 1, max);

        if (rank + 1 != size) {
            last_index++;
            MPI_Send(&last_index, 1, MPI_INT, rank + 1, 10, MPI_COMM_WORLD);
            MPI_Send(&array[last_index], N - last_index, MPI_INT, rank + 1, 20, MPI_COMM_WORLD);
            last_index--;
        } else {
            last_index = N - 1;
        }

        sort(first_index, last_index);

        if (rank == size - 1) {
            MPI_Send(&array[first_index], N - first_index, MPI_INT, rank - 1, 30, MPI_COMM_WORLD);
        } else {
            MPI_Recv(&array[last_index + 1], N - last_index - 1, MPI_INT, rank + 1, 30, MPI_COMM_WORLD, &status);
            MPI_Send(&array[first_index], N - first_index, MPI_INT, rank - 1, 30, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();

    return 0;
}