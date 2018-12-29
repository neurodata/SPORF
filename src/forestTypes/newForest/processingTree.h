#ifndef processingTree_h
#define processingTree_h

#include "obsIndexAndClassTuple.h"
#include "zipClassAndValue.h"

#include <vector>
#include <list>
#include <random>
#include <iostream>

namespace fp{


	template <typename T, typename Q> //
		class processingTree{
			protected:
				int treeNum;

				std::vector<obsIndexAndClassTuple<T> > obsInTree;
				std::vector<zipClassAndValue<T> > workingSet;

				std::list<processingNode<T, Q> > processedNodes;

				std::vector<int> potentialSamples;
				std::random_device rd; // obtain a random number from hardware
				std::mt19937 eng; // seed the generator
				std::uniform_int_distribution<> distr;

			public:
				processingTree(int numberOfObsInTree){
					obsInTree.resize(numberOfObsInTree);
					workingSet.resize(numberOfObsInTree);

					potentialSamples.resize(numberOfObsInTree);
					eng(rd()); // seed the generator
					distr(0, numberOfObsInTree-1);

					for(int i=0; i < numberOfObsInTree; ++i){
						potentialSamples[i] = i;
					}
				}

				inline void setTreeNum(int treeNumber){
					treeNum = treeNumber;
				}

				inline int returnTreeNum(){
					return treeNum;
				}

				inline void sortIndices(){
					std::sort(obsInTree.begin(), obsInTree.end());
				}

				inline void seedTree(){
					int numUnusedObs = fpSingleton::getSingleton().returnNumObs();
					int randomObsID;
					int tempMoveObs;
					for(int n=0; n<fpSingleton::getSingleton().returnNumObs(); n++){
						randomObsID = distr(eng);
						obsInTree[n].setObsIndex(potentialSamples[randomObsID]);

						if(randomObsID < numUnusedObs){
							--numUnusedObs;
							tempMoveObs = potentialSamples[numUnusedObs];
							potentialSamples[numUnusedObs] = potentialSamples[randomObsID];
							potentialSamples[randomObsID] = tempMoveObs;
						}
					}
				}

				inline void setClasses(){
					for(int n=0; n<fpSingleton::getSingleton().returnNumObs(); n++){
						obsInTree[n].setObsClass(fpSingleton::getSingleton().returnLabel(obsInTree[n].returnIndexOfObs()));
					}
				}


				inline void resetTree(){
					seedTree();
					sortIndices();
					setClasses();
				}

				inline void startTree(){
					processedNodes.emplace_back(returnObsInTreeStart(), returnObsInTreeEnd(),returnWorkingSetStart() ,returnWorkingSetEnd());
				}


				inline std::vector<obsIndexAndClassTuple<T> >::iterator returnObsInTreeStart(){
					return obsInTree.begin();
				}

				inline std::vector<obsIndexAndClassTuple<T> >::iterator returnObsInTreeEnd(){
					return obsInTree.end();
				}

				inline std::vector<zipClassAndValue<T> >::iterator returnWorkingSetStart(){
					return workingSet.begin();
				}

				inline std::vector<zipClassAndValue<T> >::iterator returnWorkingSetEnd(){
					return workingSet.end();
				}

		}; //processingTree.h
}//namespace fp
#endif //processingTree_h
