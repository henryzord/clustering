#include <math.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * Calculates the euclidean distance between two data objects.
 * @param x1 First object
 * @param x2 Second object
 * @param n_attributes Number of attributes for both objects
 * @return The euclidean distance
 */
float get_euclidean_distance(float *x1, float *x2, int n_attributes) {
    float distance = 0;
    for(int n = 0; n < n_attributes; n++) {
        distance += pow(x1[n] - x2[n], (float)2);
    }
    return (float)sqrt(distance);
}

float *get_distance_matrix(float *dataset, int n_objects, int n_attributes) {
    float *matrix = (float*)malloc(sizeof(float) * n_objects * n_objects);

    for(int i = 0; i < n_objects; i++) {
        for(int j = 0; j <= i; j++) {
            float dist = get_euclidean_distance(
                    &dataset[i * n_attributes],
                    &dataset[j * n_attributes],
                    n_attributes - 1  // minus the class attribute
            );
            matrix[i * n_objects + j] = dist;
            matrix[j * n_objects + i] = dist;
        }
    }

    return matrix;
}

int *get_partition(int *medoids, float *dm, float *dataset, int n_objects, int n_attributes) {
    int *partition = (int*)malloc(sizeof(int) * n_objects);

    for(int i = 0; i < n_objects; i++) {
        float closest_dist = INFINITY; // sets to infinity
        int closest_index = -1; // sets to infinity
        for(int j = 0; j < n_objects; j++) {
            if(medoids[j] == 1) {
                if(dm[i * n_objects + j] < closest_dist) {
                    closest_dist = dm[i * n_objects + j];
                    closest_index = j;
                }
            }
            partition[i] = closest_index;
        }
    }
    free(dm);
    return partition;
}

void print_int_array(int *array, int size) {
    for(int n = 0; n < size; n++) {
        printf("%d\t", array[n]);
    }
    printf("\n");
}


/**
 * Calculates the Simplified Silhouette Width Criterion of a partition:
 *
 * 1/N * \sum_{i=1}^{N} \frac{b(i) - a(i)}{\max{b(i), a(i)}}
 *
 * For more information on this metric, see
 *
 * Vendramin, Lucas, Ricardo JGB Campello, and Eduardo R. Hruschka.
 * "Relative clustering validity criteria: A comparative overview."
 * Statistical Analysis and Data Mining 3.4 (2010): 209-235.
 */
//float sswc(int *medoids, float *dataset, int n_objects, int n_attributes) {
//    float *dm = get_distance_matrix(dataset, n_objects, n_attributes);
//    int *partition = get_partition(medoids, dm, dataset, n_objects, n_attributes);
//
//    float index = 0;
//    for(int i = 0; i < n_objects; i++) {
//        float a = dm[i * n_objects + partition[i]];
//        float b = INFINITY;
//
//        for(int j = 0; j < n_objects; j++) {
//            // is a medoid, is not the prototype for the cluster this object belongs to, and is closer to this object
//            //
//            if((medoids[j] == 1) && (j != partition[i]) && (dm[i * n_objects + j] < b)) {
//                b = dm[i * n_objects + j] < b;
//            }
//        }
//        index += (b - a) / fmaxf(a, b);
//    }
//
//    return index / (float)n_objects;
//}

/**
 * Second implementation of the sswc.
 * @param medoids
 * @param dataset
 * @param n_objects
 * @param n_attributes
 * @return
 */
float sswc(int *medoids, float *dataset, int n_objects, int n_attributes) {
    int n_medoids = 0;
    for(int j = 0; j < n_objects; j++) {
        n_medoids += medoids[j];
    }
    if(n_medoids <= 1) {
        return -1;  // the index for the trivial partition
    }

    float dist, index = 0, a, b;

    for(int i = 0; i < n_objects; i++) {
        a = INFINITY, b = INFINITY;
        for(int j = 0; j < n_objects; j++) {
            if(medoids[j] == 0) {
                continue;
            } else {
                dist = get_euclidean_distance(
                        &dataset[i * n_attributes],
                        &dataset[j * n_attributes],
                        n_attributes - 1
                );
                if(dist < a) {
                    b = a;
                    a = dist;
                }
                else if (dist < b) {
                    b = dist;
                }
            }
        }
        printf("a: %f b: %f\n", a, b);
        index += (b - a) / ((b - a > 0)*fmaxf(b, a) + (b - a <= 0)*1);
    }
    return index / n_objects;
}

float *read_dataset(char *path, int *n_lines, int *n_columns) {
    FILE *file = fopen(path, "r");  // TODO change in the future!

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

void print_matrix(float *matrix, int n_lines, int n_columns) {
    int n = 0;
    while(n < n_lines * n_columns) {
        printf("%.8f\t\t", matrix[n]);
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
    int n_objects, n_attributes;

    float *dataset = read_dataset("../datasets/iris.csv", &n_objects, &n_attributes);

    int *medoids = randint(n_objects, 0, 1);
    medoids[49] = 1;
    medoids[99] = 1;
    medoids[149] = 1;

    float val = sswc(medoids, dataset, n_objects, n_attributes);
    printf("sswc: %f\n", val);

    free(dataset);
    return 0;
}