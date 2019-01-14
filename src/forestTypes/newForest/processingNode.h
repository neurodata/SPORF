#ifndef processingNode_h
#define processingNode_h

#include "inNodeClassTotals.h"
#include "obsIndexAndClassVec.h"
#include "zipClassAndValue.h"
#include "bestSplitInfo.h"
#include "nodeIterators.h"
#include "zipperIterators.h"
#include <assert.h>
#include "../../fpSingleton/fpSingleton.h"


#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <random>

namespace fp{

	template<typename T, typename Q>
		class processingNode{
			protected:
				int treeNum;
				int parentNodeNumber;
				int nodeNumber;
				int depth;

				bool isLeftNode;
				bool isLeftNodeBigger;
				bool leafNode;

				std::vector<Q> mtry;
				bestSplitInfo<T, Q> bestSplit;

				inNodeClassTotals propertiesOfThisNode;
				inNodeClassTotals propertiesOfLeftNode;
				inNodeClassTotals propertiesOfRightNode;

				nodeIterators nodeIndices;

				zipperIterators<int,T> zipIters;

				/*
					 inline void isFinished(){
					 return mtry.empty();
					 }
					 */

				inline void calcMtryForNode(std::vector<int>& featuresToTry){
					for (int i=0; i<fpSingleton::getSingleton().returnNumFeatures(); ++i){ 
						featuresToTry.push_back(i);
					}
					std::random_shuffle ( featuresToTry.begin(), featuresToTry.end() );
					featuresToTry.resize(fpSingleton::getSingleton().returnMtry());
				}

				inline void calcMtryForNode(std::vector<std::vector<int> >& featuresToTry){
					featuresToTry.resize(fpSingleton::getSingleton().returnMtry());
					int rndMtry;
					int rndFeature;
					for (int i=0; i < fpSingleton::getSingleton().returnMtry(); ++i){
						rndMtry = std::rand() % fpSingleton::getSingleton().returnMtry();
						rndFeature = std::rand() % fpSingleton::getSingleton().returnNumFeatures();
						featuresToTry[rndMtry].push_back(rndFeature);
					}
				}


				inline void resetLeftNode(){
					propertiesOfLeftNode.resetClassTotals();
				}

				inline void resetRightNode(){
					propertiesOfRightNode.copyInNodeClassTotals(propertiesOfThisNode);
				}


				inline void findBestSplit(Q& currMtry){
					resetLeftNode();
					resetRightNode();

					double tempImpurity;
					double currentBestImpurity =  bestSplit.returnImpurity();
					for(typename std::vector<zipClassAndValue<int,T> >::iterator it = zipIters.returnZipBegin(); it < zipIters.returnZipEnd()-1; ++it){
						propertiesOfLeftNode.incrementClass(it->returnObsClass());
						propertiesOfRightNode.decrementClass(it->returnObsClass());

						if(it->checkInequality(*(it+1))){
							tempImpurity = propertiesOfLeftNode.calcAndReturnImpurity() + propertiesOfRightNode.calcAndReturnImpurity();

							if(tempImpurity < bestSplit.returnImpurity()){
								bestSplit.setImpurity(tempImpurity);
								bestSplit.setSplitValue(it->midVal(*(it+1)));
							}
						}
					}
					if(bestSplit.returnImpurity() < currentBestImpurity){
						bestSplit.setFeature(currMtry);
					}
				}


				inline void setRootNodeIndices(obsIndexAndClassVec& indexHolder){
					nodeIndices.setInitialIterators(indexHolder);
				}

				inline void setRootNodeZipIters(typename std::vector<zipClassAndValue<int,T> >& zipper){
					zipIters.setZipIteratorsRoot(zipper);
				}

				inline void loadWorkingSet(int currMTRY){
					typename std::vector<zipClassAndValue<int,T> >::iterator zipIterator = zipIters.returnZipBegin();
					for(int classNum = 0; classNum < fpSingleton::getSingleton().returnNumClasses(); ++classNum){

						for(std::vector<int>::iterator q=nodeIndices.returnBeginIterator(classNum); q!=nodeIndices.returnEndIterator(classNum); ++q){
							fpSingleton::getSingleton().prefetchFeatureVal(currMTRY,*q);
						}


						for(std::vector<int>::iterator q=nodeIndices.returnBeginIterator(classNum); q!=nodeIndices.returnEndIterator(classNum); ++q){
							zipIterator->setPair(classNum, fpSingleton::getSingleton().returnFeatureVal(currMTRY,*q));
							++zipIterator;
						}
					}	
				}


				inline void setClassTotals(){
					propertiesOfThisNode.setupClassTotals(nodeIndices);
					bestSplit.setImpurity(propertiesOfThisNode.calcAndReturnImpurity());
				}


				inline void sortWorkingSet(){
					std::sort(zipIters.returnZipBegin(), zipIters.returnZipEnd());
				}


				inline void setVecOfSplitLocations(int fMtry){
					int tempIndex;

					for(int i = 0; i < fpSingleton::getSingleton().returnNumClasses(); ++i){
						std::vector<int>::iterator  lowerValueIndices = nodeIndices.returnBeginIterator(i);
						std::vector<int>::iterator  higherValueIndices = nodeIndices.returnEndIterator(i)-1;
						
					int numInClass = nodeIndices.returnEndIterator(i) - nodeIndices.returnBeginIterator(i);
					int numInNewClass = 0;

						if(nodeIndices.returnBeginIterator(i) == nodeIndices.returnEndIterator(i)){
							nodeIndices.loadSplitIterator(lowerValueIndices);
continue;
						}

						while(lowerValueIndices < higherValueIndices){
							while((lowerValueIndices < higherValueIndices) && (fpSingleton::getSingleton().returnFeatureVal(fMtry,*lowerValueIndices) <= bestSplit.returnSplitValue())){
								++lowerValueIndices;
							}

							if(lowerValueIndices != higherValueIndices){
								while((lowerValueIndices < higherValueIndices) && (fpSingleton::getSingleton().returnFeatureVal(fMtry,*higherValueIndices) > bestSplit.returnSplitValue())){
									--higherValueIndices;
								}

								if(lowerValueIndices != higherValueIndices){
									tempIndex = *lowerValueIndices;
									*lowerValueIndices = *higherValueIndices;
									*higherValueIndices = tempIndex;

									++lowerValueIndices;
									--higherValueIndices;
								}
							}
						}

						if(fpSingleton::getSingleton().returnFeatureVal(fMtry,*lowerValueIndices) <= bestSplit.returnSplitValue()){
							nodeIndices.loadSplitIterator(lowerValueIndices+1);
numInNewClass = (lowerValueIndices+1) - nodeIndices.returnBeginIterator(i) + nodeIndices.returnEndIterator(i) - (lowerValueIndices+1);
assert(numInNewClass == numInClass);
						assert(nodeIndices.returnEndIterator(i) - lowerValueIndices + 1 >= 0);
						assert(lowerValueIndices + 1 - nodeIndices.returnBeginIterator(i) >= 0);
						}else{
							nodeIndices.loadSplitIterator(lowerValueIndices);
numInNewClass = (lowerValueIndices) - nodeIndices.returnBeginIterator(i) + nodeIndices.returnEndIterator(i) - (lowerValueIndices);
assert(numInNewClass == numInClass);
						assert(nodeIndices.returnEndIterator(i) - lowerValueIndices >= 0);
						assert(lowerValueIndices - nodeIndices.returnBeginIterator(i) >= 0);
						}

					}
				}


				inline void setNodeIndices(nodeIterators& nodeIters){
					nodeIndices.setNodeIterators(nodeIters, isLeftNode);
				}


				inline void setZipIters(zipperIterators<int,T>& zipperIters, int numObjects){
					zipIters.setZipIterators(zipperIters, numObjects, isLeftNode);
				}

			public:

				processingNode(int tr, int pN): treeNum(tr), parentNodeNumber(pN),propertiesOfThisNode(fpSingleton::getSingleton().returnNumClasses()), propertiesOfLeftNode(fpSingleton::getSingleton().returnNumClasses()),propertiesOfRightNode(fpSingleton::getSingleton().returnNumClasses()),nodeIndices(fpSingleton::getSingleton().returnNumClasses()){}

				/*
					 inline void setRootClassTotals(){
					 propertiesOfThisNode.setupRootClassTotals(nodeIndices);
					 bestSplit.setImpurity(propertiesOfThisNode.calcAndReturnImpurity());
					 }
					 */


				inline void loadWorkingSet(std::vector<int>& currMTRY){
					/*
						 for(int q=0; q<baseUnprocessedNode<T>::obsIndices->returnInSampleSize(); q++){
						 fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[0],baseUnprocessedNode<T>::obsIndices->returnInSample(q));
						 }

						 for(int i =0; i < baseUnprocessedNode<T>::obsIndices->returnInSampleSize(); ++i){
						 baseUnprocessedNode<T>::featureHolder[i] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[0],baseUnprocessedNode<T>::obsIndices->returnInSample(i));
						 }
						 if(featuresToTry.back().size()>1){
						 for(unsigned int j =1; j < featuresToTry.back().size(); ++j){
						 for(int q=0; q<baseUnprocessedNode<T>::obsIndices->returnInSampleSize(); q++){
						 fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[j],baseUnprocessedNode<T>::obsIndices->returnInSample(q));
						 }

						 for(int i =0; i < baseUnprocessedNode<T>::obsIndices->returnInSampleSize(); ++i){
						 baseUnprocessedNode<T>::featureHolder[i] += fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[j],baseUnprocessedNode<T>::obsIndices->returnInSample(i));
						 }
						 }
						 }
						 */
				}







				inline void setupRoot(obsIndexAndClassVec& indexHolder, typename std::vector<zipClassAndValue<int,T> >& zipper){
					setRootNodeIndices(indexHolder);
					setClassTotals();
					setRootNodeZipIters(zipper);
					if(!propertiesOfThisNode.isNodePure()){
						//		calcMtryForNode(mtry);
					}
				}


				inline void setupNode(processingNode& parentNode, bool leftNode){
					setIsLeftNode(leftNode);
					setNodeIndices(parentNode.nodeIndices);
					setClassTotals();
					setZipIters(parentNode.zipIters, propertiesOfThisNode.returnNumItems());
				}


				inline void setupNode(nodeIterators& nodeIts, zipperIterators<int,T>& zips, bool leftNode){
					setIsLeftNode(leftNode);
					setNodeIndices(nodeIts);
					setClassTotals();
					setZipIters(zips, propertiesOfThisNode.returnNumItems());
					/*
					if(propertiesOfThisNode.returnNumItems() < 1 || propertiesOfThisNode.returnNumItems() > 150){
						std::cout << propertiesOfThisNode.returnNumItems() << "\n";
					}
					*/
				}


				inline bool leafPropertiesMet(){
					if(propertiesOfThisNode.isNodePure()){
						return true;
					}
					return propertiesOfThisNode.isSizeLTMinParent(fpSingleton::getSingleton().returnMinParent());
				}

				inline void setAsLeafNode(){
					leafNode = true;
				}

				inline void setAsInternalNode(){
					leafNode = false;
				}

				inline bool impurityImproved(){
					return bestSplit.returnImpurity() < propertiesOfThisNode.returnImpurity();  
				}


				inline void processNode(){
					if(leafPropertiesMet()){
						setAsLeafNode();
					}else{
						calcBestSplit();
						if(impurityImproved()){
							setAsInternalNode();
							setVecOfSplitLocations(bestSplit.returnFeatureNum());
						}else{
							setAsLeafNode();
						}
					}
				}

				inline bool isLeafNode(){
					return leafNode;
				}

				inline void calcBestSplitInfoForNode(int featureToTry){
					loadWorkingSet(featureToTry);
					sortWorkingSet();
					resetRightNode();
					resetLeftNode();
					//This next function finds and sets the best split... not just finds.
					findBestSplit(featureToTry);
				}


				inline void calcBestSplit(){
					calcMtryForNode(mtry);
					while(!mtry.empty()){
						calcBestSplitInfoForNode(mtry.back());
						mtry.pop_back();
					}
				}

				inline int returnNodeSize(){
					return propertiesOfThisNode.returnNumItems();
				}

				inline int returnNodeDepth(){
					return depth;
				}

				inline T returnNodeCutValue(){
					return bestSplit.returnSplitValue();
				}

				inline Q returnNodeCutFeature(){
					return bestSplit.returnFeatureNum();
				}

				inline bool returnIsLeftNode(){
					return isLeftNode;
				}

				inline void setIsLeftNode(bool leftNode){
					isLeftNode = leftNode;
				}

				inline int returnParentNodeNumber(){
					return parentNodeNumber;
				}

				inline int returnNodeIndices(){
					return parentNodeNumber;
				}

				inline nodeIterators& returnNodeIterators(){
					return nodeIndices;
				}

				inline zipperIterators<int,T>& returnZipIterators(){
					return zipIters;
				}

				inline bool isLeftChildLarger(){
					return nodeIndices.returnLeftChildSize() > propertiesOfThisNode.returnNumItems() - nodeIndices.returnLeftChildSize();
				}

				inline int returnNodeClass(){
					return propertiesOfThisNode.returnMaxClass();
				}

				//////////////////////////////////
				//testing functions -- not to be used in production
				//////////////////////////////////
				int inline exposeTreeNum(){
					return treeNum;
				}

				inline int exposeParentNode(){
					return parentNodeNumber;
				}

				inline std::vector<Q>& exposeMtry(){
					return mtry;
				}

				inline bestSplitInfo<T, Q>& exposeBestSplit(){
					return bestSplit;
				}

				inline inNodeClassTotals&  exposePropertiesOfThisNode(){
					return propertiesOfThisNode;
				}

				inline inNodeClassTotals& exposePropertiesOfLeftNode(){
					return propertiesOfLeftNode;
				}

				inline inNodeClassTotals& exposePropertiesOfRightNode(){
					return propertiesOfRightNode;
				}

				inline nodeIterators& exposeNodeIndices(){
					return nodeIndices;
				}

				inline zipperIterators<int,T>& exposeZipIters(){
					return zipIters;
				}

				inline void resetLeftNodeTest(){
					resetLeftNode();
				}

				inline void resetRightNodeTest(){
					resetRightNode();
				}


				inline void findBestSplitTest(Q& currMtry){
					findBestSplit(currMtry);
				}


				inline void loadWorkingSetTest(int currMTRY){
					loadWorkingSet(currMTRY);
				}


				inline void sortWorkingSetTest(){
					sortWorkingSet();
				}


				inline void setVecOfSplitLocationsTest(int fMtry){
					setVecOfSplitLocations(fMtry);
				}


		};

}//namespace fp
#endif //processingNode_h
