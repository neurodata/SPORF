#ifndef processingNode_h
#define processingNode_h

#include "inNodeClassTotals.h"
#include "obsIndexAndClassVec.h"
#include "zipClassAndValue.h"
#include "bestSplitInfo.h"
#include "nodeIterators.h"


#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <random>

namespace fp{

	template<typename T, typename Q>
		class processingNode{
			protected:

				std::vector<Q> mtry;
				bestSplitInfo<T, Q> bestSplit();

				inNodeClassTotals propertiesOfThisNode;
				inNodeClassTotals propertiesOfLeftNode;
				inNodeClassTotals propertiesOfRightNode;

				nodeIterators nodeIndices;

				zipperIterators<int,T> zipIters;

				

				inline void loadWorkingSet(std::vector<Q>& currMTRY){
					//prefetch data and load.
				}

				inline void loadWorkingSet(Q currMTRY){
					//prefetch data and load.
				}
/*
				inline void sortWorkingSet(){
					std::sort(zipOfClassAndValueStart, zipOfClassAndValueEnd);
				}

				inline void resetLeftNode(){
					propertiesOfLeftNode.resetClassTotals();
				}

				inline void resetRightNode(){
					propertiesOfRightNode.copyInNodeClassTotals(propertiesOfThisNode);
				}

				inline void isFinished(){
					return mtry.empty();
				}


				inline void findBestSplit(Q& currMtry){
					double tempImpurity;
					double currentBestImpurity = bestSplit.returnImpurity();
					for(vector<>::iterator it = zipOfClassAndValueStart; it < zipOfClassAndValueEnd-1; ++it){
						leftClasses.incrementClass(it.returnObsClass());
						rightClasses.decrementClass(it.returnObsClass());

						if(checkInequalityWithNext(it+1)){
							tempImpurity = leftClasses.calcAndReturnImpurity() + rightClasses.calcAndReturnImpurity();

							if(tempImpurity < bestSplit.returnImpurity()){
								bestSplit.setImpurity(tempImpurity);
								bestSplit.setSplitValue(it.midVal(it+1));
							}
						}
					}
					if(bestSplit.returnImpurity() < currentBestImpurity){
						bestSplit.setFeature(currMtry);
					}
				}

				inline calcMtryForNode(std::vector<int>& featuresToTry){
					for (int i=0; i<fpSingleton::getSingleton().returnNumFeatures(); ++i) featuresToTry.push_back(i);
					std::random_shuffle ( featuresToTry.begin(), featuresToTry.end() );
					featuresToTry.resize(fpSingleton::getSingleton().returnMtry());
				}

				inline calcMtryForNode(std::vector<std::vector<int> >& featuresToTry){
					featuresToTry.resize(fpSingleton::getSingleton().returnMtry());
					int rndMtry;
					int rndFeature;
					for (int i=0; i < fpSingleton::getSingleton().returnMtry(); ++i){
						rndMtry = std::rand() % fpSingleton::getSingleton().returnMtry();
						rndFeature = std::rand() % fpSingleton::getSingleton().returnNumFeatures();
						featuresToTry[rndMtry].push_back(rndFeature);
					}
				}

				*/
			public:

				processingNode(int numClasses): propertiesOfThisNode(numClasses), propertiesOfLeftNode(numClasses),propertiesOfRightNode(numClasses),nodeIndices(numClasses){}

				/*

				inline bool isLeafNode(){
					if(propertiesOfThisNode.isNodePure()){
						return true;
					}
					return nodeIndices.isSizeLTMinParent(fpSingleton::getSingleton().minParent());
				}

				inline void setupNode(){
					calcMtryForNode(mtry);
				}

				inline void calcBestSplitInfoForNode(){
					for(auto i : mtry){
						loadWorkingSet(i);
						sortWorkingSet();
						resetRightNode();
						resetLeftNode();
						//This next function finds and sets the best split... not just finds.
						findBestSplit(i);
					}
				}

				inline void processNode(){
					if(isLeafNode()){
						//send node to actual tree container
					}else{
						setupNode();
						calcBestSplitInfoForNode();
						//split node if something found.
					}
				}
				*/

		};

}//namespace fp
#endif //processingNode_h
