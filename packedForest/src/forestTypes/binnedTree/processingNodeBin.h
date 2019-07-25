#ifndef processingNodeBin_h
#define processingNodeBin_h

#include "inNodeClassTotals.h"
#include "obsIndexAndClassVec.h"
#include "zipClassAndValue.h"
#include "bestSplitInfo.h"
#include "nodeIterators.h"
#include "zipperIterators.h"
#include <assert.h>
#include "../../fpSingleton/fpSingleton.h"
#include "../../baseFunctions/pdqsort.h"
#include "../../baseFunctions/MWC.h"
#include "../../baseFunctions/weightedFeature.h"


#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <random>

namespace fp{

	template<typename T, typename Q>
		class processingNodeBin{
			protected:
				int treeNum;
				int parentNodeNumber;
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

				randomNumberRerFMWC* randNum;
				inline void calcMtryForNode(std::vector<int>& featuresToTry){
					for (int i=0; i<fpSingleton::getSingleton().returnNumFeatures(); ++i){
						featuresToTry.push_back(i);
					}

					int tempSwap;

					// This is an efficient way to shuffle the first "mtry" elements of the feature vector
					// in order to sample features w/o replacement.
					for(int locationToMove = 0; locationToMove < fpSingleton::getSingleton().returnMtry(); locationToMove++){
						int randomPosition = randNum->gen(fpSingleton::getSingleton().returnNumFeatures()-locationToMove)+locationToMove;
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
					int mtryDensity = (int)((double)fpSingleton::getSingleton().returnMtry() * fpSingleton::getSingleton().returnMtryMult());
					for (int i = 0; i < mtryDensity; ++i){
						rndMtry = randNum->gen(fpSingleton::getSingleton().returnMtry());
						rndFeature = randNum->gen(fpSingleton::getSingleton().returnNumFeatures());
						featuresToTry[rndMtry].push_back(rndFeature);
					}
				}


				inline void calcMtryForNode(std::vector<weightedFeature>& featuresToTry){
					featuresToTry.resize(fpSingleton::getSingleton().returnMtry());
					int methodToUse = fpSingleton::getSingleton().returnMethodToUse();
					assert(methodToUse == 1 || methodToUse == 2);

					switch(methodToUse){
						case 1:{
							randMatTernary(featuresToTry);
							break;
							}
						case 2:{
							randMatImagePatch(featuresToTry, paramsRandMatImagePatch());
							break;
							}
					}
				}


				inline void randMatTernary(std::vector<weightedFeature>& featuresToTry){
					int rndMtry;
					int rndFeature;
					int rndWeight;
					int mtryDensity = (int)((double)fpSingleton::getSingleton().returnMtry() * fpSingleton::getSingleton().returnMtryMult());
					for (int i = 0; i < mtryDensity; ++i){
						rndMtry = randNum->gen(fpSingleton::getSingleton().returnMtry());
						rndFeature = randNum->gen(fpSingleton::getSingleton().returnNumFeatures());
						featuresToTry[rndMtry].returnFeatures().push_back(rndFeature);
						rndWeight = (randNum->gen(2)%2) ? 1 : -1;
						assert(rndWeight==1 || rndWeight==-1);
						featuresToTry[rndMtry].returnWeights().push_back(rndWeight);
					}
				}


				inline std::vector<std::vector<int> > paramsRandMatImagePatch(){
					// Preset parameters
					const int& imageHeight = fpSingleton::getSingleton().returnImageHeight();
					const int& imageWidth = fpSingleton::getSingleton().returnImageWidth();

					const int& patchHeightMax = fpSingleton::getSingleton().returnPatchHeightMax();
					const int& patchHeightMin = fpSingleton::getSingleton().returnPatchHeightMin();
					const int& patchWidthMax  = fpSingleton::getSingleton().returnPatchWidthMax();
					const int& patchWidthMin  = fpSingleton::getSingleton().returnPatchWidthMin();

					// A vector of vectors that specifies the parameters
					// for each patch: < <Height>, <Width>, <TopLeft> >
					std::vector<std::vector<int> > heightWidthTop(3, std::vector<int>(fpSingleton::getSingleton().returnMtry()));

					int deltaH;
					int deltaW;
					int topLeftSeed;
					// The weight is currently hard-coded to 1.

					// Loop over mtry to load random patch dimensions
					// and top left position.
					for (int k = 0; k < fpSingleton::getSingleton().returnMtry(); k++){

						heightWidthTop[0][k] = randNum->gen(patchHeightMax - patchHeightMin + 1) + patchHeightMin; //sample from [patchHeightMin, patchHeightMax]
						heightWidthTop[1][k] = randNum->gen(patchWidthMax - patchWidthMin + 1) +  patchWidthMin;    //sample from [patchWidthMin, patchWidthMax]
						// Using the above, 1-pixel patches are possible ... [JLP]

						// compute the difference between the image dimensions and the current random patch dimensions for sampling
						deltaH = imageHeight - heightWidthTop[0][k] + 1;
						deltaW = imageWidth  - heightWidthTop[1][k] + 1;

						// Sample the top left pixel from the available pixels (due to buffering).
						topLeftSeed = randNum->gen(deltaH * deltaW);

						// Convert the top-left-seed value to it's appropriate index in the full image.
						heightWidthTop[2][k] = (topLeftSeed % deltaW) + (imageWidth * floor(topLeftSeed / deltaW));

						assert((heightWidthTop[2][k] % imageWidth) < deltaW); // check that TopLeft pixel is in the correct column.
						assert((int)(heightWidthTop[2][k] / imageWidth) < deltaH); // check that TopLeft pixel is in the correct row.
					}

					return(heightWidthTop);
				} // End paramsRandMatImagePatch


				inline void randMatImagePatch(std::vector<weightedFeature>& featuresToTry, std::vector<std::vector<int> > patchPositions){
					assert((int)(patchPositions[0].size()) == fpSingleton::getSingleton().returnMtry());

					// Preset parameters
					const int& imageWidth = fpSingleton::getSingleton().returnImageWidth();

					int pixelIndex = -1;
					for (int k = 0; k < fpSingleton::getSingleton().returnMtry(); k++){
						for (int row = 0; row < patchPositions[0][k]; row++) {
							for (int col = 0; col < patchPositions[1][k]; col++) {
								pixelIndex = patchPositions[2][k] + col + (imageWidth * row);
								featuresToTry[k].returnFeatures().push_back(pixelIndex);
								featuresToTry[k].returnWeights().push_back(1); // weight hard-coded to 1.
								}
						} // Could possibly turn this into one for-loop somehow later. [JLP]
					}
				} // END randMatStructured


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

						int sizeToPrefetch = globalPrefetchSize;            
						if(nodeIndices.returnEndIterator(classNum) - nodeIndices.returnBeginIterator(classNum) < 32){ 
							sizeToPrefetch = nodeIndices.returnEndIterator(classNum) - nodeIndices.returnBeginIterator(classNum);
						}

						for(std::vector<int>::iterator q=nodeIndices.returnBeginIterator(classNum); q!=nodeIndices.returnBeginIterator(classNum)+sizeToPrefetch; ++q){
							fpSingleton::getSingleton().prefetchFeatureVal(currMTRY,*q);
						}

						for(std::vector<int>::iterator q=nodeIndices.returnBeginIterator(classNum); q!=nodeIndices.returnEndIterator(classNum)-sizeToPrefetch; ++q){
							fpSingleton::getSingleton().prefetchFeatureVal(currMTRY,*(q+sizeToPrefetch));
							zipIterator->setPair(classNum, fpSingleton::getSingleton().returnFeatureVal(currMTRY,*q));
							++zipIterator;
						}

						for(std::vector<int>::iterator q=nodeIndices.returnEndIterator(classNum)-sizeToPrefetch; q!=nodeIndices.returnEndIterator(classNum); ++q){
							zipIterator->setPair(classNum, fpSingleton::getSingleton().returnFeatureVal(currMTRY,*q));
							++zipIterator;
						}

					}	
				}



				inline void loadWorkingSet(std::vector<int>& currMTRY){

					typename std::vector<zipClassAndValue<int,T> >::iterator zipIterator = zipIters.returnZipBegin();
					T accumulator;

					for(int classNum = 0; classNum < fpSingleton::getSingleton().returnNumClasses(); ++classNum){

						int sizeToPrefetch = globalPrefetchSize;            
						if(nodeIndices.returnEndIterator(classNum) - nodeIndices.returnBeginIterator(classNum) < 32){ 
							sizeToPrefetch = nodeIndices.returnEndIterator(classNum) - nodeIndices.returnBeginIterator(classNum);
						}

						for(std::vector<int>::iterator q=nodeIndices.returnBeginIterator(classNum); q!=nodeIndices.returnBeginIterator(classNum)+sizeToPrefetch; ++q){
							for(auto i : currMTRY){
								fpSingleton::getSingleton().prefetchFeatureVal(i,*q);
							}
						}

						for(std::vector<int>::iterator q=nodeIndices.returnBeginIterator(classNum); q!=nodeIndices.returnEndIterator(classNum)-sizeToPrefetch; ++q){
							accumulator=0;
							for(auto i : currMTRY){
								fpSingleton::getSingleton().prefetchFeatureVal(i,*(q+sizeToPrefetch));
								accumulator+= fpSingleton::getSingleton().returnFeatureVal(i,*q);
							}
							zipIterator->setPair(classNum,accumulator);
							++zipIterator;
						}

						for(std::vector<int>::iterator q=nodeIndices.returnEndIterator(classNum)-sizeToPrefetch; q!=nodeIndices.returnEndIterator(classNum); ++q){
							accumulator=0;
							for(auto i : currMTRY){
								accumulator+= fpSingleton::getSingleton().returnFeatureVal(i,*q);
							}
							zipIterator->setPair(classNum,accumulator);
							++zipIterator;
						}

					}
				}


				inline void loadWorkingSet(weightedFeature& currMTRY){

					typename std::vector<zipClassAndValue<int,T> >::iterator zipIterator = zipIters.returnZipBegin();
					T accumulator;
					int weightNum;

					for(int classNum = 0; classNum < fpSingleton::getSingleton().returnNumClasses(); ++classNum){

						int sizeToPrefetch = globalPrefetchSize;            
						if(nodeIndices.returnEndIterator(classNum) - nodeIndices.returnBeginIterator(classNum) < 32){ 
							sizeToPrefetch = nodeIndices.returnEndIterator(classNum) - nodeIndices.returnBeginIterator(classNum);
						}

						for(std::vector<int>::iterator q=nodeIndices.returnBeginIterator(classNum); q!=nodeIndices.returnBeginIterator(classNum)+sizeToPrefetch; ++q){
							for(auto i : currMTRY.returnFeatures()){
								fpSingleton::getSingleton().prefetchFeatureVal(i,*q);
							}
						}

						for(std::vector<int>::iterator q=nodeIndices.returnBeginIterator(classNum); q!=nodeIndices.returnEndIterator(classNum)-sizeToPrefetch; ++q){
							accumulator=0;
							weightNum = 0;
							for(auto i : currMTRY.returnFeatures()){
								fpSingleton::getSingleton().prefetchFeatureVal(i,*(q+sizeToPrefetch));
								accumulator+= fpSingleton::getSingleton().returnFeatureVal(i,*q)*currMTRY.returnWeights()[weightNum++];
							}
							zipIterator->setPair(classNum,accumulator);
							++zipIterator;
						}

						for(std::vector<int>::iterator q=nodeIndices.returnEndIterator(classNum)-sizeToPrefetch; q!=nodeIndices.returnEndIterator(classNum); ++q){
							accumulator=0;
							weightNum = 0;
							for(auto i : currMTRY.returnFeatures()){
								accumulator+= fpSingleton::getSingleton().returnFeatureVal(i,*q)*currMTRY.returnWeights()[weightNum++];
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
					pdqsort_branchless(zipIters.returnZipBegin(), zipIters.returnZipEnd());
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


				inline void setVecOfSplitLocations(weightedFeature fMtry){

					for(int i = 0; i < fpSingleton::getSingleton().returnNumClasses(); ++i){
						std::vector<int>::iterator  lowerValueIndices = nodeIndices.returnBeginIterator(i);
						std::vector<int>::iterator  higherValueIndices = nodeIndices.returnEndIterator(i);
						std::vector<int>::iterator  smallerNumberIndex = nodeIndices.returnBeginIterator(i);

						T aggregator;	
						int weightNum;
						for(; lowerValueIndices < higherValueIndices; ++lowerValueIndices){
							aggregator = 0;
							weightNum = 0;
							for(auto i : fMtry.returnFeatures()){
								aggregator += fpSingleton::getSingleton().returnFeatureVal(i,*lowerValueIndices)*fMtry.returnWeights()[weightNum++];
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

				processingNodeBin(int tr, int pN, int d, randomNumberRerFMWC& randNumBin): treeNum(tr), parentNodeNumber(pN), depth(d), propertiesOfThisNode(fpSingleton::getSingleton().returnNumClasses()), propertiesOfLeftNode(fpSingleton::getSingleton().returnNumClasses()),propertiesOfRightNode(fpSingleton::getSingleton().returnNumClasses()),nodeIndices(fpSingleton::getSingleton().returnNumClasses()){
					randNum = &randNumBin;	
				}





				inline void setupRoot(obsIndexAndClassVec& indexHolder, typename std::vector<zipClassAndValue<int,T> >& zipper){
					setRootNodeIndices(indexHolder);
					setClassTotals();
					setRootNodeZipIters(zipper);
				}


				inline void setupNode(processingNodeBin& parentNode, bool leftNode){
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
					if (depth >= fpSingleton::getSingleton().returnMaxDepth()){
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
				inline void calcBestSplitInfoForNode(Q featureToTry){
					loadWorkingSet(featureToTry);
					sortWorkingSet();
					resetRightNode();
					resetLeftNode();
					findBestSplit(featureToTry);
				}


				inline void calcBestSplit(){
					calcMtryForNode(mtry);
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

				inline void calcMtryForNodeTest(std::vector<Q> &featuresToTry){
					calcMtryForNode(featuresToTry);
				}

				inline std::vector<std::vector<int> > paramsRandMatImagePatchTest(){
					return(paramsRandMatImagePatch());
				}

				inline void randMatImagePatchTest(std::vector<weightedFeature>& featuresToTry, std::vector<std::vector<int> > patchPositions){
					return(randMatImagePatch(featuresToTry, patchPositions));
				}
};

}//namespace fp
#endif //processingNodeBin_h
