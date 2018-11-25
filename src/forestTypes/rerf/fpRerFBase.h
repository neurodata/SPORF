#ifndef fpRerF_h
#define fpRerf_h

#include "../../baseFunctions/fpForestBase.h"
#include "../../baseFunctions/displayProgress.h"
#include <vector>
#include <stdio.h>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include "rerfTree.h"

namespace fp {

	template <typename T>
		class fpRerFBase : public fpForestBase
	{
		protected:
			std::vector<rerfTree<T> > trees;

		public:

			fpRerFBase(){
				std::srand(unsigned(std::time(0)));
			}

			fpDisplayProgress printProgress;
			void printForestType(){
				std::cout << "This is a rerf forest.\n";
			}

			void changeForestSize(){
				trees.resize(fpSingleton::getSingleton().returnNumTrees());
			}

			void growTrees(){

				for(unsigned int i = 0; i < trees.size(); ++i){
					printProgress.displayProgress(i);
					trees[i].growTree();
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
				changeForestSize();
				growTrees();
				treeStats();
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
#endif //fpForestClassification_h
