#ifndef rfunprocessedNode_h
#define rfunprocessedNode_h
//#include "rfNode.h"
//#include "../baseFunctions/fpUtils.h"
//#include <stdio.h>
#include <vector>
#include <random>

namespace fp{

	template <typename T> //
		class unprocessedNode{
			protected:
				int parentID;
				float nodeImpurity; //lower impurity is better
				bool isLeftNode; //in order to set parent node with location

				splitInfo<T> bestSplitInfo;

				//split* nodeSplit;

				inNodeClassIndices obsIndices;

				std::vector<int> featuresToTry;
				std::vector<T> featureHolder;
				std::vector<int> labelHolder;
			public:
				unprocessedNode(const int numObsForRoot): obsIndices(numObsForRoot), parentID(0), isLeftNode(true){}

				unprocessedNode(int parentID, bool isLeft):parentID(parentID),isLeftNode(isLeft){}

				inline int returnParentID(){
return parentID;
				}

inline bool returnIsLeftNode(){
return isLeftNode;
}
				inline void setHolderSizes(){
					labelHolder.resize(obsIndices.returnInSampleSize());
					featureHolder.resize(obsIndices.returnInSampleSize());
				}

				inline void loadLabelHolder(fpData& dat){
					for(unsigned int i =0; i < inB.size(); ++i){
						labelHolder[i] = dat.returnLabel(i);
					}
				}

				inline void loadFeatureHolder(fpData& dat){
					for(unsigned int i =0; i < inB.size(); ++i){
						featureHolder[i] = dat.returnFeatureValue(featuresToTry.back(),i);
					}
				}

				inline void pickMTRY(int numFeatures, int mtry){
					for (int i=0; i<numFeatures; ++i) featuresToTry.push_back(i);
					std::random_shuffle ( featuresToTry.begin(), featuresToTry.end() );
					featuresToTry.resize(mtry);
				}

				inline void setupHolders(fpData& dat){
					setHolderSizes();
					loadLabelHolder(dat);
				}

				/*
				inline float returnNodeImpurity(std::vector<int> labels){
					split	nodeSplit(labels);
					return nodeSplit->returnImpurity();
				}
				*/

				inline void setupNode(fpInfo& info, fpData& dat){
					pickMTRY(dat.returnNumFeatures(), info.returnMTRY());
					setupHolders(dat);
					nodeImpurity = returnNodeImpurity(labelHolder);
				}

				inline bool isLeafNode(){
					return nodeImpurity == 0;
				}


				inline void setBestSplit(spiltInfo<T> tempSplit){
					if(tempSplit.returnImpurity() < bestSplitInfo.returnImpurity()){
						bestSplitInfo = tempSplit;
					}
				}


				splitInfo<T> findBestSplit(fpData& dat){
					spiltInfo<T> tempSplitInfo;
					split findSplit(labelHolder);
					while(!featuresToTry.empty()){
						loadFeatureHolder(dat);
						tempSplitInfo = findSplit.giniSplit(featureHolder ,featuresToTry.back());
						featuresToTry.pop_back();
						setBestSplit(tempSplitInfo);
					}
					return bestSplitInfo;
				}


				inline int returnMaxClass(){
					if(nodeImpurity==1){
						return labelHolder[0];
					} else {
classTotals findMaxClass;
findMaxClass.findNumClasses(labelHolder);
return findMaxClass.returnLargestClass();
					}
				}

		};


	template <typename T>
		class rfTree
		{
			protected:
				float OOBAccuracy;
				int currNodeIndex;
				std::vector< rfNode<T> > tree;
				std::vector< unprocessedNode<T> > nodeQueue;
				//	OOB* sampleIndices;

			public:
				rfTree() : OOBAccuracy(-1.0), currNodeIndex(-1){}

				void setInitialNode(fpData& dat){
					nodeQueue.push_back(unprocessedNode(dat.returnNumObservations()));
				}

				void setNode(unprocessedNode<T> temp){
 tree.back().setCutValue(temp.returnCutValue()); 
 tree.back().setFeature(temp.returnFeature()); 
				}

				void setLeafNode(){
nodeQueue.back()
tree.back().setClass(maxClass);
				}

				void buildTree(fpInfo& info, fpData& dat){
					setInitialNode(dat);
					//	tree.push_back();
splitInfo<T> tempSplitInfo;
						while(!nodeQueue.empty()){
							nodeQueue.back().setupNode(info,dat);
++currNodeIndex;
tree.push_back();
if(nodeQueue.back().returnIsLeftNode()){
tree[nodeQueue.back().returnParentID()].setLeftValue(currNodeIndex);
}else{
tree[nodeQueue.back().returnParentID()].setRightValue(currNodeIndex);
}
							//pop last node off
							if(nodeQueue.back().isLeafNode()){
setLeafNode();
nodeQueue.pop_back();
							}else{
tempSplitInfo = nodeQueue.back().findBestSplit(fpData& dat);
unprocessedNode<T> temp = nodeQueue.back();
nodeQueue.pop_back();
setSplitNode(temp);

nodeQueue.push_back(

							}

						}
					}


				};

		}
#endif //unprocessedNode_h
