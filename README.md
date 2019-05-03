# [RerF](https://neurodata.io/tools)

[![arXiv shield](https://img.shields.io/badge/arXiv-1506.03410-red.svg?style=flat)](https://arxiv.org/abs/1506.03410)
[![CRAN Status Badge](https://www.r-pkg.org/badges/version/rerf)](https://cran.r-project.org/package=rerf)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2558568.svg)](https://doi.org/10.5281/zenodo.2558568)
[![dockerhub](https://img.shields.io/badge/Hub.Docker-NeuroData%3ARerF-%232181E7.svg)](https://cloud.docker.com/u/neurodata/repository/docker/neurodata/rerf)

RerF (aka Randomer Forest or Random Projection Forests) is an algorithm developed by [Tomita et al. (2016)](https://arxiv.org/abs/1506.03410) which is similar to Random Forest-Random Combination (Forest-RC) developed by [Breiman (2001)](https://doi.org/10.1023/A:1010933404324).  

The difference between the two algorithms is where the random linear combinations occur: Forest-RC combines features at the tree level whereas RerF combines features at the node level.


# Packages 

## [packedForest (C++)](packedForest/README.md)
- Memory optimized C++ implementation of RandomForest and RerF.

### [Py-RerF](Python/README.md)
- Python bindings to packedForest.

## [R-RerF](R-Project/README.md)
- The R and C++ implemetation of RerF.
