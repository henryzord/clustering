import operator

import itertools
import warnings

from sklearn.metrics import pairwise_distances
import numpy as np

from __handler__ import Handler


class SequentialHandler(Handler):

    def __init__(self, dataset):
        super(SequentialHandler, self).__init__(dataset)
        self._data_dm = pairwise_distances(dataset, metric='euclidean') ** 2.
        self._data_coredist = np.empty(self._n_objects, dtype=np.float32)

    def __get_coredist__(self):
        """
        Calculate the all points core distance (a_pts_coredist) for each object.
        :rtype: numpy.ndarray
        :return: The core distance for each object in the dataset.
        """
        # -- non-vectorized version, for understanding -- #
        # for i in xrange(self._n_objects):
        #     index_neighbours = np.setdiff1d(np.flatnonzero(self.labels == self.labels[i]), [i])
        #     n_neighbours = index_neighbours.shape[0]
        #     dist_neighbours = self._data_dm[i, index_neighbours]
        #
        #     _sum = 0.
        #     for neighbour in dist_neighbours:
        #         _sum += (1. / neighbour) ** float(self._n_attributes)
        #
        #     _sum /= n_neighbours
        #     _sum **= -(1./self._n_attributes)
        #
        #     self._data_coredist[i] = _sum
        # --  vectorized counterpart, for speed -- #
        warnings.filterwarnings(action='ignore')
        vec_func = np.vectorize(
            lambda i: (
                (
                    1. / self._data_dm[i, np.setdiff1d(np.flatnonzero(self._data_labels == self._data_labels[i]), [i])]
                ) ** float(self._n_attributes)
            ).sum() / float(np.count_nonzero(self._data_labels == self._data_labels[i]) - 1)
        )

        self._data_coredist = np.power(vec_func(xrange(self._n_objects)), -(1./self._n_attributes))
        warnings.filterwarnings(action='default')

    def __mrd__(self):
        _list = itertools.combinations_with_replacement(xrange(self._n_objects), r=2)

        for x, y in _list:
            self._data_mreach[x, y] = max([self._data_dm[x, y], self._data_coredist[x], self._data_coredist[y]])
            self._data_mreach[y, x] = self._data_mreach[x, y]

        # warnings.warn('filling diagonal with zeros!')
        # np.fill_diagonal(self._data_mreach, 0.)  # TODO testing!

        return self._data_mreach


