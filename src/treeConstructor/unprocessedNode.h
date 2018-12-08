#ifndef rfunprocessedNode_h
#define rfunprocessedNode_h
#include "../baseFunctions/fpSplit.h"
#include <vector>
#include <random>

namespace fp{


	template <typename T> //
		class unprocessedNode{
			protected:
				int parentID;
				int depth;
				double nodeImpurity; //lower impurity is better
				bool isLeftNode; //in order to set parent node with location

				splitInfo<T> bestSplitInfo;

				stratifiedInNodeClassIndices* obsIndices;
				stratifiedInNodeClassIndices* leftIndices;
				stratifiedInNodeClassIndices* rightIndices;

				std::vector<int> featuresToTry;
				std::vector<T> featureHolder;
				std::vector<int> labelHolder;

			public:
				unprocessedNode(int numObsForRoot):  parentID(0), depth(0), isLeftNode(true){
					obsIndices = new stratifiedInNodeClassIndices(numObsForRoot);
					leftIndices=NULL;
					rightIndices=NULL;
				}

				unprocessedNode(int parentID, int dep, bool isLeft):parentID(parentID),depth(dep),isLeftNode(isLeft){
					leftIndices=NULL;
					rightIndices=NULL;
				}

				~unprocessedNode(){
					//	delete leftIndices;
					//	delete rightIndices;
				}


				inline stratifiedInNodeClassIndices* returnLeftIndices(){
					return leftIndices;
				}

				inline stratifiedInNodeClassIndices* returnRightIndices(){
					return rightIndices;
				}

				inline int returnParentID(){
					return parentID;
				}

				inline double returnNodeImpurity(){
					return nodeImpurity;
				}

				inline bool returnIsLeftNode(){
					return isLeftNode;
				}

				inline int returnDepth(){
					return depth;
				}

				inline int returnBestFeature(){
					return bestSplitInfo.returnFeatureNum();
				}

				inline double returnBestImpurity(){
					return bestSplitInfo.returnImpurity();
				}

				inline T returnBestCutValue(){
					return bestSplitInfo.returnSplitValue();
				}

				inline void setHolderSizes(){
					obsIndices->initializeBinnedSamples();
					if(obsIndices->useBin()){
						labelHolder.resize(obsIndices->returnBinnedSize());
						featureHolder.resize(obsIndices->returnBinnedSize());
					}else{
						labelHolder.resize(obsIndices->returnInSampleSize());
						featureHolder.resize(obsIndices->returnInSampleSize());
					}
				}

				inline int returnInSampleSize(){
					return obsIndices->returnInSampleSize();
				}

				inline int returnOutSampleSize(){
					return obsIndices->returnOutSampleSize();
				}

				inline int returnOutSampleError(int classNum){
					int totalRight=0;
					//	//for(int i : obsIndices->returnOutSamples()){
					//	for(unsigned int i =0; i <  obsIndices->returnSize(); ++i){
					//		if (i==classNum){
					//		++totalRight;
					//		}
					//		}
					return totalRight;
				}


				inline void pickMTRY(){
					for (int i=0; i<fpSingleton::getSingleton().returnNumFeatures(); ++i) featuresToTry.push_back(i);
					std::random_shuffle ( featuresToTry.begin(), featuresToTry.end() );
					featuresToTry.resize(fpSingleton::getSingleton().returnMtry());
				}

				inline int returnMTRYVectorSize(){
					return featuresToTry.size();
				}

				inline void loadLabelHolder(){
					if(obsIndices->useBin()){
						for(int i =0; i < obsIndices->returnBinnedSize(); ++i){
							labelHolder[i] = fpSingleton::getSingleton().returnLabel(obsIndices->returnBinnedInSample(i));
						}
					}else{
						for(int i =0; i < obsIndices->returnInSampleSize(); ++i){
							labelHolder[i] = fpSingleton::getSingleton().returnLabel(obsIndices->returnInSample(i));
						}
					}
				}


				inline void loadFeatureHolder(){
					if(obsIndices->useBin()){
						for(int q=0; q<obsIndices->returnBinnedSize(); q++){
							fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back(),obsIndices->returnBinnedInSample(q));
						}

						for(int i =0; i < obsIndices->returnBinnedSize(); ++i){
							featureHolder[i] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back(),obsIndices->returnBinnedInSample(i));
						}
					}else{

						for(int q=0; q<obsIndices->returnInSampleSize(); q++){
							fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back(),obsIndices->returnInSample(q));
						}

						for(int i =0; i < obsIndices->returnInSampleSize(); ++i){
							featureHolder[i] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back(),obsIndices->returnInSample(i));
						}
					}
				}

				inline float calculateNodeImpurity(){
					return obsIndices->returnImpurity();
				}


				inline void setupNode(){
					pickMTRY();
					setHolderSizes();
					loadLabelHolder();
					nodeImpurity = calculateNodeImpurity();
				}


				inline void setBestSplit(splitInfo<T> tempSplit){
					if(tempSplit.returnImpurity() < bestSplitInfo.returnImpurity()){
						bestSplitInfo = tempSplit;
					}
				}


				inline void loadIndices(stratifiedInNodeClassIndices* indices){
					obsIndices = indices;
				}

				
				inline bool goLeft(const int& index){
					int inIndex = index;
					int featureNum = bestSplitInfo.returnFeatureNum();
					if(featureNum == -1){
						std::cout << "found it\n";
						exit(1);
					}
					double featureVal = fpSingleton::getSingleton().returnFeatureVal(featureNum,inIndex);
					double splitVal = bestSplitInfo.returnSplitValue();
					if(featureVal < splitVal ){
						return true;
					}else{
						return false;
					}
				}


				inline void moveDataLeftOrRight(){
					if(leftIndices !=NULL){
						delete leftIndices;
					}
					if(rightIndices !=NULL){
						delete rightIndices;
					}

					leftIndices = new stratifiedInNodeClassIndices();
					rightIndices = new stratifiedInNodeClassIndices();

					int lNum =0;
					int rNum =0;
					for (int i=0; i < obsIndices->returnInSampleSize();++i){
						if(goLeft(obsIndices->returnInSample(i))){
							++lNum;
							leftIndices->addIndexToInSamples(obsIndices->returnInSample(i));	
						}else{
							++rNum;
							rightIndices->addIndexToInSamples(obsIndices->returnInSample(i));	
						}
					}

					for (int i=0; i < obsIndices->returnOutSampleSize();++i){
						if(goLeft(obsIndices->returnInSample(i))){
							leftIndices->addIndexToOutSamples(obsIndices->returnInSample(i));	
						}else{
							rightIndices->addIndexToOutSamples(obsIndices->returnInSample(i));	
						}
					}

					delete obsIndices;
					obsIndices = NULL;
				}


				inline void findBestSplit(){
					//timeLogger logTime;
					split<T> findSplit(labelHolder); //This is done twice
					while(!featuresToTry.empty()){
						//logTime.startGiniTimer();
						loadFeatureHolder();
						//logTime.stopGiniTimer();
						setBestSplit(findSplit.giniSplit(featureHolder ,featuresToTry.back()));

						featuresToTry.pop_back();
					}
				}


				inline int returnMaxClass(){
					if(nodeImpurity==0){
						return labelHolder[0];
					} else {
						classTotals findMaxClass;
						findMaxClass.findNumClasses(labelHolder);
						return findMaxClass.returnLargestClass();
					}
				}

				}; //unprocessedNode.h
		}//namespace fp
#endif //unprocessedNode_h
