#!/bin/bash

sudo likwid-perfctr -C 2 -O -o experiment1.UopsExec -g UOPS_EXEC -m ./bin/experiment1
mv experiment6.L2Cache experiments/experiment6/

sudo likwid-perfctr -C 2 -O -o experiment6.Energy -g ENERGY -m ./bin/experiment6
mv experiment6.CycleActivity experiments/experiment6/

sudo likwid-perfctr -C 2 -O -o experiment6.UOPS -g UOPS -m ./bin/experiment6
mv experiment6.Branch experiments/experiment6/
