#ifndef rfunprocessedNode_h
#define rfunprocessedNode_h
//#include "rfNode.h"
//#include "../baseFunctions/fpUtils.h"
#include "../baseFunctions/fpSplit.h"
//#include <stdio.h>
#include <vector>
#include <random>

namespace fp{


	template <typename T> //
		class unprocessedNode{
			protected:
				int parentID;
				int depth;
				float nodeImpurity; //lower impurity is better
				bool isLeftNode; //in order to set parent node with location

				splitInfo<T> bestSplitInfo;

				//split* nodeSplit;

				inNodeClassIndices* obsIndices;
				inNodeClassIndices* leftIndices;
				inNodeClassIndices* rightIndices;

				std::vector<int> featuresToTry;
				std::vector<T> featureHolder;
				std::vector<int> labelHolder;

			public:
				unprocessedNode(int numObsForRoot):  parentID(0), depth(0), isLeftNode(true){
					obsIndices = new inNodeClassIndices(numObsForRoot);
				}

				unprocessedNode(int parentID, int depth, bool isLeft):parentID(parentID),depth(depth),isLeftNode(isLeft){}

				~unprocessedNode(){
					if(obsIndices != NULL){
						//delete obsIndices;
					}
				}


	inline int returnIndTotal(){
return obsIndices->sumIndices();
	}

				inline inNodeClassIndices* returnLeftIndices(){
					return leftIndices;
				}

				inline inNodeClassIndices* returnRightIndices(){
					return rightIndices;
				}

				inline int returnParentID(){
					return parentID;
				}

				inline float returnNodeImpurity(){
					return nodeImpurity;
				}

				inline bool returnIsLeftNode(){
					return isLeftNode;
				}

				inline bool returnDepth(){
					return depth;
				}

				inline int returnBestFeature(){
					return bestSplitInfo.returnFeatureNum();
				}

				inline T returnBestImpurity(){
					return bestSplitInfo.returnImpurity();
				}

				inline T returnBestCutValue(){
					return bestSplitInfo.returnSplitValue();
				}

				inline void setHolderSizes(){
					labelHolder.resize(obsIndices->returnInSampleSize());
					featureHolder.resize(obsIndices->returnInSampleSize());
				}

				inline int returnInSampleSize(){
					return obsIndices->returnInSampleSize();
				}

				inline int returnOutSampleSize(){
					return obsIndices->returnOutSampleSize();
				}

				inline int returnOutSampleError(int classNum){
					int totalRight=0;
					for(int i : obsIndices->returnOutSamples()){
						if (i==classNum){
							++totalRight;
						}
					}
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
					for(int i =0; i < obsIndices->returnInSampleSize(); ++i){
						labelHolder[i] = fpSingleton::getSingleton().returnLabel(obsIndices->returnInSample(i));
					}
				}

				inline void loadFeatureHolder(){
					for(int i =0; i < obsIndices->returnInSampleSize(); ++i){
						featureHolder[i] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back(),obsIndices->returnInSample(i));
					}
				}

				inline float calculateNodeImpurity(std::vector<int> labels){
					split<T>	nodeSplit(labels); //This is done twice
					return nodeSplit.returnImpurity();
				}

				inline void setupNode(){
					pickMTRY();
					setHolderSizes();
					loadLabelHolder();
					nodeImpurity = calculateNodeImpurity(labelHolder);
				}


				//inline void setBestSplit(splitInfo<T> tempSplit, int featureNum){
				inline void setBestSplit(splitInfo<T> tempSplit){
					if(tempSplit.returnImpurity() < bestSplitInfo.returnImpurity()){
						bestSplitInfo = tempSplit;
				//		bestSplitInfo.setFeatureNum(featureNum);
					}
				}

				inline void loadIndices(inNodeClassIndices* indices){
					obsIndices = indices;
				}

				inline bool goLeft(const int& index){
					if(fpSingleton::getSingleton().returnFeatureVal(bestSplitInfo.returnFeatureNum(),index) < bestSplitInfo.returnSplitValue()){
						return true;
					}else{
						return false;
					}
				}


				inline void moveDataLeftOrRight(){
					leftIndices = new inNodeClassIndices();
					rightIndices = new inNodeClassIndices();

					int lNum =0;
					int rNum =0;
					for (int i : obsIndices->returnInSamples()){
						if(goLeft(i)){
							++lNum;
							leftIndices->addIndexToInSamples(i);	
						}else{
							++rNum;
							rightIndices->addIndexToInSamples(i);	
						}
					}

					for (int i : obsIndices->returnOutSamples()){
						if(goLeft(i)){
							leftIndices->addIndexToOutSamples(i);	
						}else{
							rightIndices->addIndexToOutSamples(i);	
						}
					}
					delete obsIndices;
					obsIndices = NULL;
				}


				inline void findBestSplit(){
					split<T> findSplit(labelHolder); //This is done twice
					while(!featuresToTry.empty()){
						loadFeatureHolder();
						setBestSplit(findSplit.giniSplit(featureHolder ,featuresToTry.back()));
						featuresToTry.pop_back();
					}
					if(bestSplitInfo.returnImpurity() <= 1){
					moveDataLeftOrRight();
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
