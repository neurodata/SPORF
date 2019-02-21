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
#include "../../baseFunctions/pdqsort.h"


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
					 inline void calcMtryForNode(std::vector<int>& featuresToTry){
					 for (int i=0; i<fpSingleton::getSingleton().returnNumFeatures(); ++i){
					 featuresToTry.push_back(i);
					 }

					 std::random_device rd; // obtain a random number from hardware
					 std::mt19937 eng(rd()); // seed the generator

					 int tempSwap;

					 for(int locationToMove = 0; locationToMove < fpSingleton::getSingleton().returnMtry(); locationToMove++){
					 std::uniform_int_distribution<> distr(locationToMove, fpSingleton::getSingleton().returnNumFeatures()-1);
					 int randomPosition = distr(eng);

					 tempSwap = featuresToTry[locationToMove];
					 featuresToTry[locationToMove] = featuresToTry[randomPosition];
					 featuresToTry[randomPosition] = tempSwap;
					 }

					 featuresToTry.resize(fpSingleton::getSingleton().returnMtry());
					 }
					 */

				inline void calcMtryForNode(std::vector<int>& featuresToTry){
					for (int i=0; i<fpSingleton::getSingleton().returnNumFeatures(); ++i){
						featuresToTry.push_back(i);
					}

					int tempSwap;

					for(int locationToMove = 0; locationToMove < fpSingleton::getSingleton().returnMtry(); locationToMove++){
						int randomPosition = fpSingleton::getSingleton().genRandom(fpSingleton::getSingleton().returnNumFeatures()-locationToMove)+locationToMove;
						tempSwap = featuresToTry[locationToMove];
						featuresToTry[locationToMove] = featuresToTry[randomPosition];
						featuresToTry[randomPosition] = tempSwap;
					}

					featuresToTry.resize(fpSingleton::getSingleton().returnMtry());
				}



				inline void calcMtryForNode(std::vector<std::vector<int> >& featuresToTry){
					featuresToTry.resize(fpSingleton::getSingleton().returnMtry());
					int rndMtry;
					int rndFeature;
					for (int i=0; i < fpSingleton::getSingleton().returnMtry(); ++i){
						rndMtry = fpSingleton::getSingleton().genRandom(fpSingleton::getSingleton().returnMtry());
						rndFeature = fpSingleton::getSingleton().genRandom(fpSingleton::getSingleton().returnNumFeatures());
						featuresToTry[rndMtry].push_back(rndFeature);
					}
				}

				/*
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
					 */


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
							//fpSingleton::getSingleton().prefetchFeatureVal(currMTRY,*q);
						}


						for(std::vector<int>::iterator q=nodeIndices.returnBeginIterator(classNum); q!=nodeIndices.returnEndIterator(classNum); ++q){
							zipIterator->setPair(classNum, fpSingleton::getSingleton().returnFeatureVal(currMTRY,*q));
							++zipIterator;
						}
					}	
				}



				inline void loadWorkingSet(std::vector<int>& currMTRY){

					typename std::vector<zipClassAndValue<int,T> >::iterator zipIterator = zipIters.returnZipBegin();
					T accumulator;

					for(int classNum = 0; classNum < fpSingleton::getSingleton().returnNumClasses(); ++classNum){

						/*
							 for(std::vector<int>::iterator q=nodeIndices.returnBeginIterator(classNum); q!=nodeIndices.returnEndIterator(classNum); ++q){
						//fpSingleton::getSingleton().prefetchFeatureVal(currMTRY,*q);
						}
						*/

						for(std::vector<int>::iterator q=nodeIndices.returnBeginIterator(classNum); q!=nodeIndices.returnEndIterator(classNum); ++q){
							accumulator=0;
							for(auto i : currMTRY){
								accumulator+=	fpSingleton::getSingleton().returnFeatureVal(i,*q);
							}
							zipIterator->setPair(classNum,accumulator);
							++zipIterator;
						}
					}

				}


				inline void setClassTotals(){
					propertiesOfThisNode.setupClassTotals(nodeIndices);
					bestSplit.setImpurity(propertiesOfThisNode.calcAndReturnImpurity());
				}


				inline void sortWorkingSet(){
					//pdqsort(zipIters.returnZipBegin(), zipIters.returnZipEnd());
					pdqsort_branchless(zipIters.returnZipBegin(), zipIters.returnZipEnd());
					//std::sort(zipIters.returnZipBegin(), zipIters.returnZipEnd());
				}


				inline void setVecOfSplitLocations(int fMtry){

					for(int i = 0; i < fpSingleton::getSingleton().returnNumClasses(); ++i){
						std::vector<int>::iterator  lowerValueIndices = nodeIndices.returnBeginIterator(i);
						std::vector<int>::iterator  higherValueIndices = nodeIndices.returnEndIterator(i);
						std::vector<int>::iterator  smallerNumberIndex = nodeIndices.returnBeginIterator(i);

						for(; lowerValueIndices < higherValueIndices; ++lowerValueIndices){
							if(fpSingleton::getSingleton().returnFeatureVal(fMtry,*lowerValueIndices) <= bestSplit.returnSplitValue()){
								std::iter_swap(smallerNumberIndex, lowerValueIndices);
								++smallerNumberIndex;
							}
						}
						nodeIndices.loadSplitIterator(smallerNumberIndex);
					}
				}


				inline void setVecOfSplitLocations(std::vector<int> fMtry){

					for(int i = 0; i < fpSingleton::getSingleton().returnNumClasses(); ++i){
						std::vector<int>::iterator  lowerValueIndices = nodeIndices.returnBeginIterator(i);
						std::vector<int>::iterator  higherValueIndices = nodeIndices.returnEndIterator(i);
						std::vector<int>::iterator  smallerNumberIndex = nodeIndices.returnBeginIterator(i);

						T aggregator;	
						for(; lowerValueIndices < higherValueIndices; ++lowerValueIndices){
							aggregator = 0;
							for(auto i : fMtry){
								aggregator += fpSingleton::getSingleton().returnFeatureVal(i,*lowerValueIndices);
							}
							if(aggregator <= bestSplit.returnSplitValue()){
								std::iter_swap(smallerNumberIndex, lowerValueIndices);
								++smallerNumberIndex;
							}
						}
						nodeIndices.loadSplitIterator(smallerNumberIndex);
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





				inline void setupRoot(obsIndexAndClassVec& indexHolder, typename std::vector<zipClassAndValue<int,T> >& zipper){
					setRootNodeIndices(indexHolder);
					setClassTotals();
					setRootNodeZipIters(zipper);
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
					//LIKWID_MARKER_START("loadNode");
					loadWorkingSet(featureToTry);
					//LIKWID_MARKER_STOP("loadNode");
					//LIKWID_MARKER_START("sort");
					sortWorkingSet();
					//LIKWID_MARKER_STOP("sort");
					resetRightNode();
					resetLeftNode();
					//This next function finds and sets the best split... not just finds.
					//LIKWID_MARKER_START("split");
					findBestSplit(featureToTry);
					//LIKWID_MARKER_STOP("split");
				}

inline void calcBestSplitInfoForNode(std::vector<int> featureToTry){
					loadWorkingSet(featureToTry);
					sortWorkingSet();
					resetRightNode();
					resetLeftNode();
					findBestSplit(featureToTry);
				}


				inline void calcBestSplit(){
					//LIKWID_MARKER_START("mtry");
					calcMtryForNode(mtry);
					//LIKWID_MARKER_STOP("mtry");
					while(!mtry.empty()){
						calcBestSplitInfoForNode(mtry.back());
						removeTriedMtry();
					}
				}

				inline void removeTriedMtry(){
					if(bestSplit.perfectSplitFound()){
						mtry.clear();
					}else{
						mtry.pop_back();
					}
				}

				inline int returnNodeSize(){
					return propertiesOfThisNode.returnNumItems();
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
