 /**
  * Given two objects in a dataset, finds the euclidean distance between them.
  * params:
  * i_a: index of first object;
  * i_b: index of second object;
  * n_attributes: dimensionality of dataset;
  * dataset: dataset store in an unidimensional array of shape n_objects * n_attributes;
  */
 __device__ float euclidean_distance(int i_a, int i_b, int n_attributes, float *dataset) {
    int j;
    float distance = 0;
    for(j = 0; j < n_attributes; j++) {
        distance += powf(
            dataset[(i_a * n_attributes) + j] - dataset[(i_b * n_attributes) + j],
            2
        );
    }
    return distance;
}

 __global__ void distance_matrix(float *distance_matrix, int n_objects, int n_attributes, float *dataset) {
	const int idx = threadIdx.x + (blockDim.x * blockIdx.x);
	if(idx < n_objects) {
		int i;
		for(i = 0; i < n_objects; i++) {
			distance_matrix[(idx * n_objects) + i] = euclidean_distance(i, idx, n_attributes, dataset);
		}
	}
 }

 __global__ void all_pts_coredist(float *coredist, int n_objects, int n_attributes, float *dataset, int *labels) {

     const int idx = threadIdx.x + blockDim.x * blockIdx.x;

     if(idx < n_objects) {
        int i;

        float numeral = 0;
        int objects_in_cluster = 0;

        for(i = 0; i < n_objects; i++) {
            int within_group = (labels[idx] == labels[i]);
            objects_in_cluster += within_group;

            float distance = euclidean_distance(idx, i, n_attributes, dataset);

            float temp = powf(
                ((float)1 / distance),
                n_attributes
            );
            numeral += (float)within_group * (temp * (float)(idx != i));
        }

        float core_dist = powf(
            (objects_in_cluster - 1) / numeral,
            ((float)1/n_attributes)
        );

        coredist[idx] = core_dist;
     }
}

__global__ void global_mreach_matrix(float *distance_matrix, float *mreach, float *coredist, int n_objects) {
	const int idx = threadIdx.x + blockDim.x * blockIdx.x;
	if(idx < n_objects) {
		int i;
		float mreach_dist, distance;
		for(i = 0; i < n_objects; i++) {
			distance = distance_matrix[(idx * n_objects) + i];  // euclidean distance from objects i and idx
			mreach_dist = fmaxf(
				fmaxf(
					coredist[idx],
					coredist[i]
				),
				distance
			);
			mreach[(idx * n_objects) + i] = mreach_dist;
		}
	}
}

 __global__ void mreach_matrix(float *distance_matrix, float *mreach, float *coredist, int cluster, int *labels, int n_objects, int n_attributes, float *dataset) {
	const int idx = threadIdx.x + blockDim.x * blockIdx.x;
	if(idx < n_objects) {
		int i;
		float mreach_dist, distance;
		for(i = 0; i < n_objects; i++) {
			distance = distance_matrix[(idx * n_objects) + i];
			mreach_dist = fmaxf(
				fmaxf(
					coredist[idx],
					coredist[i]
				),
				distance
			);
			mreach[(idx * n_objects) + i] = ((labels[idx] == cluster) && (labels[i] == cluster)) * mreach_dist;
				// + (((labels[idx] != cluster) || (labels[i] != cluster)) * 0);  // 0 denotes no link between objects
		}
	}
 }