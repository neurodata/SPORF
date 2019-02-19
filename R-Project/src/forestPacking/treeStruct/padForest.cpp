#include <stdio.h>
#include <Rcpp.h>
#include "padForest.h"

void padForest::printForest(){
   Rprintf("\nThere are %d trees in the forest.\n", numTreesInForest);  
   Rprintf("The mean number of nodes per tree is %f\n", (float)totalNumberOfNodes/(float)numTreesInForest);
   Rprintf("\nThere are %d leaves in the forest.\n", totalNumberOfNodes>>1); 
}

void padForest::makePredictions(){
   Rprintf("not implemented in pad class\n");
}

void padForest::toggleDebugMode(){
    debugModeOn = !debugModeOn;
}

void padForest::toggleShowAllResults(){
    showAllResults = !debugModeOn;
}

