#ifndef fpRerFBase_h
#define fpRerFBase_h

#include "../../../baseFunctions/fpForestBase.h"
#include <vector>
#include <stdio.h>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include "rerfTree.h"
#include <map>
#include <limits>

namespace fp {

	template <typename T>
		class fpRerFBase : public fpForestBase<T>
	{
		protected:
			std::vector<rerfTree<T> > trees;

		public:

			~fpRerFBase(){}

			fpDisplayProgress printProgress;
			inline void printForestType(){
				std::cout << "This is a rerf forest.\n";
			}

			inline void changeForestSize(){
				trees.resize(fpSingleton::getSingleton().returnNumTrees());
			}

			inline void growTrees(){

				//fpSingleton::getSingleton().printXValues();
#pragma omp parallel for num_threads(fpSingleton::getSingleton().returnNumThreads())
				for(int i = 0; i < (int)trees.size(); ++i){
					printProgress.displayProgress(i);
					trees[i].growTree();
				}
				std::cout << " done growing forest.\n"<< std::flush;
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

                                if (treesWithOOB > 0) {
				  return oobi / (float) treesWithOOB;
                                } else {
                                  return std::numeric_limits<double>::quiet_NaN();
                                }
			}

			inline void checkParameters(){
				//TODO: check parameters to make sure they make sense for this forest type.
				;
			}

			inline std::map<std::string, int> calcTreeStats(){
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

				std::map<std::string, int> treeStats;
				treeStats["maxDepth"] = maxDepth;
				treeStats["totalLeafDepth"] = totalLeafDepth;
				treeStats["totalLeafNodes"] = totalLeafNodes;
				treeStats["OOBaccuracy"] = reportOOB();
				return treeStats;
			}

			inline void treeStats(){
				std::map<std::string, int> treeStats = calcTreeStats();

				std::cout << "max depth: " << treeStats["maxDepth"] << "\n";
				std::cout << "avg depth: " << float(treeStats["totalLeafDepth"])/float(treeStats["totalLeafNodes"]) << "\n";
				std::cout << "num leaf nodes: " << treeStats["totalLeafNodes"] << "\n";
				std::cout << "OOB Accuracy: " << treeStats["OOBaccuracy"] << "\n";
			}

			void printTree0(){
				trees[0].printTree();
			}

			void growForest(){
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
#endif //fpRerFBase_h
