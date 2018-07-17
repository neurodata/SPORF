#include "inferenceSamples.h"
#include <Rcpp.h>

void inferenceSamples::percentRight(){
    int numCorrectPredictions = 0;
    for(int i = 0; i < numObservations; i++){
if(0){
           Rprintf("observation%d actual %d predicted %d\n", i, observationClasses[i],predictedClasses[i]);
           }
           if(observationClasses[i] == predictedClasses[i]){
           numCorrectPredictions++;
           }
    }

     Rprintf("%f%% of the predictions were correct\n", 100.0*(float)numCorrectPredictions/(float)numObservations);

}

float inferenceSamples::returnPercentRight(){
    int numCorrectPredictions = 0;
    for(int i = 0; i < numObservations; i++){
if(0){
           Rprintf("observation%d actual %d predicted %d\n", i, observationClasses[i],predictedClasses[i]);
           }
           if(observationClasses[i] == predictedClasses[i]){
           numCorrectPredictions++;
           }
    }
return (float)numCorrectPredictions/(float)numObservations;
}

inferenceSamples::inferenceSamples(const std::string& testFile){
    std::ifstream fin(testFile.c_str());

    fin >>  numObservations; 
    fin >> numFeatures;
    Rprintf("\nThere are %d observations to test\n", numObservations);
    Rprintf("There are %d features in each observation\n", numFeatures);

    samplesMatrix.resize(numObservations);
    observationClasses.resize(numObservations);
		predictedClasses.resize(numObservations);

    for(int i = 0; i < numObservations; i++){
        fin >> observationClasses[i];
        samplesMatrix[i].resize(numFeatures);
        for(int j=0; j < numFeatures; j++){
            fin >> samplesMatrix[i][j];
        }
    }

    int eofTest;
    fin >> eofTest;
    if(!fin.eof()){
        Rprintf("test csv not exausted");
        fin.close();
    }else{
        fin.close();
    }
}


inferenceSamples::~inferenceSamples(){
}
