#ifndef smallSampleBase_h
#define smallSampleBase_h
//#include "../baseFunctions/fpForestBase.h"
#include "../baseFunctions/fpUtils.h"
#include <vector>
#include <stdio.h>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include "rfTree.h"

namespace fp {

	template <typename T>
		class smallSampleBase : public fpForestBase
	{
		protected:
			std::vector<rfTree<T> > trees;

		public:
			fpForestClassificationBase(){
				std::srand(unsigned(std::time(0)));
			}

			void printForestType(){
				std::cout << "This is a basic classification forest.\n";
			}

			void changeForestSize(){
				trees.resize(fpSingleton::getSingleton().returnNumTrees());
			}

			void growTrees(){
				auto startTime = std::chrono::system_clock::now();
				auto stopTime = std::chrono::system_clock::now();
				//	std::chrono::duration<double, std::chrono::seconds> diffSeconds;
				std::chrono::seconds diffSeconds;
				std::chrono::seconds updateTime(30);

				std::cout << "starting tree 1" << std::flush;

				for(unsigned int i = 0; i < trees.size(); ++i){
					stopTime = std::chrono::high_resolution_clock::now();
					diffSeconds =	std::chrono::duration_cast<std::chrono::seconds>(stopTime - startTime);
					if(diffSeconds > updateTime){
						std::cout << "..." << i << std::flush;
						startTime = std::chrono::high_resolution_clock::now();
					}
					trees[i].growTree();
				}
				std::cout << "\n"<< std::flush;
			}

			inline void checkParameters(){
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
				changeForestSize();
				growTrees();
				treeStats();
				//	printTree0();
			}

			int predictClass(int observationNumber){

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
#endif //smallSampleBase_h
