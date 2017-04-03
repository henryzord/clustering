import pandas as pd
import numpy as np
from scipy.spatial.distance import cdist

__author__ = 'Henry Cagnini'


def get_partition(medoids, dataset):
    medoid_index = np.flatnonzero(medoids)
    medoids_sample = dataset.loc[medoid_index]
    m_dist = cdist(dataset, medoids_sample, metric='euclidean')

    closest = map(lambda x: medoid_index[np.argmin(x)], m_dist)
    return closest


def sswc(medoids, dataset):
    n_objects, n_attributes = dataset.shape

    # TODO fix this later!
    del dataset['class']

    medoid_index = np.flatnonzero(medoids)  # index of objects that are medoids
    medoids_sample = dataset.loc[medoid_index]  # values for attributes for medoid objects
    m_dist = cdist(dataset, medoids_sample, metric='euclidean')  # distance between objects and medoid objects

    a_raw = np.array(map(np.argmin, m_dist))
    a = np.array(map(lambda i: m_dist[i, np.argmin(m_dist[i])], xrange(n_objects)), dtype=np.float32)  # distance to closest object

    for i in xrange(n_objects):
        m_dist[i, a_raw[i]] = np.inf

    b = np.array(map(lambda i: m_dist[i, np.argmin(m_dist[i])], xrange(n_objects)), np.float32)  # distance to second closest object

    # TODO remove me!
    import itertools as it
    for x, y in it.izip(a, b):
        print 'a: %f b: %f' % (x, y)

    index = np.sum((b - a) / np.maximum(b, a)) / float(n_objects)

    return np.float32(index)


def main():
    dataset_path = '../../datasets/iris.csv'
    dataset = pd.read_csv(dataset_path, names=['sepal_width', 'sepal_height', 'petal_width', 'petal_height', 'class'], header=None)
    medoids = np.zeros(dataset.shape[0], dtype=np.int)
    medoids[[49, 99, 149]] = 1

    # print 'REMOVE ME LATER!'
    # print cdist(dataset, dataset, metric='euclidean')[:5, :5]

    index = sswc(medoids, dataset)
    print 'sswc: %f' % index

main()
