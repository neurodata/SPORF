#ifndef urerfTree_h
#define urerfTree_h
#include "../../../baseFunctions/fpBaseNode.h"
#include "unprocessedURerFNode.h"
#include <vector>
#include <map>
#include <assert.h>
#include <limits>

namespace fp{

	template <typename T>
		class urerfTree
		{
			protected:
				float OOBAccuracy;
				float totalOOB;
				std::vector<std::vector<int> > indexAndVote;
				std::vector< fpBaseNode<T, std::vector<int> > > tree;
				std::vector< unprocessedURerFNode<T> > nodeQueue;
				std::vector< unprocessedURerFNode<T> > leafNodes;

			public:
				urerfTree() : totalOOB(0){}

				void loadFirstNode(){
					nodeQueue.emplace_back(fpSingleton::getSingleton().returnNumObservations());
				}

				inline bool shouldProcessNode(){
                                        if(nodeQueue.back().returnNodeImpurity() < std::numeric_limits<T>::epsilon())
                                                return false;
                                        if(nodeQueue.back().returnInSampleSize() <= fpSingleton::getSingleton().returnMinParent())
                                                return false;
                                        if(nodeQueue.back().returnDepth() >= fpSingleton::getSingleton().returnMaxDepth())
                                                return false;
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



 				inline void updateSimMat(std::map<int, std::map<int, int> > &simMat, std::map<std::pair<int, int>, double> &pairMat){
				for(auto nodes : leafNodes){
					stratifiedInNodeClassIndicesUnsupervised* obsI = nodes.returnObsIndices();
					std::vector<int> leafObs;
					std::vector<int> leafObsOut;
					leafObs = obsI->returnInSampsVec();
					leafObsOut = obsI->returnOutSampsVec();
					auto siz = leafObs.size();
					if (siz <= 0)
						continue;
					for(unsigned int i = 0; i < siz; ++i) {
						for (unsigned int j=0; j<=i; ++j) {
 							std::pair<int, int> pair1 = std::make_pair(leafObs[i], leafObs[j]);

							if(pairMat.count(pair1) > 0){
								#pragma omp critical
                                                               	{
									pairMat[pair1]++;
								}
							}
							else{
								#pragma omp critical
								{
									pairMat.insert({pair1, 1});
 								}
 							}
 						}
					}
				}
			}

			inline void updateSimMatOut(std::map<int, std::map<int, int> > &simMat, std::map<std::pair<int, int>, double> &pairMat){
 				for(auto nodes : leafNodes){
					stratifiedInNodeClassIndicesUnsupervised* obsI = nodes.returnObsIndices();
					std::vector<int> leafObs;
 					leafObs = obsI->returnOutSampsVec();
					auto siz = leafObs.size();
					if (siz <= 0)
						continue;

					for(unsigned int i = 0; i < siz; ++i) {
						for (unsigned int j=0; j<=i; ++j) {
							std::pair<int, int> pair1 = std::make_pair(leafObs[i], leafObs[j]);
                                        		#pragma omp critical
							{
								auto it = pairMat.find(pair1);
								if(it!=pairMat.end())
									pairMat[pair1]++;
								else
									pairMat.insert({pair1, 1});
							}
						}
					}
				}
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
					tree.back().setDepth(nodeQueue.back().returnDepth());
				}

				inline void makeWholeNodeALeaf(){
					tree.emplace_back();
					linkParentToChild();
					setAsLeaf();
					leafNodes.emplace_back(nodeQueue.back());
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

				inline bool isLeftNode(){
					return true;
				}

				inline bool isRightNode(){
					return false;
				}

				inline void createChildren(){
					nodeQueue.back().moveDataLeftOrRight();

					stratifiedInNodeClassIndicesUnsupervised* leftIndices = nodeQueue.back().returnLeftIndices();
					stratifiedInNodeClassIndicesUnsupervised* rightIndices = nodeQueue.back().returnRightIndices();

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
				        if(nodeQueue.back().returnBestImpurity() == -1)
                                                return true;
			
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

				inline void processNodes(){
					while(!nodeQueue.empty()){
						processANode();
					}
				}

				inline void growTree(){
					loadFirstNode();
					processNodes();
				}

		};

}//fp
#endif //urerfTree_h
