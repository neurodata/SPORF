#include "inferenceSamples.h"

void inferenceSamples::percentRight(){
    int numCorrectPredictions = 0;
    for(int i = 0; i < numObservations; i++){
if(0){
           printf("observation%d actual %d predicted %d\n", i, observationClasses[i],predictedClasses[i]);
           }
           if(observationClasses[i] == predictedClasses[i]){
           numCorrectPredictions++;
           }
    }

     printf("%f%% of the predictions were correct\n", 100.0*(float)numCorrectPredictions/(float)numObservations);

}

inferenceSamples::inferenceSamples(const std::string& testFile){
    std::ifstream fin(testFile.c_str());

    fin >>  numObservations; 
    fin >> numFeatures;
   // printf("\nThere are %d observations to test\n", numObservations);
   // printf("There are %d features in each observation\n", numFeatures);

    samplesMatrix = new double*[numObservations];
    observationClasses = new int[numObservations];
    predictedClasses = new int[numObservations];

    if(samplesMatrix == NULL || observationClasses == NULL){
        printf("memory for samples was not allocated\n");
        exit(1);
    }

    for(int i = 0; i < numObservations; i++){
        fin >> observationClasses[i];
        samplesMatrix[i] = new double[numFeatures];
        for(int j=0; j < numFeatures; j++){
            fin >> samplesMatrix[i][j];
        }
    }

    int eofTest;
    fin >> eofTest;
    if(!fin.eof()){
        printf("test csv not exausted");
        exit(1);
    }else{
        fin.close();
    }
}


inferenceSamples::~inferenceSamples(){
    delete[] observationClasses;
    delete[] predictedClasses;
    for(int i = 0; i < numObservations; i++){
        delete[] samplesMatrix[i];
    }
    delete[] samplesMatrix;
}
