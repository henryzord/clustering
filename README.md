# Clustering with EDAs

This repository is a container for Estimation of Distribution Algorithms (EDAs) that perform the data mining task of clustering.

All algorithms are published, as denoted in Section [Citation]().

AffEDA is an EDA for enhancing the quality of Affinity Propagation, which is a clustering algorithm _per se_.

Clus-EDA performs clustering through the use of medoids, cluster representatives that are coincident with data objects (as opposed to centroids, which are hypothetical data objects). 

PASCAL performs a density-based clustering using a Minimum Spanning Tree (MST). It produces a MST from the set of data objects and then proceeds to remove edges from this structure. Two objects connected by an edge are considered to belong to the same cluster.

## Citation
If you use any of the algorithms here implemented in your work, please cite the corresponding authors:

#### AffEDA

> Santana, Roberto, Pedro Larrañaga, and José A. Lozano. "Learning factorizations in estimation of distribution algorithms using affinity propagation." Evolutionary Computation 18.4 (2010): 515-546.

```latex
@article{santana2010learning,
  title={Learning factorizations in estimation of distribution algorithms using affinity propagation},
  author={Santana, Roberto and Larra{\~n}aga, Pedro and Lozano, Jos{\'e} A},
  journal={Evolutionary Computation},
  volume={18},
  number={4},
  pages={515--546},
  year={2010},
  publisher={MIT Press}
}
```

#### Clus-EDA

> Cagnini, Henry EL, et al. "Medoid-based data clustering with estimation of distribution algorithms." Proceedings of the 31st Annual ACM Symposium on Applied Computing. ACM, 2016.

```latex
@inproceedings{cagnini2016medoid,
  title={Medoid-based data clustering with estimation of distribution algorithms},
  author={Cagnini, Henry EL and Barros, Rodrigo C and Quevedo, Christian V and Basgalupp, M{\'a}rcio P},
  booktitle={Proceedings of the 31st Annual ACM Symposium on Applied Computing},
  pages={112--115},
  year={2016},
  organization={ACM}
}
```

#### PASCAL

> Henry E. L. Cagnini, Rodrigo C. Barros. "PASCAL: An EDA for parameterless shape-independent clustering". Proceedings of the 2016 IEEE Congress on Evolutionary Computation. IEEE, 2016.

```latex
@inproceedings{DBLP:conf/cec/CagniniB16,
  author    = {Henry E. L. Cagnini and
               Rodrigo C. Barros},
  title     = {{PASCAL:} An {EDA} for parameterless shape-independent clustering},
  booktitle = {{IEEE} Congress on Evolutionary Computation, {CEC} 2016, Vancouver,
               BC, Canada, July 24-29, 2016},
  pages     = {3433--3440},
  year      = {2016},
  url       = {http://dx.doi.org/10.1109/CEC.2016.7744224},
  doi       = {10.1109/CEC.2016.7744224},
  timestamp = {Thu, 24 Nov 2016 20:39:06 +0100},
  biburl    = {http://dblp.uni-trier.de/rec/bib/conf/cec/CagniniB16},
  bibsource = {dblp computer science bibliography, http://dblp.org}
}
```
