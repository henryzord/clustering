import numpy as np
from scipy.sparse import coo_matrix
import pandas as pd


def prim_mst(objects, weight_matrix, shape):
    """
    Calculates the Prim's Minimum Spanning Tree (MST).
    
    :param objects: 
    :param weight_matrix: 
    :param shape: 
    :return: 
    """
    vec = np.empty((objects.shape[0] - 1, 3), dtype=np.float32)

    added = {x: False for x in objects}
    degree = {x: 0 for x in objects}
    parent = {x: x for x in objects}
    d = {x: np.inf for x in objects}

    v = min(objects)
    d[v] = 0
    counter = 0

    n_vertices = objects.shape[0]
    while counter < n_vertices - 1:
        added[v] = True
        dist = np.inf
        for w in objects:
            if w != v and not added[w]:
                weight = weight_matrix[v, w]
                if d[w] > weight:  # current neighbor is closer than the previous one
                    d[w] = weight
                    parent[w] = v  # the neighbor points to the current object
                if dist > d[w]:  # current lighter edge is heavier than the one just found
                    dist = d[w]  # sets the lighter edge to be the new one
                    next_v = w  # next object to be evaluated is w

        degree[parent[next_v]] += 1
        degree[next_v] += 1

        vec[counter, :] = [parent[next_v], next_v, weight_matrix[parent[next_v], next_v]]
        counter += 1

        v = next_v

    coo = coo_matrix((vec[:, -1], (vec[:, 0], vec[:, 1])), shape=shape)

    return coo, degree


def coredist(dm, partition, n_attributes):
    """
    
    
    :type dm: numpy.ndarray
    :param dm: Powered euclidean distance matrix
    :type partition: numpy.ndarray
    :param partition: Cluster assignment for each and every object
    :type n_attributes: int
    :param n_attributes: Number of attributes in the dataset
    :return: The core distance for each and every object
    """

    n_objects, _ = dm.shape

    data_coredist = np.empty(n_objects, dtype=np.float32)

    for i in xrange(n_objects):
        index_neighbours = np.setdiff1d(np.flatnonzero(partition == partition[i]), [i])
        n_neighbours = index_neighbours.shape[0]
        dist_neighbors = dm[i, index_neighbours]

        _sum = 0.
        for j, dist_neighbor in enumerate(dist_neighbors):
            if dist_neighbor > 0:
                _sum += (1. / dist_neighbor) ** float(n_attributes)

        _sum /= n_neighbours
        _sum **= -(1./n_attributes)

        data_coredist[i] = _sum

    return data_coredist


def main():
    dataset_path = '../../datasets/iris.csv'
    dataset = pd.read_csv(dataset_path, names=['sepal_width', 'sepal_height', 'petal_width', 'petal_height'], header=None)
    partition = np.zeros(dataset.shape[0], dtype=np.int)
    partition[:dataset.shape[0]/2] = 1

    from scipy.spatial.distance import cdist
    dm = cdist(dataset, dataset, metric='euclidean')
    dm **= 2.

    _coredist = coredist(dm, partition, dataset.shape[1])
    print _coredist


if __name__ == '__main__':
    main()
