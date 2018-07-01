#ifndef rfTree_h
#define rfTree_h
#include "rfNode.h"
#include "../baseFunctions/fpUtils.h"
//#include "../baseFunctions/timeLogger.h"
#include <vector>
#include <random>
//#include <stdio.h>
#include "../treeConstructor/unprocessedNode.h"

namespace fp{

	template <typename T>
		class rfTree
		{
			protected:
				float OOBAccuracy;
				float correctOOB;
				float totalOOB;
				int totalIndices;
				std::vector< rfNode<T> > tree;
				std::vector< unprocessedNode<T> > nodeQueue;

			public:
				rfTree() : OOBAccuracy(-1.0),correctOOB(0),totalOOB(0), totalIndices(0){}

				void loadFirstNode(){
					nodeQueue.emplace_back(fpSingleton::getSingleton().returnNumObservations());
				}

				inline bool shouldProcessNode(){
					if(nodeQueue.back().returnNodeImpurity()==0){
						return false;
					}
					if(nodeQueue.back().returnInSampleSize() <= fpSingleton::getSingleton().returnMinParent()){
						return false;
					}
					/*
						 if(nodeQueue.back().returnDepth() > fpSingleton::getSingleton().returnMinParent()){
						 return false;
						 }
						 */
					return true;
				}

				inline float returnOOB(){
					return correctOOB/totalOOB;
				}

				inline int returnLastNodeID(){
					return tree.size()-1;
				}

				inline void linkParentToChild(){
					if(nodeQueue.back().returnIsLeftNode()){
						tree[nodeQueue.back().returnParentID()].setLeftValue(returnLastNodeID());
					}else{
						tree[nodeQueue.back().returnParentID()].setRightValue(returnLastNodeID());
					}

				}

				inline void setAsLeaf(){
					tree.back().setClass(nodeQueue.back().returnMaxClass());
				}

				inline void checkOOB(){
					totalOOB += nodeQueue.back().returnOutSampleSize();
					correctOOB += nodeQueue.back().returnOutSampleError(tree.back().returnClass());
				}

				inline void makeWholeNodeALeaf(){
					tree.emplace_back();
					linkParentToChild();
					setAsLeaf();
					checkOOB();
					//totalIndices += nodeQueue.back().returnIndTotal();
					nodeQueue.pop_back();
				}

				inline void makeNodeInternal(){
					tree.emplace_back();
					linkParentToChild();
					tree.back().setCutValue(nodeQueue.back().returnBestCutValue());
					tree.back().setFeatureValue(nodeQueue.back().returnBestFeature());
				}


				inline void createChildren(){
					bool isLeftNode = true;

					/*
						 inNodeClassIndices* leftIndices = nodeQueue.back().returnLeftIndices();
						 inNodeClassIndices* rightIndices = nodeQueue.back().returnRightIndices();
						 */
					stratifiedInNodeClassIndices* leftIndices = nodeQueue.back().returnLeftIndices();
					stratifiedInNodeClassIndices* rightIndices = nodeQueue.back().returnRightIndices();

					int childDepth = nodeQueue.back().returnDepth()+1;

					nodeQueue.pop_back();

					nodeQueue.emplace_back(returnLastNodeID(),childDepth, isLeftNode);
					nodeQueue.back().loadIndices(leftIndices);

					nodeQueue.emplace_back(returnLastNodeID(),childDepth, !isLeftNode);
					nodeQueue.back().loadIndices(rightIndices);
				}


				inline void processANode(){
					timeLogger logTime;
					logTime.startSortTimer();
					nodeQueue.back().setupNode();
					logTime.stopSortTimer();
					logTime.startGiniTimer();
					if(shouldProcessNode()){
						nodeQueue.back().findBestSplit();
						if(nodeQueue.back().returnBestImpurity() > 1){
							makeWholeNodeALeaf();
						}else{
							makeNodeInternal();
							createChildren();
						}
					}else{
						makeWholeNodeALeaf();
					}
					logTime.stopGiniTimer();
				}


				void processNodes(){
					while(!nodeQueue.empty()){
						processANode();
					}
				}


				void growTree(){
					loadFirstNode();
					processNodes();
				}


				int predictObservation(int observationNum){
					int currNode = 0;
					int featureNum = 0;
					T featureVal;
					while(tree[currNode].isInternalNode()){
						featureNum = tree[currNode].returnFeatureNumber();
						featureVal =fpSingleton::getSingleton().returnTestFeatureVal(featureNum,observationNum);
						currNode = tree[currNode].nextNode(featureVal);
					}

					return tree[currNode].returnClass();
				}
		};

}
#endif //rfTree_h
