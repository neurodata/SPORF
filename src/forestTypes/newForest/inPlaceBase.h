#ifndef inPlaceBase_h
#define inPlaceBase_h

#include "../../baseFunctions/fpForestBase.h"
#include <vector>
#include <stdio.h>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include "treeStruct.h"
#include <random>

#include <iostream>
#include <algorithm>


namespace fp {

	template <typename T, typename Q>
		class inPlaceBase : public fpForestBase<T>
	{
		protected:
			std::vector<treeStruct<T, Q> > trees;

			std::vector<int> nodeIndices;
		public:

			~inPlaceBase(){}
			inPlaceBase(){
				nodeIndices.resize(fpSingleton::getSingleton().returnNumObservations());
				for(int i = 0; i < fpSingleton::getSingleton().returnNumObservations(); ++i){
					nodeIndices[i] =i;
				}
			}

			fpDisplayProgress printProgress;

			inline void printForestType(){
				std::cout << "This is an inPlaceBase forest.\n";
			}

			inline void changeForestSize(){
				trees.reserve(fpSingleton::getSingleton().returnNumTrees());
			}


			inline void setSharedVectors(obsIndexAndClassVec& indicesInNode){

				std::random_device rd; // obtain a random number from hardware
				std::mt19937 eng(rd());
				std::uniform_int_distribution<> distr(0, fpSingleton::getSingleton().returnNumObservations()-1);

				indicesInNode.resetVectors();

				int numUnusedObs = fpSingleton::getSingleton().returnNumObservations();
				int randomObsID;
				int tempMoveObs;

				for(int n = 0; n < fpSingleton::getSingleton().returnNumObservations(); n++){
					randomObsID = distr(eng);

					indicesInNode.insertIndex(nodeIndices[randomObsID], fpSingleton::getSingleton().returnLabel(nodeIndices[randomObsID]));

					if(randomObsID < numUnusedObs){
						--numUnusedObs;
						tempMoveObs = nodeIndices[numUnusedObs];
						nodeIndices[numUnusedObs] = nodeIndices[randomObsID];
						nodeIndices[randomObsID] = tempMoveObs;
					}
				}

			}


			inline void growTrees(){

				obsIndexAndClassVec indexHolder(fpSingleton::getSingleton().returnNumClasses());
				std::vector<zipClassAndValue<int, T> > zipVec(fpSingleton::getSingleton().returnNumObservations());

				//#pragma omp parallel for
				for(int i = 0; i < fpSingleton::getSingleton().returnNumTrees(); ++i){
					setSharedVectors(indexHolder);
					printProgress.displayProgress(i);
					trees.emplace_back(indexHolder, zipVec);
					//	LIKWID_MARKER_START("createTree");
					trees.back().createTree();
					//	LIKWID_MARKER_STOP("createTree");
				}
				nodeIndices.clear();
				std::cout << "\n"<< std::flush;
			}

			inline void checkParameters(){
				//TODO: check parameters to make sure they make sense for this forest type.
				;
			}

			inline void treeStats(){
				int maxDepth=0;
				int totalLeafNodes=0;
				int totalLeafDepth=0;

				int tempMaxDepth;
				for(int i = 0; i < fpSingleton::getSingleton().returnNumTrees(); ++i){
					tempMaxDepth = trees[i].returnMaxDepth();
					maxDepth = ((maxDepth < tempMaxDepth) ? tempMaxDepth : maxDepth);

					totalLeafNodes += trees[i].returnNumLeafNodes();
					totalLeafDepth += trees[i].returnLeafDepthSum();
				}

				std::cout << "max depth: " << maxDepth << "\n";
				std::cout << "avg depth: " << float(totalLeafDepth)/float(totalLeafNodes) << "\n";
				std::cout << "num leaf nodes: " << totalLeafNodes << "\n";
			}

			void printTree0(){
				trees[0].printTree();
			}

			inline void growForest(){
				//	checkParameters();
				//TODO: change this so forest isn't grown dynamically.
				//changeForestSize();
				growTrees();
				treeStats();
			}

			inline int predictClass(int observationNumber){
				std::vector<int> classTally(fpSingleton::getSingleton().returnNumClasses(),0);
				for(int i = 0; i < fpSingleton::getSingleton().returnNumTrees(); ++i){
					++classTally[trees[i].predictObservation(observationNumber)];
				}
				int bestClass = 0;
				for(int j = 1; j < fpSingleton::getSingleton().returnNumClasses(); ++j){
					if(classTally[bestClass] < classTally[j]){
						bestClass = j;
					}
				}
				return bestClass;
			}

			inline int predictClass(std::vector<T>& observation){
				std::vector<int> classTally(fpSingleton::getSingleton().returnNumClasses(),0);
				for(int i = 0; i < fpSingleton::getSingleton().returnNumTrees(); ++i){
					++classTally[trees[i].predictObservation(observation)];
				}
				int bestClass = 0;
				for(int j = 1; j < fpSingleton::getSingleton().returnNumClasses(); ++j){
					if(classTally[bestClass] < classTally[j]){
						bestClass = j;
					}
				}
				return bestClass;
			}


inline int predictClass(const T* observation){
	/*
				std::vector<int> classTally(fpSingleton::getSingleton().returnNumClasses(),0);
				for(int i = 0; i < fpSingleton::getSingleton().returnNumTrees(); ++i){
					++classTally[trees[i].predictObservation(observation)];
				}
				int bestClass = 0;
				for(int j = 1; j < fpSingleton::getSingleton().returnNumClasses(); ++j){
					if(classTally[bestClass] < classTally[j]){
						bestClass = j;
					}
				}
				return bestClass;
				*/
	return 0;
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
#endif //inPlaceBase_h
