//#include <iostream>
//#include <fstream>
//#include <queue>
#include <stdio.h>
//#include <stdlib.h>
//#include <string>
//#include <typeinfo>
//#include <vector>
#include "padForest.h"

void padForest::printForest(){
    printf("\nThere are %d trees in the forest.\n", numTreesInForest);  
    printf("The mean number of nodes per tree is %f\n", (float)totalNumberOfNodes/(float)numTreesInForest);
    printf("\nThere are %d leaves in the forest.\n", totalNumberOfNodes>>1); 
}

void padForest::makePredictions(){
    printf("not implemented in pad class\n");
}

void padForest::toggleDebugMode(){
    debugModeOn = !debugModeOn;
}

void padForest::toggleShowAllResults(){
    showAllResults = !debugModeOn;
}
/*
int padForest::returnClassPrediction(int *classTotals){
    int predictMaxValue = -1;
    int predictMaxClass = -1;
    for(int m=0; m<numOfClasses; m++){
        if(classTotals[m] > predictMaxValue){
            predictMaxClass = m;
            predictMaxValue = classTotals[m];
        }
    }
    return predictMaxClass;
}*/
