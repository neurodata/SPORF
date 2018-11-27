#ifndef rerfTree_h
#define rerfTree_h
#include "rerfNode.h"
#include <vector>
#include <random>
#include "unprocessedRerFNode.h"

namespace fp{

	template <typename T>
		class rerfTree
		{
			protected:
				float OOBAccuracy;
				float correctOOB;
				float totalOOB;
				std::vector< rerfNode<T> > tree;
				std::vector< unprocessedRerFNode<T> > nodeQueue;

			public:
				rerfTree() : OOBAccuracy(-1.0),correctOOB(0),totalOOB(0){}

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


				inline int returnMaxDepth(){
					int maxDepth=0;
					for(auto nodes : tree){
						if(maxDepth < nodes.returnDepth()){
							maxDepth = nodes.returnDepth();
						}
					}
					return maxDepth;
				}


				inline int returnNumLeafNodes(){
					int numLeafNodes=0;
					for(auto nodes : tree){
						if(!nodes.isInternalNode()){
							++numLeafNodes;
						}
					}
					return numLeafNodes;
				}


				inline int returnLeafDepthSum(){
					int leafDepthSums=0;
					for(auto nodes : tree){
						if(!nodes.isInternalNode()){
							leafDepthSums += nodes.returnDepth();
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
					correctOOB += nodeQueue.back().returnOutSampleError(tree.back().returnClass());
				}


				inline void makeWholeNodeALeaf(){
					tree.emplace_back();
					linkParentToChild();
					setAsLeaf();
					checkOOB();
					nodeQueue.pop_back();
				}

				void printTree(){
					for(auto nd : tree){
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


				inline void createChildren(){
					bool isLeftNode = true;

					nodeQueue.back().moveDataLeftOrRight();

					stratifiedInNodeClassIndices* leftIndices = nodeQueue.back().returnLeftIndices();
					stratifiedInNodeClassIndices* rightIndices = nodeQueue.back().returnRightIndices();

					int childDepth = nodeQueue.back().returnDepth()+1;

					nodeQueue.pop_back();

					nodeQueue.emplace_back(returnLastNodeID(),childDepth, isLeftNode);
					nodeQueue.back().loadIndices(leftIndices);

					nodeQueue.emplace_back(returnLastNodeID(),childDepth, !isLeftNode);
					nodeQueue.back().loadIndices(rightIndices);
				}


				inline void findTheBestSplit(){
					nodeQueue.back().findBestSplit();
				}


				inline bool noGoodSplitFound(){
					return nodeQueue.back().returnBestFeature().empty();
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


				inline int predictObservation(int observationNum){
					int currNode = 0;
					//std::vector<int> featureNum;
					int featureNum = 0;
					T featureVal;
					while(tree[currNode].isInternalNode()){
						featureNum = tree[currNode].returnFeatureNumber()[0];
						featureVal = fpSingleton::getSingleton().returnTestFeatureVal(featureNum,observationNum);
						//featureVal = fpSingleton::getSingleton().returnTestFeatureVal(featureNum[0],observationNum);
						if(tree[currNode].returnFeatureNumber().size()>1){
							for(unsigned int j =1; j < tree[currNode].returnFeatureNumber().size(); ++j){
						featureNum = tree[currNode].returnFeatureNumber()[j];
								featureVal += fpSingleton::getSingleton().returnTestFeatureVal(featureNum,observationNum);
							}
						}
						
						currNode = tree[currNode].nextNode(featureVal);
					}
					return tree[currNode].returnClass();
				}
		};

}
#endif //rfTree_h
