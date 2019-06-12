#ifndef rfTree_h
#define rfTree_h
#include "../../../baseFunctions/fpBaseNode.h"
#include <vector>
#include <random>
#include "unprocessedNode.h"

namespace fp{

	template <typename T>
		class rfTree
		{
			protected:
				float totalOOB;
				std::vector<std::vector<int> > indexAndVote;
				std::vector< fpBaseNode<T, int> > tree;
				std::vector< unprocessedNode<T> > nodeQueue;

			public:
				rfTree() : totalOOB(0){}

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
					if (nodeQueue.back().returnDepth()>= fpSingleton::getSingleton().returnMaxDepth()){
						return false;
					}
					return true;
				}

				inline std::vector<std::vector<int> > returnOOBvotes(){
					return indexAndVote;
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


				inline int returnMaxDepth(){
					int maxDepth=0;
					for(auto& node : tree){
						if(maxDepth < node.returnDepth()){
							maxDepth = node.returnDepth();
						}
					}
					return maxDepth;
				}


				inline int returnNumLeafNodes(){
					int numLeafNodes=0;
					for(auto& node : tree){
						if(!node.isInternalNode()){
							++numLeafNodes;
						}
					}
					return numLeafNodes;
				}


				inline int returnLeafDepthSum(){
					int leafDepthSums=0;
					for(auto& node : tree){
						if(!node.isInternalNode()){
							leafDepthSums += node.returnDepth();
						}
					}
					return leafDepthSums;
				}


				inline void setAsLeaf(){
					tree.back().setClass(nodeQueue.back().returnMaxClass());
					tree.back().setDepth(nodeQueue.back().returnDepth());
				}


				inline void checkOOB(){
					totalOOB += nodeQueue.back().returnOutSampleSize();
					// Get oob_indices and vote from leaf node as
					// <(oob_id, vote)> and push back onto the
					// indexAndVote object.
					for(auto &j : nodeQueue.back().returnOutSampleIdsFromLeaf()){
						indexAndVote.push_back(std::vector<int>{j, tree.back().returnClass()});
					}
				}


				inline void makeWholeNodeALeaf(){
					tree.emplace_back();
					linkParentToChild();
					setAsLeaf();
					checkOOB();
					nodeQueue.back().deleteObsIndices();
					nodeQueue.pop_back();
				}

				void printTree(){
					for(auto& nd : tree){
						nd.printNode();
					}
				}

				inline void createNodeInTree(){
					tree.emplace_back();
					linkParentToChild();
					tree.back().setCutValue(nodeQueue.back().returnBestCutValue());
					tree.back().setFeatureValue(nodeQueue.back().returnBestFeature());
					tree.back().setDepth(nodeQueue.back().returnDepth());
				}


				inline void makeNodeInternal(){
					createNodeInTree();
					createChildren();
				}


				inline bool isLeftNode(){
					return true;
				}

				inline bool isRightNode(){
					return false;
				}


				inline void createChildren(){
					nodeQueue.back().moveDataLeftOrRight();

					stratifiedInNodeClassIndices* leftIndices = nodeQueue.back().returnLeftIndices();
					stratifiedInNodeClassIndices* rightIndices = nodeQueue.back().returnRightIndices();

					assert(leftIndices->returnInSampleSize() > 0);
					assert(rightIndices->returnInSampleSize() > 0);

					int childDepth = nodeQueue.back().returnDepth()+1;

					nodeQueue.pop_back();

					nodeQueue.emplace_back(returnLastNodeID(),childDepth, isLeftNode());
					nodeQueue.back().loadIndices(leftIndices);

					nodeQueue.emplace_back(returnLastNodeID(),childDepth, isRightNode());
					nodeQueue.back().loadIndices(rightIndices);
				}


				inline void findTheBestSplit(){
					nodeQueue.back().findBestSplit();
				}


				inline bool noGoodSplitFound(){
					return nodeQueue.back().returnBestFeature() == -1;
				}


				inline void processANode(){
					//	timeLogger logTime;
					//	logTime.startSortTimer();
					nodeQueue.back().setupNode();
					//	logTime.stopSortTimer();
					//	logTime.startGiniTimer();
					if(shouldProcessNode()){
						findTheBestSplit();
						if(noGoodSplitFound()){
							makeWholeNodeALeaf();
						}else{
							makeNodeInternal();
						}
					}else{
						makeWholeNodeALeaf();
					}
					//	logTime.stopGiniTimer();
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


				//Note: this is 
				inline int predictObservation(int observationNum){
					int currNode = 0;
					int featureNum = 0;
					T featureVal;
					while(tree[currNode].isInternalNode()){
						featureNum = tree[currNode].returnFeatureNumber();
						featureVal = fpSingleton::getSingleton().returnTestFeatureVal(featureNum,observationNum);
						currNode = tree[currNode].fpBaseNode<T, int>::nextNode(featureVal);
					}
					return tree[currNode].returnClass();
				}


				inline int predictObservation(std::vector<T>& observation){
					int currNode = 0;
					while(tree[currNode].isInternalNode()){
						currNode = tree[currNode].nextNode(observation);
					}
					return tree[currNode].returnClass();
				}


				std::vector<int> growTreeTest(){
					//JLP testing.
					std::vector<int> outSampleIndices;
					loadFirstNode();
					// grab outSamplesIndices before they are deleted.
					outSampleIndices = nodeQueue.back().returnOutSampleIdsFromLeaf();
					processNodes();
					checkOOB();
					return outSampleIndices;
				}

		};

}//fp
#endif //rfTree_h
