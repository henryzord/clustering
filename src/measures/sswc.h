#ifndef CLUSTERING_SSWC_H
#define CLUSTERING_SSWC_H

#include "commons.h"

/**
 * Calculates the Simplified Silhouette Width Criterion of a partition: <br><br>
 * <img src="file:/home/henry/Projects/clustering/docs/img/sswc.png" border="0" /> <br>
 *
 * For more information about this metric, see
 *
 * <q>Vendramin, Lucas, Ricardo JGB Campello, and Eduardo R. Hruschka.
 * "Relative clustering validity criteria: A comparative overview."
 * Statistical Analysis and Data Mining 3.4 (2010): 209-235.</q>
 * @param medoids A truth array where zeros denote default objects and ones the medoids.
 * @param dataset A pointer to the first position of the dataset.
 * @param n_objects Number of objects.
 * @param n_attributes Number of attributes.
 * @return The Simplified Silhouette Width Criterion.
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
        index += (b - a) / ((b - a > 0)*fmaxf(b, a) + (b - a <= 0)*1);
    }
    return index / n_objects;
}

#endif //CLUSTERING_SSWC_H
