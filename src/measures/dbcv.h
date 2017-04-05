#ifndef CLUSTERING_DBCV_H
#define CLUSTERING_DBCV_H

#define NEIGHBOR_INDEX 0
#define NEIGHBOR_DISTANCE 1
#define DEGREE 2
#define MST_FIELDS 3

/**
 * Calculates the coredistance between all points within a dataset, which is:
 *
 * <img src="file:/home/henry/Projects/clustering/docs/img/a_pts_coredist.png" border="0" /> <br>
 *
 * Where:
 * <ul>
 * <li>X_i is the i-th object in the dataset</li>
 * <li>X_pi is the set of objects in the same cluster as of X_i</li>
 * <li>M is the dimensionality of the data (i.e number of attributes)</li>
 * <li> d(X_i, X_j) is the euclidean unpowered distance between object X_i and the j-th closest same-cluster neighbor</li>
 * </ul>
 *
 * @param partition An array with the cluster assignment for each object
 * @param dm A pointer to the first position of a distance matrix
 * @param n_objects Number of objects in the dataset
 * @param n_attributes Number of attributes in the dataset
 * @return An array with the a_pts_coredist for each and every object in the dataset
 */
float *a_pts_coredist(int *partition, float *dm, int n_objects, int n_attributes) {
    float *apts = (float*)malloc(sizeof(float) * n_objects);

    for(int i = 0; i < n_objects; i++) {
        int cluster_size = 1;
        float sum_term = 0;
        float dist;
        for(int j = 0; j < n_objects; j++) {
            if((partition[i] == partition[j]) && (i != j)) {
                dist = dm[i * n_objects + j];
                sum_term += powf(1 / dist, (float)n_attributes);
                cluster_size += 1;
            }
        }

        apts[i] = 1 / powf(sum_term / (cluster_size - 1), (float)1/n_attributes);
    }

    print_array_float(apts, n_objects);

    return apts;
}


/**
 * Finds the Minimum Spanning Tree of a dataset using the Prim algorithm.
 *
 * @param dataset A pointer to the first position of a dataset.
 * @param n_objects Number of objects in the dataset.
 * @param n_attributes Number of attributes in the dataset.
 * @return A pointer to the first position of the minimum spanning tree, which is
 *  a matrix with n_objects * 3 positions:
 *  <ul>
 *  <li>The first column informs the index of the closest neighbor for the current-index object;</li>
 *  <li>The second column is the distance to aforementined neighbor;</li>
 *  <li>The third and last column is the degree of the current-index object (i.e degree one means that the object is
 *    linked to only another object).</li>
 *  </ul>
 */
float *prim_dat(float *dataset, int n_objects, int n_attributes) {
    // closest neighbor index, closest neighbor distance, and degree of the object
    float *mst = (float*)malloc(sizeof(float) * (n_objects * MST_FIELDS));
    for(int n = 0; n < n_objects; n++) {
        mst[n * MST_FIELDS + NEIGHBOR_INDEX] = INFINITY;
        mst[n * MST_FIELDS + NEIGHBOR_DISTANCE] = INFINITY;
        mst[n * MST_FIELDS + DEGREE] = 0;
    }

    int v = rand() % n_objects, next_v = INFINITY;

    int counter = 0;
    while(counter < n_objects - 1) {
        float dist = INFINITY;

        for(int w = 0; w < n_objects; w++) {
            if((w != v) && (mst[w * MST_FIELDS + DEGREE] == 0)) {
                float weight = get_euclidean_distance(
                        &dataset[v * n_attributes],
                        &dataset[w * n_attributes],
                        n_attributes,
                        true
                );
                if(mst[w * MST_FIELDS + NEIGHBOR_DISTANCE] > weight) {
                    mst[w * MST_FIELDS + NEIGHBOR_DISTANCE] = weight;
                    mst[w * MST_FIELDS + NEIGHBOR_INDEX] = v;
                }
                if(dist > mst[w * MST_FIELDS + NEIGHBOR_DISTANCE]) {
                    dist = mst[w * MST_FIELDS + NEIGHBOR_DISTANCE];
                    next_v = w;
                }
            }
        }
        mst[(int)mst[next_v * MST_FIELDS + NEIGHBOR_INDEX] * MST_FIELDS + DEGREE] += 1;
        mst[next_v * MST_FIELDS + DEGREE] += 1;
        counter += 1;

        v = next_v;
    }
    return mst;
}

/**
 * Finds the Minimum Spanning Tree of a dataset using the Prim algorithm.
 *
 * @param dm A pointer to the first position of a distance matrix.
 * @param n_objects Number of objects in the dataset.
 * @return A pointer to the first position of the minimum spanning tree, which is
 *  a matrix with n_objects * 3 positions:
 *  <ul>
 *  <li>The first column informs the index of the closest neighbor for the current-index object;</li>
 *  <li>The second column is the distance to aforementined neighbor;</li>
 *  <li>The third and last column is the degree of the current-index object (i.e degree one means that the object is
 *    linked to only another object).</li>
 *  </ul>
 */
float *prim_mat(float *dm, int n_objects) {
    // closest neighbor index, closest neighbor distance, and degree of the object
    float *mst = (float*)malloc(sizeof(float) * (n_objects * MST_FIELDS));
    for(int n = 0; n < n_objects; n++) {
        mst[n * MST_FIELDS + NEIGHBOR_INDEX] = INFINITY;
        mst[n * MST_FIELDS + NEIGHBOR_DISTANCE] = INFINITY;
        mst[n * MST_FIELDS + DEGREE] = 0;
    }

    int v = rand() % n_objects, next_v = INFINITY;

    int counter = 0;
    while(counter < n_objects - 1) {
        float dist = INFINITY;

        for(int w = 0; w < n_objects; w++) {
            if((w != v) && (mst[w * MST_FIELDS + DEGREE] == 0)) {
                float weight = dm[v * n_objects + w];
                if(mst[w * MST_FIELDS + NEIGHBOR_DISTANCE] > weight) {
                    mst[w * MST_FIELDS + NEIGHBOR_DISTANCE] = weight;
                    mst[w * MST_FIELDS + NEIGHBOR_INDEX] = v;
                }
                if(dist > mst[w * MST_FIELDS + NEIGHBOR_DISTANCE]) {
                    dist = mst[w * MST_FIELDS + NEIGHBOR_DISTANCE];
                    next_v = w;
                }
            }
        }
        mst[(int)mst[next_v * MST_FIELDS + NEIGHBOR_INDEX] * MST_FIELDS + DEGREE] += 1;
        mst[next_v * MST_FIELDS + DEGREE] += 1;
        counter += 1;

        v = next_v;
    }
    return mst;
}

#endif //CLUSTERING_DBCV_H
