//
// Created by henry on 30/03/17.
//

#ifndef CLUSTERING_SSWC_H
#define CLUSTERING_SSWC_H

#include <math.h>
#include <stdlib.h>

/**
 * Calculates the (unsquared) euclidean distance between two data objects.
 * @param a First object
 * @param b Second object
 * @param n_attributes Number of attributes for both objects
 * @return The (unsquared) euclidean distance
 */
float get_pow_euclidean_distance(float *a, float *b, int n_attributes) {
    float distance = 0;
    for(int n = 0; n < n_attributes; n++) {
        distance += powf(a[n] - b[n], (float)2);
    }
    return distance;
}

float *get_distance_matrix(float *dataset, int n_objects, int n_attributes) {
    float *matrix = (float*)malloc(sizeof(float) * n_objects * n_objects);

    for(int i = 0; i < n_objects; i++) {
        for(int j = 0; j < i; j++) {
            float dist = get_pow_euclidean_distance(
                    &dataset[i * n_objects],
                    &dataset[j * n_objects],
                    n_attributes
            );
            matrix[i * n_objects + j] = dist;
            matrix[j * n_objects + i] = dist;
        }
    }

    return NULL;
}

int *get_partition(int *medoids, float *dataset, int n_objects, int n_attributes) {
    return 0;
    float *dm = get_distance_matrix(dataset, n_objects, n_attributes);

    free(dm);  // TODO remove
    return 0;
}


/**
 * Calculates the Simplified Silhouette Width Criterion of a partition.
 *
 * For more information on this metric, see
 *
 * Vendramin, Lucas, Ricardo JGB Campello, and Eduardo R. Hruschka.
 * "Relative clustering validity criteria: A comparative overview."
 * Statistical Analysis and Data Mining 3.4 (2010): 209-235.
 */
float sswc(int *medoids, float *dataset, int n_objects, int n_attributes) {
    int *partition = get_partition(medoids, dataset, n_objects, n_attributes);

    return 0;
}

#endif //CLUSTERING_SSWC_H
