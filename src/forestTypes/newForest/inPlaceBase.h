#ifndef inPlaceBase_h
#define inPlaceBase_h

#include "../../baseFunctions/fpBaseForest.h"
#include <vector>
#include <stdio.h>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include "treeStruct.h"

namespace fp {

	template <typename T>
		class inPlaceBase : public fpForestBase<T>
	{
		protected:
			std::vector<treeStruct<T, int> > trees;

		public:

			using fpForestBase<T>::fpForestBase;

			fpDisplayProgress printProgress;
			void printForestType(){
				std::cout << "This is an inPlaceBase forest.\n";
			}

			void changeForestSize(){
				trees.reserve(fpSingleton::getSingleton().returnNumTrees());
			}

			void growTrees(){

				std::vector<int> testIndices;
				obsIndexAndClassVec indexHolder(fpSingleton::getSingleton().returnNumClasses());

				for(int i = 0; i < fpSingleton::getSingleton().returnNumObservations(); ++i){
					testIndices.push_back(i);
				}

				for(int i = 0; i < (int)testIndices.size(); ++i){
					indexHolder.insertIndex(testIndices[i], fpSingleton::getSingleton().returnLabel(i));
				}

				std::vector<zipClassAndValue<int, T> > zipVec(fpSingleton::getSingleton().returnNumObservations());

				//#pragma omp parallel for
				for(int i = 0; i < fpSingleton::getSingleton().returnNumTrees(); ++i){
					printProgress.displayProgress(i);
					trees.emplace_back(indexHolder, zipVec);
					trees.back().createTree();
					
				}
				std::cout << "\n"<< std::flush;
			}

			inline void checkParameters(){
				//TODO: check parameters to make sure they make sense for this forest type.
				;
			}

			void treeStats(){
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

			void growForest(){
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


			float testForest(){
				float numTried = 0;
				float numWrong = 0;

				for (int i = 0; i <fpSingleton::getSingleton().returnNumObservations();i++){
					++numTried;
					int predClass = predictClass(i);

					if(predClass != fpSingleton::getSingleton().returnTestLabel(i)){
						++numWrong;
					}
				}
				std::cout << "\nnumWrong= " << numWrong << "\n";

				return numWrong/numTried;
			}
	};

}// namespace fp
#endif //inPlaceBase_h
