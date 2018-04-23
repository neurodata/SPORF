#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <typeinfo>
#include <ctime>
#include <chrono>
#include <omp.h>
#include <thread>
#include "inferenceSamples.h"
#include "improv1.h"
#include "improv3.h"
#include "improv7.h"
#include "improv4.h"
#include "improv5.h"
#include "improv6.h"
#include "improv2.h"
#include "improv8.h"
#include "naive.h"

int main(int argc, char* argv[]) {
  if (argc < 3){
    printf("packer requires at least 2 arguments.\n");
    return -1;
  }
  int numOfBatches;
  int depthIntertwined;
  int numCores=0;

  unsigned numThreadsPossible = std::thread::hardware_concurrency();
  omp_set_num_threads(numThreadsPossible/2);

  if (argc < 4){
    numOfBatches =0;
    depthIntertwined =0;
    numCores =0;
  }else if(argc == 4){
    numOfBatches =atoi(argv[3]);
    depthIntertwined = 0;
  }else if(argc == 5){
    numOfBatches =atoi(argv[3]);
    depthIntertwined =atoi(argv[4]);
  }else if(argc == 6){
    numOfBatches =atoi(argv[3]);
    depthIntertwined =atoi(argv[4]);
    numCores =  atoi(argv[5]);
  }

  int algorithmToRun = atoi(argv[1]);
  int runPrediction = atoi(argv[2]);
  const std::string forestFileName = "res/forest.csv";
  const std::string testFileName = "res/testObservations.csv";
  const std::string traversalFileName = "res/traversal.csv";
  auto start_time = std::chrono::high_resolution_clock::now();
  auto stop_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::micro> diffMilli;

  inferenceSamples travs(traversalFileName);
  inferenceSamples observations(testFileName);

  if(algorithmToRun == 0){
    printf("running naive src=csv, pred=%d\n",runPrediction);
    naive tester(forestFileName,1);
    tester.printForest();
    printf("size of a node is %d\n",(int) sizeof(padNode));
    printf("starting run\n");
    tester.makePredictions(observations);
    start_time = std::chrono::high_resolution_clock::now();
    if(runPrediction){
      tester.makePredictions(observations);
    }
    stop_time = std::chrono::high_resolution_clock::now();
  }else if(algorithmToRun ==1){
    printf("running improv1 src=csv, pred=%d\n",runPrediction);
    improv1 tester(forestFileName,1);
    tester.printForest();
    printf("size of a node is %d\n",(int) sizeof(padNode));
    printf("starting run\n");
    tester.makePredictions(observations);
    start_time = std::chrono::high_resolution_clock::now();
    if(runPrediction){
      tester.makePredictions(observations);
    }
    stop_time = std::chrono::high_resolution_clock::now();
  }else if(algorithmToRun ==2){
    printf("running improv2 src=csv, pred=%d\n",runPrediction);
    improv2 tester(forestFileName,1);
    tester.printForest();
    printf("size of a node is %d\n",(int) sizeof(padNode));
    printf("starting run\n");
    tester.makePredictions(observations);
    start_time = std::chrono::high_resolution_clock::now();
    if(runPrediction){
      tester.makePredictions(observations);
    }
    stop_time = std::chrono::high_resolution_clock::now();
  }else if(algorithmToRun ==3){
    printf("running improv3 src=csv, pred=%d\n",runPrediction);
    improv3 tester(forestFileName,1, travs);
    tester.printForest();
    printf("size of a node is %d\n",(int) sizeof(padNode));
    printf("starting run\n");
    tester.makePredictions(observations);
    start_time = std::chrono::high_resolution_clock::now();
    if(runPrediction){
      tester.makePredictions(observations);
    }
    stop_time = std::chrono::high_resolution_clock::now();
  }else if(algorithmToRun ==4){
    printf("running improv4 src=csv, pred=%d\n",runPrediction);
    improv4 tester(forestFileName,1, travs);
    tester.printForest();
    printf("size of a node is %d\n",(int) sizeof(padNode));
    printf("starting run\n");
    tester.makePredictions(observations);
    start_time = std::chrono::high_resolution_clock::now();
    if(runPrediction){
      tester.makePredictions(observations);
    }
    stop_time = std::chrono::high_resolution_clock::now();

  }else if(algorithmToRun ==5){
    printf("running improv5 src=csv, pred=%d, batches=%d, head=%d\n",runPrediction,numOfBatches, depthIntertwined);
    improv5 tester(forestFileName,1, observations,numOfBatches,depthIntertwined);
    tester.printForest();
    printf("size of a node is %d\n",(int) sizeof(padNode));
    printf("starting run\n");
    tester.makePredictions(observations);
    start_time = std::chrono::high_resolution_clock::now();
    if(runPrediction){
      tester.makePredictions(observations);
    }
    stop_time = std::chrono::high_resolution_clock::now();
  }else if(algorithmToRun ==6){
    printf("running improv6 src=csv, pred=%d, batches=%d, head=%d\n",runPrediction,numOfBatches,depthIntertwined );
    improv6 tester(forestFileName,1, observations,numOfBatches,depthIntertwined);
    tester.printForest();
    printf("size of a node is %d\n",(int) sizeof(padNode));
    printf("starting run\n");
    tester.makePredictions(observations);
    start_time = std::chrono::high_resolution_clock::now();
    if(runPrediction){
      tester.makePredictions(observations);
    }
    stop_time = std::chrono::high_resolution_clock::now();
  }else if(algorithmToRun ==7){
    printf("running improv7 src=csv, pred=%d, batches=%d, head=%d\n",runPrediction,numOfBatches,depthIntertwined );
    improv7 tester(forestFileName,1, observations,numOfBatches,depthIntertwined);
    tester.printForest();
    printf("size of a node is %d\n",(int) sizeof(padNode));
    printf("starting run\n");
    tester.makePredictions(observations);
    start_time = std::chrono::high_resolution_clock::now();
    if(runPrediction){
      tester.makePredictions(observations);
    }
    stop_time = std::chrono::high_resolution_clock::now();

  }else if(algorithmToRun ==8){
    printf("running improv8 src=csv, pred=%d, batches=%d, head=%d\n",runPrediction,numOfBatches,depthIntertwined );
    improv8 tester(forestFileName,1, observations,numOfBatches,depthIntertwined);
    tester.printForest();
    printf("size of a node is %d\n",(int) sizeof(padNode));
    printf("starting run\n");
    int currentPred;
    std::cout<<"starting run with "<< numCores <<" cores and " << tester.numbin() << " bins."<<std::endl;
    //one time to warm cache
#pragma omp parallel for num_threads(numCores) schedule(static) private(currentPred)
    for(int q=0; q<observations.numObservations;q++){
      currentPred = tester.makePrediction(observations.samplesMatrix[q],1);
      observations.predictedClasses[q] = currentPred;
    }
    start_time = std::chrono::high_resolution_clock::now();
#pragma omp parallel for num_threads(numCores) schedule(static) private(currentPred)
    for(int q=0; q<observations.numObservations;q++){
      currentPred = tester.makePrediction(observations.samplesMatrix[q], 1);
      observations.predictedClasses[q] = currentPred;
    }
    stop_time = std::chrono::high_resolution_clock::now();
  }


  diffMilli = stop_time - start_time;
  std::cout<< std::fixed << "Time to test observations: "<< diffMilli.count()<< " us" <<std::endl;

  std::ofstream outfile;
  outfile.open("runTimes.csv", std::ios_base::app);
  outfile << std::fixed << algorithmToRun <<  " , " << diffMilli.count()<< std::endl;

  observations.percentRight();
  return 0;
}
