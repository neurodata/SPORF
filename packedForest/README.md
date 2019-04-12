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

#### Add the following lines to your `~/.bash_profile`

Get the path to your brew install of `llvm` with the following command:
`brew --prefix llvm` and add it to teh lines below.

> \#\#brew install llvm for openmp support\
> export LDFLAGS="-L\<path to llvm\>/lib"\
> export CPPFLAGS="-I\<path to llvm\>/include"

#### Compile and run

The first number specifies the algorithm, the second the dataset, and
the third is number of cores.
- `make`
- `./bin/fp 8 1 1` 



