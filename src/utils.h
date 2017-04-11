#ifndef CLUSTERING_UTILS_H
#define CLUSTERING_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void print_float_array(float *array, int size) {
    for(int n = 0; n < size; n++) {
        printf("%f\t", array[n]);
    }
    printf("\n");
}

void print_int_array(int *array, int size) {
    for(int n = 0; n < size; n++) {
        printf("%d\t", array[n]);
    }
    printf("\n");
}

void print_matrix(float *matrix, int n_lines, int n_columns, bool show_index) {
    int n = 0, line_c = 0;

    while(n < n_lines * n_columns) {
        if((n % n_columns == 0) && show_index) {
            printf("%05.d: ", line_c);
        }

        printf("%05.4f\t\t", matrix[n]);

        n += 1;
        if ((n % n_columns == 0) && (n > 0)) {
            line_c += 1;
            printf("\n");
        }
    }
}

float *read_dataset(char *path, int *n_lines, int *n_columns) {
    FILE *file = fopen(path, "r");

    char c, str[65536];  // maximum size of line. it's pretty big!
    int str_counter = 0;

    *n_lines = 0;
    *n_columns = 0;

    int line_c = 0, column_c;

    float *matrix = NULL;

    // gets number of dimensions
    if (file) {
        bool count_columns = false;

        while ((c = (char)getc(file)) != EOF) {
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
        while (true) {
            c = (char)getc(file);
            if((c == '\n') || (c == EOF)) {
                column_c = 0;
                for (char *p = strtok(str, ","); p != NULL; p = strtok(NULL, ",")) {
                    float val = (float)atof(p);
                    matrix[line_c * *n_columns + column_c] = val;
                    column_c += 1;
                }
                str_counter = 0;
                line_c += 1;

                if(c == EOF) {
                    break;
                }
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

#endif //CLUSTERING_UTILS_H
