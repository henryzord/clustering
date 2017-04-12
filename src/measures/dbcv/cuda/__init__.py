import pycuda.autoinit
import pycuda.driver as cuda
from pycuda.compiler import SourceModule

import warnings

import numpy as np
import os

from dbcv.__handler__ import Handler


class CudaHandler(Handler):
    _MIN_N_THREADS = 32
    _MAX_N_THREADS = 1024
    _N_OUTPUT = 3

    def __init__(self, dataset):
        super(CudaHandler, self).__init__(dataset)

        sep = '\\' if os.name == 'nt' else '/'

        cur_path = os.path.relpath(__file__)
        split = cur_path.split(sep)[:-1]

        kernel = '\n'.join(open(sep.join(split + ['kernel.cu']), 'r').readlines())
        mod = SourceModule(kernel)

        self._func_coredist = mod.get_function("all_pts_coredist")
        self._func_mreach = mod.get_function("global_mreach_matrix")
        self._func_dm = mod.get_function("distance_matrix")

        self._threads_per_block = ((self._n_objects / CudaHandler._MIN_N_THREADS) + 1) * CudaHandler._MIN_N_THREADS
        if self._threads_per_block > CudaHandler._MAX_N_THREADS:
            warnings.warn('Warning: using more threads per GPU than allowed! Rolling back to ' + str(self._MAX_N_THREADS) + '.')
            self._threads_per_block = CudaHandler._MAX_N_THREADS

        n_blocks = (self._n_objects / self._threads_per_block) + 1
        self._grid_size = (
            int(np.sqrt(n_blocks)),
            int(np.sqrt(n_blocks))
        )

        self._mem_coredist = cuda.mem_alloc(self._data_coredist.nbytes)
        self._mem_mreach = cuda.mem_alloc(self._data_mreach.nbytes)
        self._mem_dm = cuda.mem_alloc(self._data_dm.nbytes)

        self._mem_labels = cuda.mem_alloc(self._n_objects * np.float32(1).itemsize)
        self._mem_dataset = cuda.mem_alloc(self._dataset.nbytes)
        cuda.memcpy_htod(self._mem_dataset, self._dataset)

        self.__get_distance_matrix__(copy=True)

    def __get_distance_matrix__(self, copy=False):
        """
        Compute the euclidean distance matrix WITHOUT the square root.

        :param copy: optional - whether to copy the matrix from GPU to host memory. If True, will also return the matrix.
        """

        self._func_dm(
            self._mem_dm,
            np.int32(self._n_objects),
            np.int32(self._n_attributes),
            self._mem_dataset,
            block=(self._threads_per_block, 1, 1),  # block size
            grid=self._grid_size
        )

        if copy:
            cuda.memcpy_dtoh(self._data_dm, self._mem_dm)  # get info from gpu memory
            return self._data_dm

    def __get_coredist__(self):
        """
        Compute all points coredist.
        """

        cuda.memcpy_htod(self._mem_labels, self._data_labels)  # send info to gpu memory

        self._func_coredist(
            self._mem_coredist,
            np.int32(self._n_objects),
            np.int32(self._n_attributes),
            self._mem_dataset,
            self._mem_labels,
            block=(self._threads_per_block, 1, 1),  # block size
            grid=self._grid_size
        )

    def __mrd__(self):
        self._func_mreach(
            self._mem_dm,
            self._mem_mreach,
            self._mem_coredist,
            np.int32(self._n_objects),
            block=(self._threads_per_block, 1, 1),  # block size
            grid=self._grid_size
        )

        cuda.memcpy_dtoh(self._data_mreach, self._mem_mreach)  # get info from gpu memory
        return self._data_mreach

    def __copy_labels__(self, labels):
        """
        Copy labels to self._data_labels and GPU memory.

        :type labels: numpy.ndarray
        :param labels: cluster assignment for each object.
        """
        super(CudaHandler, self).__copy_labels__(labels)
        cuda.memcpy_htod(self._mem_labels, self._data_labels)  # send info to gpu memory
