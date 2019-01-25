#!/bin/bash

sudo likwid-perfctr -C 2 -O -o load.UOPS_EXEC -g UOPS_EXEC -m ./bin/experiment1 5 4 1
sudo likwid-perfctr -C 2 -O -o load.CACHES -g CACHES -m ./bin/experiment1 5 4 1
sudo likwid-perfctr -C 2 -O -o load.BRANCH -g BRANCH -m ./bin/experiment1 5 4 1
sudo likwid-perfctr -C 2 -O -o load.CYCLE_STALLS -g CYCLE_STALLS -m ./bin/experiment1 5 4 1
