#ifndef treeStruct_h
#define treeStruct_h
#include "../../baseFunctions/fpBaseNode.h"
#include "obsIndexAndClassVec.h"
#include "zipClassAndValue.h"
#include "processingNode.h"
#include <vector>

namespace fp{

	template <typename T, typename Q>
		class treeStruct
		{
			protected:
				float OOBAccuracy;
				float correctOOB;
				float totalOOB;
				std::vector< fpBaseNode<T,Q> > tree;
				std::vector<processingNode<T,Q> > nodeQueue;

				int numberOfNodes;


				//obsIndexAndClassVec indexHolder(numClasses);
				//std::vector<zipClassAndValue<int, float> > zipVec(testSize);

				inline bool rightNode(){
					return false;
				}

				inline bool leftNode(){
					return true;
				}
			public:
				treeStruct(obsIndexAndClassVec& indices, std::vector<zipClassAndValue<int, T> >& zip) : OOBAccuracy(-1.0),correctOOB(0),totalOOB(0),numberOfNodes(0){
					loadFirstNode(indices, zip);	
				}


				inline void loadFirstNode(obsIndexAndClassVec& indicesHolder, std::vector<zipClassAndValue<int, T> >& zipper){
					nodeQueue.emplace_back(0,0);
					nodeQueue.back().setupRoot(indicesHolder, zipper);
					nodeQueue.back().processNode();
					setInitialNumberOfNodes();
					makeLeafNodes();
					if(nodeQueue.back().isLeafNode()){
						makeRootALeaf();
					}else{
						copyProcessedNodeToTree();
						createChildNodes();
					}
				}

				inline void makeRootALeaf(){
					tree.emplace_back(nodeQueue.back().returnNodeClass());
					nodeQueue.pop_back();
				}

				inline bool shouldProcessNode(){
					return !nodeQueue.back().isLeafNode();
				}


				inline int positionOfNextNode(){
					return (int)tree.size()-1;
				}


				inline int parentNodesPosition(){
					return (int)tree.size()-1;
				}


				inline void makeLeafNodes(){
					for(int i= 0; i < fpSingleton::getSingleton().returnNumClasses(); ++i){
						tree.emplace_back(i);
					}
				}


				inline void setInitialNumberOfNodes(){
					numberOfNodes = fpSingleton::getSingleton().returnNumClasses();
				}


				inline int returnDepthOfNode(){
					return tree[nodeQueue.back().returnParentNodeNumber()].returnDepth()+1;
				}


				inline void copyProcessedNodeToTree(){
					tree.emplace_back(nodeQueue.back().returnNodeCutValue(), returnDepthOfNode(), nodeQueue.back().returnNodeCutFeature());
				}


				inline void linkParentToChild(){
					if(nodeQueue.back().returnIsLeftNode()){
						tree[nodeQueue.back().returnParentNodeNumber()].setLeftValue(positionOfNextNode());
					}else{
						tree[nodeQueue.back().returnParentNodeNumber()].setRightValue(positionOfNextNode());
					}
				}


				inline void linkParentToLeaf(){
					assert(nodeQueue.back().returnParentNodeNumber() >= fpSingleton::getSingleton().returnNumClasses());
					assert(nodeQueue.back().returnParentNodeNumber() <= parentNodesPosition());

					assert(nodeQueue.back().returnNodeClass() >= 0);
					assert(nodeQueue.back().returnNodeClass() < fpSingleton::getSingleton().returnNumClasses());

					if(nodeQueue.back().returnIsLeftNode()){
						tree[nodeQueue.back().returnParentNodeNumber()].setLeftValue(nodeQueue.back().returnNodeClass());
					}else{
						tree[nodeQueue.back().returnParentNodeNumber()].setRightValue(nodeQueue.back().returnNodeClass());
					}
				}


				inline void createChildNodes(){
					nodeIterators nodeIts(nodeQueue.back().returnNodeIterators());
					zipperIterators<int,T> zipIts(nodeQueue.back().returnZipIterators());
					if(nodeQueue.back().isLeftChildLarger()){
						nodeQueue.pop_back();
						//TODO: don't emplace_back if should be leaf node.
						nodeQueue.emplace_back(1,parentNodesPosition());
						nodeQueue.back().setupNode(nodeIts, zipIts, rightNode());
						nodeQueue.emplace_back(1,parentNodesPosition());
						nodeQueue.back().setupNode(nodeIts, zipIts, leftNode());
					}else{
						nodeQueue.pop_back();
						nodeQueue.emplace_back(1,parentNodesPosition());
						nodeQueue.back().setupNode(nodeIts, zipIts, leftNode());
						nodeQueue.emplace_back(1,parentNodesPosition());
						nodeQueue.back().setupNode(nodeIts, zipIts, rightNode());
					}
				}


				inline void processLeafNode(){
					assert(nodeQueue.back().returnNodeSize() > 0);
					assert(nodeQueue.back().returnNodeSize() <= fpSingleton::getSingleton().returnNumObservations());
					linkParentToLeaf();
					nodeQueue.pop_back();
				}


				inline void processInternalNode(){
					copyProcessedNodeToTree();
					linkParentToChild();
					createChildNodes();
				}


				inline void processNode(){
					nodeQueue.back().processNode();
					if(nodeQueue.back().isLeafNode()){
						processLeafNode();						
					}else{
						processInternalNode();
					}
				}

				inline void createTree(){
					while(!nodeQueue.empty()){
						processNode();
					}
				}


				inline int returnMaxDepth(){
					int maxDepth=0;
					for(auto nodes : tree){
						if(maxDepth < nodes.returnDepth()){
							maxDepth = nodes.returnDepth();
						}
					}
					return maxDepth+1;
				}


				inline int returnNumLeafNodes(){
					return (int)tree.size() - fpSingleton::getSingleton().returnNumClasses() + 1;
				}


				inline int returnLeafDepthSum(){
					int leafDepthSums=0;
					for(auto nodes : tree){
						if(nodes.isInternalNodeFront()){
							if(nodes.returnLeftNodeID() < fpSingleton::getSingleton().returnNumClasses()){
								leafDepthSums += nodes.returnDepth()+1;
							}
							if(nodes.returnRightNodeID() < fpSingleton::getSingleton().returnNumClasses()){
								leafDepthSums += nodes.returnDepth()+1;
							}
						}
					}
					return leafDepthSums;
				}

				template<typename U>
					struct identity { typedef U type; };

				inline int predictObservation(int observationNum){
return predictObservation(observationNum, identity<Q>());
				}

				inline int predictObservation(int observationNum, identity<int> ){
					int currNode = fpSingleton::getSingleton().returnNumClasses();
					int featureNum = 0;
					T featureVal;
					while(tree[currNode].isInternalNodeFront()){
						featureNum = tree[currNode].returnFeatureNumber();
						featureVal = fpSingleton::getSingleton().returnTestFeatureVal(featureNum,observationNum);
						currNode = tree[currNode].fpBaseNode<T, Q>::nextNode(featureVal);
					}
					return tree[currNode].returnClass();
				}

				
inline int predictObservation(int observationNum, identity<std::vector<int> >){
					int currNode = fpSingleton::getSingleton().returnNumClasses();
					T aggregator;
					while(tree[currNode].isInternalNodeFront()){
						aggregator = 0;
						for(auto i : tree[currNode].returnFeatureNumber()){
aggregator += fpSingleton::getSingleton().returnTestFeatureVal(i,observationNum);
						}
						currNode = tree[currNode].fpBaseNode<T, Q>::nextNode(aggregator);
					}
					return tree[currNode].returnClass();
				}

/*
inline int predictObservation(std::vector<T>& observation){
return predictObservation(observation, identity<Q>());
				}
				*/

				//inline int predictObservation(std::vector<T>& observation, identity<int>){
				inline int predictObservation(std::vector<T>& observation){
					int currNode = fpSingleton::getSingleton().returnNumClasses();
					while(tree[currNode].isInternalNode()){
						currNode = tree[currNode].nextNode(observation);
					}
					return tree[currNode].returnClass();
				}

				/*
				inline int predictObservation(std::vector<T>& observation, identity<std::vector<int> >){
int currNode = fpSingleton::getSingleton().returnNumClasses();
					while(tree[currNode].isInternalNode()){
						currNode = tree[currNode].nextNode(observation);
					}
					return tree[currNode].returnClass();
				}
				*/

				///////////////////////////////////
				/// Test Functions not to be used in production
				//////////////////////////////////


				inline std::vector< fpBaseNode<T,Q> >& exposeTreeTest(){
					return tree;
				}

				inline std::vector<processingNode<T,Q> >& exposeNodeQueueTest(){
					return nodeQueue;
				}

				void printTree(){
					std::cout << "\n";
					for(auto nd : tree){
						nd.printNode();
					}
				}
				/*
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
					T featureVal = 0;
					while(tree[currNode].isInternalNode()){
						featureVal = 0;
						for(auto featureNumber : tree[currNode].returnFeatureNumber()){
							featureVal += fpSingleton::getSingleton().returnTestFeatureVal(featureNumber,observationNum);
						}
						currNode = tree[currNode].fpBaseNode<T, std::vector<int> >::nextNode(featureVal);
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
				*/
		};

}//fp
#endif //treeStruct_h
