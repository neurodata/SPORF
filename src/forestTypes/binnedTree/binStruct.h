#ifndef binStruct_h
#define binStruct_h
#include "../../baseFunctions/fpBaseNode.h"
#include "obsIndexAndClassVec.h"
#include "zipClassAndValue.h"
#include "processingNode.h"
#include <vector>

namespace fp{

	template <typename T, typename Q>
		class binStruct
		{
			protected:
				float OOBAccuracy;
				float correctOOB;
				float totalOOB;
				std::vector< fpBaseNode<T,Q> > bin;
				std::vector<processingNode<T,Q> > nodeQueue;

				int numberOfNodes;

				int numOfTreesInBin;
				int currTree;

				obsIndexAndClassVec& indicesHolder;
				std::vector<zipClassAndValue<int, T> >& zipper;
				std::vector<int>& nodeIndices;
				//obsIndexAndClassVec indexHolder(numClasses);
				//std::vector<zipClassAndValue<int, float> > zipVec(testSize);

				inline bool rightNode(){
					return false;
				}

				inline bool leftNode(){
					return true;
				}
			public:
				binStruct(obsIndexAndClassVec& indices, std::vector<zipClassAndValue<int, T> >& zip, std::vector<int>& indicesChooser, int numTrees) : OOBAccuracy(-1.0),correctOOB(0),totalOOB(0),numberOfNodes(0),numOfTreesInBin(numTrees),currTree(0),indicesHolder(indices),zipper(zip),nodeIndices(indicesChooser){

					bin.resize(numOfTreesInBin+fpSingleton::getSingleton().returnNumClasses());
					makeLeafNodes();
				}


				inline void loadFirstNode(){
					//inline void loadFirstNode(obsIndexAndClassVec& indicesHolder, std::vector<zipClassAndValue<int, T> >& zipper){
					nodeQueue.emplace_back(0,0);
					nodeQueue.back().setupRoot(indicesHolder, zipper);
					nodeQueue.back().processNode();
					if(nodeQueue.back().isLeafNode()){
						makeRootALeaf();
					}else{
						copyProcessedRootToBin();
						createRootChildNodes();
					}
				}

				inline void makeRootALeaf(){
          bin[returnRootLocation()].setClass(nodeQueue.back().returnNodeClass());
					bin[returnRootLocation()].setDepth(-1);
				}

				inline void setSharedVectors(obsIndexAndClassVec& indicesInNode){
					indicesInNode.resetVectors();

					int numUnusedObs = fpSingleton::getSingleton().returnNumObservations();
					int randomObsID;
					int tempMoveObs;

					for(int n = 0; n < fpSingleton::getSingleton().returnNumObservations(); n++){
						randomObsID = fpSingleton::getSingleton().genRandom(fpSingleton::getSingleton().returnNumObservations());

						indicesInNode.insertIndex(nodeIndices[randomObsID], fpSingleton::getSingleton().returnLabel(nodeIndices[randomObsID]));

						if(randomObsID < numUnusedObs){
							--numUnusedObs;
							tempMoveObs = nodeIndices[numUnusedObs];
							nodeIndices[numUnusedObs] = nodeIndices[randomObsID];
							nodeIndices[randomObsID] = tempMoveObs;
						}
					}

				}


				inline bool shouldProcessNode(){
					return !nodeQueue.back().isLeafNode();
				}


				inline int positionOfNextNode(){
					return (int)bin.size()-1;
				}


				inline int parentNodesPosition(){
					return (int)bin.size()-1;
				}


				inline void makeLeafNodes(){
					for(int i= 0; i < fpSingleton::getSingleton().returnNumClasses(); ++i){
						bin[i].setSharedClass(i);
					}
				}


				inline int returnDepthOfNode(){
					return bin[nodeQueue.back().returnParentNodeNumber()].returnDepth()+1;
				}


				inline void copyProcessedNodeToBin(){
					bin.emplace_back(nodeQueue.back().returnNodeCutValue(), returnDepthOfNode(), nodeQueue.back().returnNodeCutFeature());
				}


				inline void copyProcessedRootToBin(){
					bin[returnRootLocation()].setCutValue(nodeQueue.back().returnNodeCutValue());
					bin[returnRootLocation()].setDepth(0);
					bin[returnRootLocation()].setFeatureValue(nodeQueue.back().returnNodeCutFeature());
				}

				inline int returnRootLocation(){
					return currTree+fpSingleton::getSingleton().returnNumClasses();
				}


				inline void linkParentToChild(){
					if(nodeQueue.back().returnIsLeftNode()){
						bin[nodeQueue.back().returnParentNodeNumber()].setLeftValue(positionOfNextNode());
					}else{
						bin[nodeQueue.back().returnParentNodeNumber()].setRightValue(positionOfNextNode());
					}
				}


				inline void linkParentToLeaf(){
					assert(nodeQueue.back().returnParentNodeNumber() >= fpSingleton::getSingleton().returnNumClasses());
					assert(nodeQueue.back().returnParentNodeNumber() <= parentNodesPosition());

					assert(nodeQueue.back().returnNodeClass() >= 0);
					assert(nodeQueue.back().returnNodeClass() < fpSingleton::getSingleton().returnNumClasses());

					if(nodeQueue.back().returnIsLeftNode()){
						bin[nodeQueue.back().returnParentNodeNumber()].setLeftValue(nodeQueue.back().returnNodeClass());
					}else{
						bin[nodeQueue.back().returnParentNodeNumber()].setRightValue(nodeQueue.back().returnNodeClass());
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


				inline void createRootChildNodes(){
					nodeIterators nodeIts(nodeQueue.back().returnNodeIterators());
					zipperIterators<int,T> zipIts(nodeQueue.back().returnZipIterators());
					if(nodeQueue.back().isLeftChildLarger()){
						nodeQueue.pop_back();
						//TODO: don't emplace_back if should be leaf node.
						nodeQueue.emplace_back(1,returnRootLocation());
						nodeQueue.back().setupNode(nodeIts, zipIts, rightNode());
						nodeQueue.emplace_back(1,returnRootLocation());
						nodeQueue.back().setupNode(nodeIts, zipIts, leftNode());
					}else{
						nodeQueue.pop_back();
						nodeQueue.emplace_back(1,returnRootLocation());
						nodeQueue.back().setupNode(nodeIts, zipIts, leftNode());
						nodeQueue.emplace_back(1,returnRootLocation());
						nodeQueue.back().setupNode(nodeIts, zipIts, rightNode());
					}
				}

				inline void processLeafNode(){
					assert(nodeQueue.back().returnNodeSize() > 0);
					assert(nodeQueue.back().returnNodeSize() <= fpSingleton::getSingleton().returnNumObservations());
					linkParentToLeaf();
					nodeQueue.pop_back();
				}


				inline int returnNumTrees(){
					return numOfTreesInBin;
				}


				inline void processInternalNode(){
					copyProcessedNodeToBin();
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


				inline void createBin(){
					for(; currTree < numOfTreesInBin; ++currTree){
						setSharedVectors(indicesHolder);
						//loadFirstNode(indicesHolder, zipper);	
						loadFirstNode();	
						while(!nodeQueue.empty()){
							processNode();
						}
					}
				}


				inline int returnMaxDepth(){
					int maxDepth=0;
					for(auto nodes : bin){
						if(maxDepth < nodes.returnDepth()){
							maxDepth = nodes.returnDepth();
						}
					}
					return maxDepth+1;
				}


				inline int returnNumLeafNodes(){
					return (int)bin.size() - fpSingleton::getSingleton().returnNumClasses() + 1;
				}


				inline int returnLeafDepthSum(){
					int leafDepthSums=0;
					for(auto nodes : bin){
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

				/////////////////////////
				// This is required to template the predictObservation function
				// //////////////////////////////
				template<typename U>
					struct identity { typedef U type; };

				inline void predictBinObservation(int observationNum, std::vector<int>& preds){
					predictBinObservation(observationNum,preds, identity<Q>());
				}
				////////////////////////////////

				//PredictForRF
				inline void predictBinObservation(int observationNum,std::vector<int>& preds, identity<int> ){
					std::vector<int> currNode(numOfTreesInBin);
					int numberNotInLeaf;
					int featureNum;
					T featureVal;
					int q;

					for( q=0; q<numOfTreesInBin; ++q){
						currNode[q] = q+fpSingleton::getSingleton().returnNumClasses();
						__builtin_prefetch(&bin[currNode[q]], 0, 3);
					}

					do{
						numberNotInLeaf = 0;

						for( q=0; q<numOfTreesInBin; ++q){

							if(bin[currNode[q]].isInternalNodeFront()){
								featureNum = bin[currNode[q]].returnFeatureNumber();
								featureVal = fpSingleton::getSingleton().returnTestFeatureVal(featureNum,observationNum);
								currNode[q] = bin[currNode[q]].fpBaseNode<T, Q>::nextNode(featureVal);
								__builtin_prefetch(&bin[currNode[q]], 0, 3);
								++numberNotInLeaf;
							}
						}

					}while(numberNotInLeaf);

					for( q=0; q<numOfTreesInBin; q++){
#pragma omp atomic update
						++preds[bin[currNode[q]].returnClass()];
					}
				}



				inline void predictBinObservation(int observationNum, std::vector<int>& preds, identity<std::vector<int> >){
std::vector<int> currNode(numOfTreesInBin);
					int numberNotInLeaf;
					T featureVal;
					int  q;


					for( q=0; q<numOfTreesInBin; ++q){
						currNode[q] = q+fpSingleton::getSingleton().returnNumClasses();
						__builtin_prefetch(&bin[currNode[q]], 0, 3);
					}

					do{
						numberNotInLeaf = 0;

						for( q=0; q<numOfTreesInBin; ++q){

							if(bin[currNode[q]].isInternalNodeFront()){
featureVal = 0;
for(auto i : bin[currNode[q]].returnFeatureNumber()){
							featureVal += fpSingleton::getSingleton().returnTestFeatureVal(i,observationNum);
						}
								currNode[q] = bin[currNode[q]].fpBaseNode<T, Q>::nextNode(featureVal);
								__builtin_prefetch(&bin[currNode[q]], 0, 3);
								++numberNotInLeaf;
							}
						}

					}while(numberNotInLeaf);

					for( q=0; q<numOfTreesInBin; q++){
#pragma omp atomic update
						++preds[bin[currNode[q]].returnClass()];
					}
				}


				//inline int predictObservation(std::vector<T>& observation, identity<int>){
				inline void predictBinObservation(std::vector<T>& observation, std::vector<int> preds){
					/*
					int currNode = fpSingleton::getSingleton().returnNumClasses();
					while(bin[currNode].isInternalNode()){
						currNode = bin[currNode].nextNode(observation);
					}
					return bin[currNode].returnClass();
					*/
				}



				///////////////////////////////////
				/// Test Functions not to be used in production
				//////////////////////////////////


				inline std::vector< fpBaseNode<T,Q> >& exposeBinTest(){
					return bin;
				}

				inline std::vector<processingNode<T,Q> >& exposeNodeQueueTest(){
					return nodeQueue;
				}

				void printBin(){
					std::cout << "\n";
					for(auto nd : bin){
						nd.printNode();
					}
				}

				};

				}//fp
#endif //binStruct_h
