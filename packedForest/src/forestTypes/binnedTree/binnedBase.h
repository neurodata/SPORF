#ifndef binnedBase_h
#define binnedBase_h

#include "../../baseFunctions/fpForestBase.h"
#include <vector>
#include <stdio.h>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include "binStruct.h"
#include <random>
#include <numeric>

#include <iostream>
#include <algorithm>
#include <map>


namespace fp {

	template <typename T, typename Q>
		class binnedBase : public fpForestBase<T>
	{
		protected:
			std::vector<binStruct<T, Q> > bins;
			int numBins;
			std::map<std::pair<int, int>, double> pairMat;
			std::vector<int> binSizes;
			std::vector<int> binSeeds;


			inline void checkParameters(){
				if(fpSingleton::getSingleton().returnNumTreeBins() > fpSingleton::getSingleton().returnNumTrees()){
					fpSingleton::getSingleton().setNumTreeBins(fpSingleton::getSingleton().returnNumTrees());
				}

				if(fpSingleton::getSingleton().returnNumTreeBins() < 1){
					fpSingleton::getSingleton().setNumTreeBins(fpSingleton::getSingleton().returnNumThreads());
				}
			}

		public:

			~binnedBase(){}
			binnedBase(){
				checkParameters();
				numBins =  fpSingleton::getSingleton().returnNumTreeBins();
				generateSeedsForBins();
			}

			inline void generateSeedsForBins(){
				binSeeds.resize(numBins);
				for(int i = 0; i < numBins; ++i){
					binSeeds[i] = fpSingleton::getSingleton().genRandom(std::numeric_limits<int>::max());
				}
			}

			inline void printForestType(){
				std::cout << "This is a binned forest.\n";
			}

			inline void changeForestSize(){
				bins.reserve(numBins);
			}

			inline void calcBinSizes(){
				int minBinSize = fpSingleton::getSingleton().returnNumTrees()/numBins;
				binSizes.resize(numBins,minBinSize);
				int remainingTreesToBin = fpSingleton::getSingleton().returnNumTrees()-minBinSize*numBins;
				while(remainingTreesToBin != 0){
					++binSizes[--remainingTreesToBin];
				}
			}


			inline void growBins(){

				calcBinSizes();

				fpDisplayProgress printProgress;
				bins.resize(numBins);
#pragma omp parallel for num_threads(fpSingleton::getSingleton().returnNumThreads())
				for(int j = 0; j < numBins; ++j){
					bins[j].createBin(binSizes[j], binSeeds[j]);
				}
				std::cout << "\n"<< std::flush;
			}

			inline float reportOOB(){
				return -1;
			}
			inline std::map<std::string, int> calcBinStats(){
				int maxDepth=0;
				int totalLeafNodes=0;
				int totalLeafDepth=0;

				int tempMaxDepth;
				for(int i = 0; i < numBins; ++i){
					tempMaxDepth = bins[i].returnMaxDepth();
					maxDepth = ((maxDepth < tempMaxDepth) ? tempMaxDepth : maxDepth);

					totalLeafNodes += bins[i].returnNumLeafNodes();
					totalLeafDepth += bins[i].returnLeafDepthSum();
				}

				std::map<std::string, int> binStats;
				binStats["maxDepth"] = maxDepth;
				binStats["totalLeafDepth"] = totalLeafDepth;
				binStats["totalLeafNodes"] = totalLeafNodes;
				return binStats;
			}

			inline void binStats(){
				std::map<std::string, int> binStats = calcBinStats();

				std::cout << "max depth: " << binStats["maxDepth"] << "\n";
				std::cout << "avg leaf node depth: " << float(binStats["totalLeafDepth"])/float(binStats["totalLeafNodes"]) << "\n";
				std::cout << "num leaf nodes: " << binStats["totalLeafNodes"] << "\n";
			}

			void printBin0(){
				bins[0].printBin();
			}

			inline void growForest(){
				//	checkParameters();
				//TODO: change this so forest isn't grown dynamically.
				//changeForestSize();
				growBins();
				binStats();
			}


			inline int predictClass(int observationNumber){
				std::vector<int> predictions(fpSingleton::getSingleton().returnNumClasses(),0);

#pragma omp parallel for num_threads(fpSingleton::getSingleton().returnNumThreads())
				for(int k = 0; k < numBins; ++k){
					bins[k].predictBinObservation(observationNumber, predictions);
				}

				assert(std::accumulate(predictions.begin(), predictions.end(),0) == fpSingleton::getSingleton().returnNumTrees());

				int bestClass = 0;
				for(int j = 1; j < fpSingleton::getSingleton().returnNumClasses(); ++j){
					if(predictions[bestClass] < predictions[j]){
						bestClass = j;
					}
				}
				return bestClass;
			}


			inline int predictClass(std::vector<T>& observation){
				std::vector<int> predictions(fpSingleton::getSingleton().returnNumClasses(),0);

#pragma omp parallel for num_threads(fpSingleton::getSingleton().returnNumThreads())
				for(int k = 0; k < numBins; ++k){
					bins[k].predictBinObservation(observation, predictions);
				}
				int bestClass = 0;
				for(int j = 1; j < fpSingleton::getSingleton().returnNumClasses(); ++j){
					if(predictions[bestClass] < predictions[j]){
						bestClass = j;
					}
				}
				return bestClass;
			}


			inline std::vector<int> predictClassPost(std::vector<T>& observation){
				std::vector<int> predictions(fpSingleton::getSingleton().returnNumClasses(),0);

#pragma omp parallel for num_threads(fpSingleton::getSingleton().returnNumThreads())
				for(int k = 0; k < numBins; ++k){
					bins[k].predictBinObservation(observation, predictions);
				}
				return predictions;
			}


			inline int predictClass(const T* observation){
				/*
					 std::vector<int> predictions(fpSingleton::getSingleton().returnNumClasses(),0);

#pragma omp parallel for num_threads(fpSingleton::getSingleton().returnNumThreads())
for(int k = 0; k < numBins; ++k){
bins[k].predictBinObservation(observation, predictions);
}

int bestClass = 0;
for(int j = 1; j < fpSingleton::getSingleton().returnNumClasses(); ++j){
if(predictions[bestClass] < predictions[j]){
bestClass = j;
}
}
return bestClass;
*/
			return 0;
			}

inline std::map<std::pair<int, int>, double> returnPairMat(){
                                        return pairMat;
                                }

inline float testForest(){
	int numTried = 0;
	int numWrong = 0;

	for (int i = 0; i <fpSingleton::getSingleton().returnNumObservations();i++){
		++numTried;
		int predClass = predictClass(i);

		if(predClass != fpSingleton::getSingleton().returnTestLabel(i)){
			++numWrong;
		}
	}
	std::cout << "\nnumWrong= " << numWrong << "\n";

	return (float)numWrong/(float)numTried;
}
};

}// namespace fp
#endif //binnedBase_h
