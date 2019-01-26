#!/bin/bash

sudo likwid-perfctr -C 2 -O -o load.UOPS_EXEC1 -g UOPS_EXEC -m ./bin/testFast 1 100000
sudo likwid-perfctr -C 2 -O -o load.UOPS_EXEC7 -g ICACHE -m ./bin/testFast 7 100000
#sudo likwid-perfctr -C 2 -O -o load.UOPS_EXEC3 -g UOPS_EXEC -m ./bin/testFast 3 100000
#sudo likwid-perfctr -C 2 -O -o load.UOPS_EXEC4 -g UOPS_EXEC -m ./bin/testFast 4 100000
#sudo likwid-perfctr -C 2 -O -o load.UOPS_EXEC5 -g UOPS_EXEC -m ./bin/testFast 5 100000
#sudo likwid-perfctr -C 2 -O -o load.UOPS_EXEC5 -g UOPS_EXEC -m ./bin/testFast 6 100000
