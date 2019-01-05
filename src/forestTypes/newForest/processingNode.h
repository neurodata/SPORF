#ifndef processingNode_h
#define processingNode_h

#include "inNodeClassTotals.h"
#include "obsIndexAndClassVec.h"
#include "zipClassAndValue.h"
#include "bestSplitInfo.h"
#include "nodeIterators.h"
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
				int parentNode;

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

			public:

				processingNode(int tr, int pN): treeNum(tr), parentNode(pN),propertiesOfThisNode(fpSingleton::getSingleton().returnNumClasses()), propertiesOfLeftNode(fpSingleton::getSingleton().returnNumClasses()),propertiesOfRightNode(fpSingleton::getSingleton().returnNumClasses()),nodeIndices(fpSingleton::getSingleton().returnNumClasses()){}


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


				inline void resetLeftNode(){
					propertiesOfLeftNode.resetClassTotals();
				}

				inline void resetRightNode(){
					propertiesOfRightNode.copyInNodeClassTotals(propertiesOfThisNode);
				}


				inline void setRootNodeIndices(obsIndexAndClassVec& indexHolder){
					nodeIndices.setInitialIterators(indexHolder);
				}

				inline void setRootNodeZipIters(typename std::vector<zipClassAndValue<int,T> >& zipper){
					zipIters.setZipIterators(zipper);
				}

				inline void setRootClassTotals(){
					propertiesOfThisNode.setupRootClassTotals(nodeIndices);
				}


				inline void loadWorkingSet(std::vector<Q>& currMTRY){
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




				inline void loadWorkingSet(Q currMTRY, int numClasses){
					typename std::vector<zipClassAndValue<int,T> >::iterator zipIterator = zipIters.returnZipBegin();
					for(int classNum = 0; classNum < numClasses; ++classNum){

						for(std::vector<int>::iterator q=nodeIndices.returnBeginIterator(classNum); q!=nodeIndices.returnEndIterator(classNum); ++q){
							fpSingleton::getSingleton().prefetchFeatureVal(currMTRY,*q);
						}


						for(std::vector<int>::iterator q=nodeIndices.returnBeginIterator(classNum); q!=nodeIndices.returnEndIterator(classNum); ++q){
							zipIterator->setPair(classNum, fpSingleton::getSingleton().returnFeatureVal(currMTRY,*q));
							++zipIterator;
						}
					}	
				}
				//	inline void setNodeIndices(nodeIterators& nodeIts){
				//		nodeIndices=nodeIts;
				//	}

				//	inline void setupNode(){
				//		setNodeIndices();
				//calcMtryForNode(mtry);
				//	}


				inline void sortWorkingSet(){
					std::sort(zipIters.returnZipBegin(), zipIters.returnZipEnd());
				}


				inline void setupRoot(obsIndexAndClassVec& indexHolder, typename std::vector<zipClassAndValue<int,T> >& zipper){
					setRootNodeIndices(indexHolder);
					setRootNodeZipIters(zipper);
					setRootClassTotals();
					bestSplit.setImpurity(propertiesOfThisNode.calcAndReturnImpurity());
					if(!propertiesOfThisNode.isNodePure()){
						calcMtryForNode(mtry);
					}
				}

				/*

					 inline bool isLeafNode(){
					 if(propertiesOfThisNode.isNodePure()){
					 return true;
					 }
					 return nodeIndices.isSizeLTMinParent(fpSingleton::getSingleton().minParent());
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

//testing functions
int inline exposeTreeNum(){
	return treeNum;
}

inline int exposeParentNode(){
	return parentNode;
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

};

}//namespace fp
#endif //processingNode_h
