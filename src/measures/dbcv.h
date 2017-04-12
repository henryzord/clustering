#ifndef CLUSTERING_DBCV_H
#define CLUSTERING_DBCV_H

#define MST_FIELDS 3

#define NEIGHBOR_INDEX 0
#define NEIGHBOR_DISTANCE 1
#define SELF_DEGREE 2

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
        int cluster_size = 0;
        float _sum = 0;
        for(int j = 0; j < n_objects; j++) {
            if(partition[i] == partition[j]) {
                float dist = dm[i * n_objects + j];
                if(dist > 0) {
                    _sum += powf(1 / dist, (float)n_attributes);
                }
                cluster_size += 1;
            }
        }
        _sum /= (float)(cluster_size - 1);

        apts[i] = powf(_sum, -1/(float)n_attributes);
    }

    return apts;
}

/**
 * Calculates the mutual reachability distance between two objects.
 *
 * @param apts_1 The core distance for the first object.
 * @param apts_2 The core distance for the second object.
 * @param sqd_dist Squared euclidean distance between those two objects.
 * @return The mutual reachability distance (mreach).
 */
float mreach_dist(float apts_1, float apts_2, float sqd_dist) {
    return
        fmaxf(
            fmaxf(apts_1, apts_2),
            sqd_dist
        );
}

/**
 * Calculates a matrix of mutual reachability distance between data objects.
 *
 * @param apts The core distance for each and every object in the dataset
 * @param sqd_dm The matrix of squared euclidean distance between data objects
 * @param n_objects Number of objects in the dataset
 * @return
 */
float *mreach_mat(float *apts, float *sqd_dm, int n_objects) {
    float *matrix = (float*)malloc(sizeof(float) * n_objects * n_objects);

    for(int i = 0; i < n_objects; i++) {
        for(int j = 0; j <= i; j++) {
            matrix[i * n_objects + j] = mreach_dist(apts[i], apts[j], sqd_dm[i * n_objects + j]);
            matrix[j * n_objects + i] = matrix[i * n_objects + j];
        }
    }

    return matrix;
}


/**
 * Finds the Minimum Spanning Tree of a dataset using the Prim algorithm.
 *
 * @param dataset A pointer to the first position of a dataset
 * @param n_objects Number of objects in the dataset
 * @param n_attributes Number of attributes in the dataset
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
        mst[n * MST_FIELDS + SELF_DEGREE] = 0;
    }

    int v = rand() % n_objects, next_v = INFINITY;

    int counter = 0;
    while(counter < n_objects - 1) {
        float dist = INFINITY;

        for(int w = 0; w < n_objects; w++) {
            if((w != v) && (mst[w * MST_FIELDS + SELF_DEGREE] == 0)) {
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
        mst[(int)mst[next_v * MST_FIELDS + NEIGHBOR_INDEX] * MST_FIELDS + SELF_DEGREE] += 1;
        mst[next_v * MST_FIELDS + SELF_DEGREE] += 1;
        counter += 1;

        v = next_v;
    }
    return mst;
}

/**
 * Finds the Minimum Spanning Tree of a dataset using the Prim algorithm.
 *
 * @param dm A pointer to the first position of a distance matrix
 * @param n_objects Number of objects in the dataset
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
        mst[n * MST_FIELDS + SELF_DEGREE] = 0;
    }

    int v = rand() % n_objects, next_v = INFINITY;

    int counter = 0;
    while(counter < n_objects - 1) {
        float dist = INFINITY;

        for(int w = 0; w < n_objects; w++) {
            if((w != v) && (mst[w * MST_FIELDS + SELF_DEGREE] == 0)) {
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
        mst[(int)mst[next_v * MST_FIELDS + NEIGHBOR_INDEX] * MST_FIELDS + SELF_DEGREE] += 1;
        mst[next_v * MST_FIELDS + SELF_DEGREE] += 1;
        counter += 1;

        v = next_v;
    }
    return mst;
}

int *get_labels(int *partition, int n_objects, int *n_groups, int *group_size) {
    int *unique = NULL;

    *n_groups = 0;

    for(int n = 0; n < n_objects; n++) {
        group_size[n] = 0;
    }

    for(int n = 0; n < n_objects; n++) {
        if(group_size[partition[n]] == 0) {
            *n_groups += 1;
        }
        group_size[partition[n]] += 1;
    }

    unique = (int*)malloc(sizeof(int) * *n_groups);
    int counter = 0;
    for(int n = 0; n < n_objects; n++) {
        if(group_size[n] > 0) {
            unique[counter] = n;
            counter += 1;
        }
    }
    return unique;
}

int get_group_size(int *partition, int n_objects, int cluster) {
    int group_size = 0;
    for(int n = 0; n < n_objects; n++) {
        if(partition[n] == cluster) {
            group_size += 1;
        }
    }
    return group_size;
}

float *prim_cls(float *dm, int *partition, int n_objects, int *labels, int n_groups) {
    // closest neighbor index, closest neighbor distance, and degree of the object
    float *mst = (float*)malloc(sizeof(float) * (n_objects * MST_FIELDS));
    for(int n = 0; n < n_objects; n++) {
        mst[n * MST_FIELDS + NEIGHBOR_INDEX] = INFINITY;
        mst[n * MST_FIELDS + NEIGHBOR_DISTANCE] = INFINITY;
        mst[n * MST_FIELDS + SELF_DEGREE] = 0;
    }

    for(int c = 0; c < n_groups; c++) {
        int
            v = INFINITY, // current vertex
            next_v = INFINITY, // next vertex
            counter = 0,  // objects in this group that were add to the current mst
            group_size = get_group_size(partition, n_objects, labels[c]);  // size of this group

        // current vertex is the first object for considered group
        for(int n = 0; n < n_objects; n++) {
            if(partition[n] == c) {
                v = n;
                break;
            }
        }

        while(counter < group_size - 1) {
            float dist = INFINITY;

            for(int w = 0; w < n_objects; w++) {
                if((partition[w] == partition[v]) && (partition[w] == labels[c]) &&
                        (w != v) && (mst[w * MST_FIELDS + SELF_DEGREE] == 0)) {
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
            mst[(int)mst[next_v * MST_FIELDS + NEIGHBOR_INDEX] * MST_FIELDS + SELF_DEGREE] += 1;
            mst[next_v * MST_FIELDS + SELF_DEGREE] += 1;
            counter += 1;

            v = next_v;
        }

    }
    return mst;
}

float *validity_of_cluster(float *mreach_mst, float *mreach_matrix, int *partition, int n_objects, int *labels, int n_groups) {
    float *vc = (float*)malloc(sizeof(float) * n_groups);

    for(int c = 0; c < n_groups; c++) {
        float dsc = -INFINITY;
        float dspc = INFINITY;

        int min_degree = 2;
//        while(dsc == -INFINITY && dspc == INFINITY && min_degree > 0) {
        for(int i = 0; i < n_objects; i++) {  // outer object
            // object is not an inner node, or its group is not the currently analysed group
            if((mreach_mst[i * MST_FIELDS + SELF_DEGREE] < min_degree) || (partition[i] != labels[c])) {
                continue;
            }

            for(int j = 0; j < n_objects; j++) {  // inner object
                if(mreach_mst[j * MST_FIELDS + SELF_DEGREE] < min_degree) {
                    continue;
                }

                if(partition[i] != partition[j]) {
                    if(mreach_matrix[i * n_objects + j] < dspc) {  // minimum distance between two clusters
                        dspc = mreach_matrix[i * n_objects + j];
                    }
                } else if( // maximum distance between two same-cluster objects
                        (mreach_mst[i * MST_FIELDS + NEIGHBOR_INDEX] == j) &&
                        (mreach_mst[i * MST_FIELDS + NEIGHBOR_DISTANCE] > dsc)) {
                    dsc = mreach_mst[i * MST_FIELDS + NEIGHBOR_DISTANCE];
                }
            }
        }
//            min_degree -= 1;
//        }
        printf("group %d dsc %f dspc: %f\n", c, dsc, dspc);

        if(dspc == INFINITY) {
            dspc = 0;
        }
        if(dsc == -INFINITY) {
            vc[c] = 0;
        } else {
            vc[c] = (dspc - dsc) / fmaxf(dspc, dsc);
        }
    }
    return vc;
}


float dbcv(int *partition, float *dm, int n_objects, int n_attributes) {
    float *apts = a_pts_coredist(partition, dm, n_objects, n_attributes);
    float *mreach_matrix = mreach_mat(apts, dm, n_objects);

    int *group_size = (int*)malloc(sizeof(int) * n_objects);
    int n_groups, *labels = get_labels(partition, n_objects, &n_groups, group_size);

    float *mreach_mst = prim_cls(mreach_matrix, partition, n_objects, labels, n_groups);

    float *vc = validity_of_cluster(mreach_mst, mreach_matrix, partition, n_objects, labels, n_groups);

    float dbcv_index = 0;
    for(int c = 0; c < n_groups; c++) {
        dbcv_index += (group_size[labels[c]] / (float)n_objects) * vc[c];
    }

    free(group_size);
    free(mreach_matrix);
    free(mreach_mst);
    free(labels);
    free(apts);
    free(vc);

    return dbcv_index;
}

#endif //CLUSTERING_DBCV_H
