#ifndef rfTree_h
#define rfTree_h
#include "rfNode.h"
#include "../baseFunctions/fpUtils.h"
#include <vector>
#include <random>
#include "../treeConstructor/unprocessedNode.h"

namespace fp{

	template <typename T>
		class rfTree
		{
			protected:
				float OOBAccuracy;
				float correctOOB;
				float totalOOB;
				std::vector< rfNode<T> > tree;
				std::vector< unprocessedNode<T> > nodeQueue;

			public:
				rfTree() : OOBAccuracy(-1.0),correctOOB(0),totalOOB(0){}

				void loadFirstNode(){
					nodeQueue.emplace_back(fpSingleton::getSingleton().returnNumObservations());
				}

				inline bool shouldProcessNode(){
					if(!nodeQueue.back().returnNodeImpurity()){
						return false;
					}
					if(nodeQueue.back().returnInSampleSize() < fpSingleton::getSingleton().returnMinParent()){
						return false;
					}
					if(nodeQueue.back().returnDepth() > fpSingleton::getSingleton().returnMinParent()){
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
					nodeQueue.pop_back();
				}

				inline void makeNodeInternal(){
tree.emplace_back();
					linkParentToChild();
					tree.back().setCutValue(nodeQueue.back().returnBestCutValue());
					tree.back().setFeatureValue(nodeQueue.back().returnBestFeature());
				}

				inline void createChildren(){
inNodeClassIndices* leftIndices = nodeQueue.back().returnLeftIndices();
				inNodeClassIndices* rightIndices = nodeQueue.back().returnRightIndices();

					nodeQueue.pop_back();
					nodeQueue.emplace_back(returnLastNodeID(),nodeQueue.back().returnDepth()+1, true);
					nodeQueue.back().loadIndices(leftIndices);

					nodeQueue.emplace_back(returnLastNodeID(),nodeQueue.back().returnDepth()+1, false);
					nodeQueue.back().loadIndices(rightIndices);
				}


			inline void processANode(){
					nodeQueue.back().setupNode();
					if(shouldProcessNode()){
					nodeQueue.back().findBestSplit();
						makeNodeInternal();
						createChildren();
					}else{
						makeWholeNodeALeaf();
					}

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
		};

}
#endif //rfTree_h
