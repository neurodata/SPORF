#!/bin/bash

g++ -std=c++14 -O3 -funroll-loops -pipe -march=native -msse2 -mavx2 -Ivec -I. -fpic -Wall -Wextra -Wdisabled-optimization -Wno-unused-parameter -pedantic -fno-strict-aliasing -Wno-attributes -Wno-pragmas -o fastTest test.cpp
