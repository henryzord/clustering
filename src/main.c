#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "measures/sswc.h"

float *to_matrix(char *path, int *n_lines, int *n_columns) {
    FILE *file = fopen(path, "r");  // TODO change in the future!

    char c = NULL, str[65536];  // maximum size of line. it's pretty big!
    int str_counter = 0;

    *n_lines = 0;
    *n_columns = 0;

    int line_c = 0, column_c;

    float *matrix = NULL;

    // gets number of dimensions
    if (file) {
        bool count_columns = false;

        while ((c = (char) getc(file)) != EOF) {
            if((c == ',') && (count_columns == false)) {
                *n_columns += 1;
            }
            if(c == '\n') {
                if(count_columns == false) {
                    *n_columns += 1;  // newline adds one column
                }
                count_columns = true;
                *n_lines += 1;
            }
        }
        *n_lines += 1;  // EOF adds one line
    }
    rewind(file);

    // allocates ands reads matrix

    matrix = (float*)malloc(sizeof(float) * *n_lines * *n_columns);

    if (file) {
        while ((c = (char)getc(file)) != EOF) {
            if(c == '\n') {
                column_c = 0;
                for (char *p = strtok(str, ","); p != NULL; p = strtok(NULL, ",")) {
                    float val = (float)atof(p);
                    matrix[line_c * *n_columns + column_c] = val;
                    column_c += 1;
                }
                str_counter = 0;
                line_c += 1;
            } else {
                str[str_counter] = c;
                str_counter += 1;
            }
        }
        fclose(file);
    } else {
        printf("Error reading file!");
    }
    return matrix;
}

void print_matrix(float *matrix, int n_lines, int n_columns) {
    int n = 0;
    while(n < n_lines * n_columns) {
        printf("%.2f\t", matrix[n]);
        n += 1;
        if ((n % n_columns == 0) && (n > 0)) {
            printf("\n");
        }
    }
}

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
    int n_lines, n_columns;

    float *dataset = to_matrix("../datasets/iris.csv", &n_lines, &n_columns);

    int *dummy_partition = randint(n_lines, 0, 1);
    dummy_partition[49] = 1;
    dummy_partition[99] = 1;
    dummy_partition[149] = 1;

    float val = sswc(dummy_partition, dataset, n_lines, n_columns);
    printf("val: %f\n", val);

//    printf("n_lines: %d n_columns: %d\n", n_lines, n_columns);
    free(dataset);
    return 0;
}