#ifndef processingNode_h
#define processingNode_h

#include "inNodeClassTotals.h"
#include "obsIndexAndClassTuple.h"
#include "zipClassAndValue.h"
#include "bestSplitInfo.h"


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

				std::unique_ptr<bestSplitInfo<T, Q> > bestSplit;


				std::unique_ptr<inNodeClassTotals> propertiesOfThisNode;
				std::unique_ptr<inNodeClassTotals> propertiesOfLeftNode;
				std::unique_ptr<inNodeClassTotals> propertiesOfRightNode;

				std::vector<obsIndexAndClassTuple<T> >::iterator zipOfIndexAndClassStart;
				std::vector<obsIndexAndClassTuple<T> >::iterator zipOfIndexAndClassEnd;

				std::vector<zipClassAndValue<T> >::iterator zipOfClassAndValueStart;
				std::vector<zipClassAndValue<T> >::iterator zipOfClassAndValueEnd;

				inline void loadWorkingSet(Q& currMTRY){
					//prefetch data and load.
				}

				inline void sortWorkingSet(){
					std::sort(zipOfClassAndValueStart, zipOfClassAndValueEnd);
				}

				inline void resetLeftNode(){
					propertiesOfLeftNode->resetClassTotals();
				}

				inline void resetRightNode(){
					propertiesOfRightNode->copyProperties(propertiesOfThisNode);
				}

				inline void isFinished(){
					return mtry.empty();
				}


				inline void findBestSplit(Q& currMtry){
					double tempImpurity;
					double currentBestImpurity = bestSplit.returnImpurity();
					for(vector<>::iterator it = zipOfClassAndValueStart; it != zipOfClassAndValueEnd; ++it){
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

			public:

				//needs parent node
				processingNode(std::vector<obsIndexAndClassTuple<T> >::iterator obsStart, std::vector<obsIndexAndClassTuple<T> >::iterator obsEnd, std::vector<zipClassAndValue<T> >::iterator zipStart, std::vector<zipClassAndValue<T> >::iterator zipEnd): zipOfIndexAndClassStart(obsStart), zipOfIndexAndClassend(obsEnd), zipOfClassAndValueStart(zipStart), zipOfClassAndValueEnd(zipEnd){}



				inline bool isLeafNode(){
					if (zipOfIndexAndClassEnd - zipOfIndexAndClassStart <= fpSingleton::getSingleton().minParent()){
						return true;
					}

					propertiesOfThisNode = make_unique<inNodeClassTotals>();
					//This has already been calculated, can be stored, and passed as argument.
					propertiesOfThisNode.findClassCardinalities(zipOfIndexAndClassStart,zipOfIndexAndClassEnd);

					if (propertiesOfThisNode.returnImpurity() == 0){
						return true;
					}
					return false;
				}

				inline void setupNode(){
					propertiesOfLeftNode = make_unique<inNodeClassTotals>();
					propertiesOfRightNode = make_unique<inNodeClassTotals>();
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

		};

}//namespace fp
#endif //processingNode_h
