//
// Created by henry on 04/04/17.
//

#ifndef CLUSTERING_COMMONS_H
#define CLUSTERING_COMMONS_H

#include <math.h>

/**
 * Calculates the euclidean distance between two data objects.
 * @param x1 First object
 * @param x2 Second object
 * @param n_attributes Number of predictive attributes for both objects
 * @param squared Whether to return a squared euclidean distance
 * @return The euclidean distance
 */
float get_euclidean_distance(float *x1, float *x2, int n_attributes, bool squared) {
    float distance = 0;
    for(int n = 0; n < n_attributes; n++) {
        distance += pow(x1[n] - x2[n], (float)2);
    }
    return ((squared)*distance) + (!squared)*sqrtf(distance);
}

/**
 * Gets the distance matrix between objects within a dataset.

 * @param dataset A pointer to the first element in the dataset
 * @param n_objects Number of objects within the dataset
 * @param n_attributes Total number of attributes in the dataset
 * @param squared Whether to return a distance matrix of squared euclidean distances or not
 * @return A pointer to the first position of the distance matrix, which has size n_objects x n_objects.
 */
float *get_distance_matrix(float *dataset, int n_objects, int n_attributes, bool squared) {
    float *matrix = (float*)malloc(sizeof(float) * n_objects * n_objects);

    for(int i = 0; i < n_objects; i++) {
        for(int j = 0; j <= i; j++) {
            float dist = get_euclidean_distance(
                    &dataset[i * n_attributes],
                    &dataset[j * n_attributes],
                    n_attributes,
                    squared
            );
            matrix[i * n_objects + j] = dist;
            matrix[j * n_objects + i] = dist;
        }
    }
    return matrix;
}

/**
 * From a set of medoids, gets the partition.
 *
 * @param medoids A truth array where zeros are default objects and ones the medoids
 * @param dm A pointer to the first position in the distance matrix
 * @param dataset A pointer to the first position of the dataset
 * @param n_objects Number of objects in the dataset
 * @param n_attributes Total number of attributes in the dataset
 * @return A pointer to the first position of the partition array, which has size n_objects
 */
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

#endif //CLUSTERING_COMMONS_H
