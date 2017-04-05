import numpy as np
from scipy.sparse import coo_matrix


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
