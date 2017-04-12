# -*- coding: utf-8 -*-

__author__ = 'Henry Cagnini'

try:
    import pycuda
    from cuda import CudaHandler as AvailableHandler
except ImportError:
    from sequential import SequentialHandler as AvailableHandler


def main():
    import pandas as pd
    import numpy as np

    dataset_path = '../../../datasets/iris.csv'
    dataset = pd.read_csv(dataset_path, names=['sepal_width', 'sepal_height', 'petal_width', 'petal_height'], header=None)
    partition = np.zeros(dataset.shape[0], dtype=np.int)
    partition[:dataset.shape[0]/2] = 1

    handler = AvailableHandler(dataset)
    print 'dbcv:', handler.get_dbcv(partition)


if __name__ == '__main__':
    main()
