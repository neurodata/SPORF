#ifndef fpForestClassification_h
#define fpForestClassification_h

#include "../../../baseFunctions/fpForestBase.h"
#include <vector>
#include <stdio.h>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <omp.h>
#include "rfTree.h"
#include <map>

namespace fp {

	template <typename T>
		class fpForestClassificationBase : public fpForestBase<T>
	{
		protected:
			std::vector<rfTree<T> > trees;

		public:
			fpDisplayProgress printProgress;

			//			using fpForestBase<T>::fpForestBase;
			~fpForestClassificationBase(){}

			inline void printForestType(){
				std::cout << "This is a basic classification forest.\n";
			}

			inline void changeForestSize(){
				trees.resize(fpSingleton::getSingleton().returnNumTrees());
			}

			inline void growTrees(){

#pragma omp parallel for num_threads(fpSingleton::getSingleton().returnNumThreads())
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

			inline std::map<std::string, float> calcTreeStats(){
				// rewrite as a struct in a top level header file.
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

				std::map<std::string, float> treeStats;
				treeStats["maxDepth"] = (float) maxDepth;
				treeStats["totalLeafDepth"] = (float) totalLeafDepth;
				treeStats["totalLeafNodes"] = (float) totalLeafNodes;
				treeStats["OOBaccuracy"] = reportOOB();
				return treeStats;
			}

			inline void treeStats(){
				std::map<std::string, float> treeStats = calcTreeStats();

				std::cout << "max depth: " << treeStats["maxDepth"] << "\n";
				std::cout << "avg depth: " << float(treeStats["totalLeafDepth"])/float(treeStats["totalLeafNodes"]) << "\n";
				std::cout << "num leaf nodes: " << treeStats["totalLeafNodes"] << "\n";
				std::cout << "OOB Accuracy: " << treeStats["OOBaccuracy"] << "\n";
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


			inline std::vector<int> predictClassPost(std::vector<T>& observation){
				std::vector<int> classTally(fpSingleton::getSingleton().returnNumClasses(),0);
				for(int i = 0; i < fpSingleton::getSingleton().returnNumTrees(); ++i){
					++classTally[trees[i].predictObservation(observation)];
				}
				return classTally;
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

			inline float reportOOB(){
				// for tree_i in trees sum OOB_i and normalize by the number of trees.
				float oobi = 0;
				int treesWithOOB = 0;
				for (unsigned int i = 0; i < trees.size(); ++i){
					if (trees[i].returnTotalOOB() > 0) {
					  oobi += trees[i].returnOOB();
					  treesWithOOB++;
					}
				}
				// need to divide by the number of trees that had non-zero OOB points.

				return oobi / (float) treesWithOOB;
			}
	};

}// namespace fp
#endif //fpForestClassification_h
