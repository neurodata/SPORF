# [SPORF/RerF](https://neurodata.io/sporf)

[![arXiv shield](https://img.shields.io/badge/arXiv-1506.03410-red.svg?style=flat)](https://arxiv.org/abs/1506.03410)
[![PyPI version](https://badge.fury.io/py/rerf.svg)](https://badge.fury.io/py/rerf)
[![CRAN Status Badge](https://www.r-pkg.org/badges/version/rerf)](https://cran.r-project.org/package=rerf)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2595524.svg)](https://doi.org/10.5281/zenodo.2595524)
[![dockerhub](https://img.shields.io/badge/Hub.Docker-NeuroData%3ARerF-%232181E7.svg)](https://hub.docker.com/r/neurodata/rerf)
[![Gigantum](https://img.shields.io/badge/Gigantum-View%20Project-593C5E?link=https://gigantum.com&link=https://gigantum.com/neurodata/sporf-demo)](https://gigantum.com/neurodata/sporf-demo)
![Downloads shield](https://img.shields.io/pypi/dm/rerf.svg)


SPORF -- sparse projection oblique randomer forests (aka RerF, Randomer Forest or Random Projection Forests) -- is an algorithm developed by [Tomita et al. (2016)](https://arxiv.org/abs/1506.03410) which is similar to Random Forest-Random Combination (Forest-RC) developed by [Breiman (2001)](https://doi.org/10.1023/A:1010933404324).

The difference between the two algorithms is where the random linear combinations occur: Forest-RC combines features at the tree level whereas RerF combines features at the node level.


# Packages 

## [packedForest (C++)](packedForest/README.md)
- Memory optimized C++ implementation of RandomForest and RerF.

### [Py-RerF](Python/README.md)
- Python bindings to packedForest.

## [R-RerF](R-Project/README.md)
- The R and C++ implemetation of RerF.
