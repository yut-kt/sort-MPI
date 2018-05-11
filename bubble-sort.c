#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define N 50000

double gettime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((double) (tv.tv_sec) + (double) (tv.tv_usec) * 0.001 * 0.001);
}

int main(int argc, char *argv[]) {
    int array[N];
    for (int i = 0; i < N; i++) {
        array[i] = rand() % 10000;
    }

    double st = gettime();
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < N - 1; j++) {
            if (array[j] > array[j + 1]) {
                int tmp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = tmp;
            }
        }
    }
    double et = gettime();

    printf("%lf\n", et - st);

    return 0;
}