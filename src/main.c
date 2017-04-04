#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "measures/sswc.h"

/**
 * Allocates an array with size values between hmin (inclusive) and hmax (exclusive)
 *
 * @param size Size of the array
 * @param hmin Minimum value (inclusive)
 * @param hmax Maximum value (exclusive)
 * @return An array with size length
 */
int *randint(int size, int hmin, int hmax) {
    int i;
    int *my_array = (int*)malloc(sizeof(int) * size);
    for (i = 0; i < size; i++) {
        my_array[i] = (rand() % hmax) + hmin;
    }
    return my_array;
}

int main(int argc, char **argv) {
    int n_objects, n_attributes;

    float *dataset = read_dataset("../datasets/iris.csv", &n_objects, &n_attributes);

    int *medoids = randint(n_objects, 0, 1);
    medoids[49] = 1;
    medoids[99] = 1;
    medoids[149] = 1;

    float val = sswc(medoids, dataset, n_objects, n_attributes);
    printf("sswc: %f\n", val);

    free(medoids);
    free(dataset);
    return 0;
}