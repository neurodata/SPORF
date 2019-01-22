#!/bin/bash

sudo likwid-perfctr -C 2 -O -o experiment1.UopsExec -g UOPS_EXEC -m ./bin/experiment1 5 3 1
