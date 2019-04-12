[![Build Status](https://travis-ci.org/neurodata/packedForest.svg?branch=master)](https://travis-ci.org/neurodata/packedForest)

# packedForest
A memory efficient and fast random forest which produces trees with fast prediction speeds.


## Steps to compile

### macOS

For macOS (10.14) these were a sequence of steps that resulted in a
succesful outcome (yours may be different.) 

#### Using hombrew

- `brew install gcc@8`
- `brew install libomp`
- `brew install llvm`

#### Compile and run

The first number specifies the algorithm, the second the dataset, and
the third is number of cores.
- `make`
- `./bin/fp 8 1 1` 



