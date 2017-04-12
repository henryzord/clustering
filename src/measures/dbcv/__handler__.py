import itertools
from collections import Counter

import numpy as np
import operator
from scipy.sparse.csgraph._min_spanning_tree import minimum_spanning_tree
from scipy.sparse import csr_matrix, lil_matrix, coo_matrix


class Handler(object):

    def __init__(self, dataset):
        self._dataset = dataset.astype(np.float32)
        self._n_objects, self._n_attributes = dataset.shape

        self._data_dm = np.empty((self._n_objects, self._n_objects), dtype=np.float32)
        self._data_labels = np.empty(self._n_objects, dtype=np.int32)
        self._data_coredist = np.empty(self._n_objects, dtype=np.float32)
        self._data_mreach = np.empty((self._n_objects, self._n_objects), dtype=np.float32)

    def get_distance_matrix(self):
        return self._data_dm

    def __get_coredist__(self):
        pass

    def get_dbcv(self, labels):
        """
        Gets the Density Based Clustering Validation (DBCV) for a given cluster assignment.

        :type labels: numpy.ndarray
        :param labels: The cluster assignment for each object.
        :rtype: float
        :return: The DBCV.
        """
        labels = labels.astype(np.int32)
        counter = Counter(labels)
        clusters = counter.keys()

        for k in clusters:
            if counter[k] < 2:  # must have at least 1 internal node
                del counter[k]

        if len(counter) == 1:
            return -1.  # index of trivial cluster is -1

        self.__copy_labels__(labels)
        self.__get_coredist__()
        self.__mrd__()

        degrees = dict()
        dsc = dict()

        for k in clusters:
            mst, degrees[k] = self.__prim_mst__(
                np.array(np.flatnonzero(labels == k)),
                self._data_mreach,
                shape=(self._n_objects, self._n_objects)
            )

            if max(degrees[k].itervalues()) > 1:
                dsc[k] = self.__dsc__(mst, degrees[k])
            else:
                del counter[k]
                del degrees[k]

        dspc = self.__dspc__(degrees, self._data_mreach)

        print 'dsc:', dsc
        print 'dspc:', dspc

        index = np.sum(
            map(lambda k:
                (counter[k] / float(self._n_objects)) * ((dspc[k] - dsc[k]) / max(dspc[k], dsc[k])),
                counter.iterkeys()
            )
        )

        return index

    @staticmethod
    def __get_degree__(mst_mrd):
        """
        Given a Minimum Spanning Tree (MST) based on the Mutual Reachability Matrix (MRD) of a given cluster,
         finds the degree of each node.

        :type mst_mrd: scipy.sparse.coo_matrix
        :param mst_mrd: The MSTMRD.
        :rtype: collections.Counter
        :return: A collections.Counter object where each key is the index of the object in the cluster, and the value
            its degree.
        """
        col = mst_mrd.col
        row = mst_mrd.row

        if len(row) == 0 or len(col) == 0:
            return {None: 0}

        degree = Counter(itertools.chain(row, col))
        return degree

    def __mrd__(self):
        """
        Calculates the minimum reachability matrix (MRD) of the whole dataset.

        :rtype: numpy.ndarray
        :return: The mutual reachability matrix of the whole dataset.
        """
        pass

    @staticmethod
    def __dsc__(mst_mrd, degree):
        """
        Given a Minimum Spanning Tree (MST) based on the Mutual Reachability Matrix (MRD) of a given cluster,
        finds the density sparseness of the cluster (i.e, the maximum edge weight from a node with degree higher than one).

        :type mst_mrd: scipy.sparse.coo_matrix
        :param mst_mrd: The MSTMRD of a cluster.
        :type degree: collections.Counter
        :param degree: The degree of each object in the MST.
        :rtype: float
        :return: The DSC index.
        """
        weighted_edges = list(itertools.izip(mst_mrd.data, itertools.izip(mst_mrd.row, mst_mrd.col)))

        dsc = 0.
        has_internals = False
        for weight, (row, col) in weighted_edges:
            if degree[row] != 1 and degree[col] != 1:
                has_internals = True
                dsc = max(dsc, weight)

        to_return = mst_mrd.data.max() if not has_internals else dsc
        return to_return

    def __dspc__(self, degrees, mrd_matrix):
        """
        Calculates the Density Separation of a Pair of Clusters (DSPC) for each and every cluster.

        :type degrees: dict
        :param degrees: A dictionary of lists of internal nodes for every cluster.
        :type mrd_matrix: numpy.ndarray
        :param mrd_matrix: The whole dataset mutual reachability matrix.
        :rtype: dict
        :return: The DSPC for each and every cluster.
        """

        if len(degrees.keys()) == 1:  # if there is only one cluster
            return {x: 0. for x in degrees.iterkeys()}

        internals = dict()
        for c in degrees.iterkeys():
            internals[c] = self.__get_internals__(degrees[c])

        combinations = itertools.combinations(degrees.iterkeys(), r=2)

        _dspc = {x: np.inf for x in degrees.iterkeys()}
        for ci, cj in combinations:
            _min = (mrd_matrix[:, internals[ci]][internals[cj]]).min()
            _dspc[ci] = min(_dspc[ci], _min)
            _dspc[cj] = min(_dspc[cj], _min)

        return _dspc

    @staticmethod
    def __get_internals__(degree):
        """
        Given the degree of each node from a cluster, return the internal nodes of the given cluster.

        :type degree: collections.Counter
        :param degree: A dictionary with the degree of each object.
        :rtype: numpy.ndarray
        :return: A list of the internal nodes of the cluster.
        """

        return np.array(degree.keys())[
            np.flatnonzero(np.array(map(lambda x: degree[x] != 1, degree.iterkeys())))
        ]

    def __copy_labels__(self, labels):
        """
        Copy labels to self._data_labels.

        :type labels: numpy.ndarray
        :param labels: Cluster assignment for each object.
        """
        self._data_labels = labels.astype(np.int32)

    @staticmethod
    def __prim_mst__(objects, weight_matrix, shape):
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
                    if d[w] > weight:
                        d[w] = weight
                        parent[w] = v
                    if dist > d[w]:
                        dist = d[w]
                        next_v = w

            degree[parent[next_v]] += 1
            degree[next_v] += 1

            vec[counter, :] = [parent[next_v], next_v, weight_matrix[parent[next_v], next_v]]
            counter += 1

            v = next_v

        coo = coo_matrix((vec[:, -1], (vec[:, 0], vec[:, 1])), shape=shape)

        return coo, degree
