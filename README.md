# [RerF](neurodata.io/tools)

[![arXiv shield](https://img.shields.io/badge/arXiv-1506.03410-red.svg?style=flat)](https://arxiv.org/abs/1506.03410)

R-RerF (aka Randomer Forest (RerF) or Random Projection Forests) is an algorithm developed by Tomita et al. (2016) <arXiv:1506.03410v2> which is similar to Random Forest - Random Combination (Forest-RC) developed by Breiman (2001) <doi:10.1023/A:1010933404324>.  Random Forests create axis-parallel, or orthogonal trees. That is, the feature space is recursively split along directions parallel to the axes of the feature space. Thus, in cases in which the classes seem inseparable along any single dimension, Random Forests may be suboptimal.  To address this, Breiman also proposed and characterized Forest-RC, which uses linear combinations of coordinates rather than individual coordinates, to split along.  This package, 'rerf', implements RerF which is similar to Forest-RC.  The difference between the two algorithms is where the random linear combinations occur: Forest-RC combines features at the per tree level whereas RerF takes linear combinations of coordinates at every node in the tree.  

This algorithm has a main implementation in C++ with bindings to both
Python and R.

# README

## [C++-RerF](./packedForest/README.md)

## [Py-RerF](./Python/README.md)

## [R-RerF](./R-Project/README.md)
[![CRAN Status Badge](https://www.r-pkg.org/badges/version/rerf)](https://cran.r-project.org/package=rerf)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2558568.svg)](https://doi.org/10.5281/zenodo.2558568)
