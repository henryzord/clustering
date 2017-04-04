#ifndef CLUSTERING_DBCV_H
#define CLUSTERING_DBCV_H

float a_pts_coredist() {
}

float prim(float *dataset, int n_objects, int n_attributes) {
    int *neighbor = (int*)malloc(sizeof(int) * n_objects);
    int *degree = (int*)malloc(sizeof(int) * n_objects);
    float *distance = (float*)malloc(sizeof(float) * n_objects);

    int added = 0;
    while(added < n_objects) {




        // TODO put this only when adding an object!
        added += 1;
    }

}

#endif //CLUSTERING_DBCV_H
