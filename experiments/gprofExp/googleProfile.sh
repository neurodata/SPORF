#!/bin/bash

LD_PRELOAD=/usr/local/lib/libprofiler.so CPUPROFILE=temp.prof ./bin/fp 1 3 1
google-pprof --pdf bin/fp temp.prof > rerf_profiling.pdf
LD_PRELOAD=/usr/local/lib/libprofiler.so CPUPROFILE=temp.prof ./bin/fp 2 3 1
google-pprof --pdf bin/fp temp.prof > rf_profiling.pdf
LD_PRELOAD=/usr/local/lib/libprofiler.so CPUPROFILE=temp.prof ./bin/fp 5 3 1
google-pprof --pdf bin/fp temp.prof > new_profiling.pdf
rm temp.prof
