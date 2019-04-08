#ifndef binStruct_h
#define binStruct_h
#include "../../baseFunctions/fpBaseNode.h"
#include "../../baseFunctions/MWC.h"
#include "obsIndexAndClassVec.h"
#include "zipClassAndValue.h"
#include "processingNodeBin.h"
#include <vector>
#include <assert.h>

namespace fp{

	template <typename T, typename Q>
		class binStruct
		{
			protected:
				float OOBAccuracy;
				float correctOOB;
				float totalOOB;
				std::vector< fpBaseNode<T,Q> > bin;
				std::vector<processingNodeBin<T,Q> > nodeQueue;

				int numberOfNodes;

				int numOfTreesInBin;
				int currTree;

				obsIndexAndClassVec indicesHolder;
				std::vector<zipClassAndValue<int, T> > zipper;

				std::vector<int> nodeIndices;


				randomNumberRerFMWC randNum;

				//obsIndexAndClassVec indexHolder(numClasses);
				//std::vector<zipClassAndValue<int, float> > zipVec(testSize);

				inline bool rightNode(){
					return false;
				}

				inline bool leftNode(){
					return true;
				}

			public:
				binStruct() : OOBAccuracy(-1.0),correctOOB(0),totalOOB(0),numberOfNodes(0),numOfTreesInBin(0),currTree(0), indicesHolder(fpSingleton::getSingleton().returnNumClasses()){	}


				inline void loadFirstNode(){
					//inline void loadFirstNode(obsIndexAndClassVec& indicesHolder, std::vector<zipClassAndValue<int, T> >& zipper){
					nodeQueue.emplace_back(0,0,0,randNum);
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
					bin[returnRootLocation()].setDepth(0);
				}

				inline void setSharedVectors(obsIndexAndClassVec& indicesInNode){
					indicesInNode.resetVectors();

					int numUnusedObs = fpSingleton::getSingleton().returnNumObservations();
					int randomObsID;
					int tempMoveObs;

					for(int n = 0; n < fpSingleton::getSingleton().returnNumObservations(); n++){
						randomObsID = randNum.gen(fpSingleton::getSingleton().returnNumObservations());

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
					assert(!nodeQueue.empty());
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
					int childDepth = returnDepthOfNode()+1;
					if(nodeQueue.back().isLeftChildLarger()){
						nodeQueue.pop_back();
						//TODO: don't emplace_back if should be leaf node.
						nodeQueue.emplace_back(1,parentNodesPosition(), childDepth, randNum);
						nodeQueue.back().setupNode(nodeIts, zipIts, rightNode());
						nodeQueue.emplace_back(1,parentNodesPosition(), childDepth, randNum);
						nodeQueue.back().setupNode(nodeIts, zipIts, leftNode());
					}else{
						nodeQueue.pop_back();
						nodeQueue.emplace_back(1,parentNodesPosition(), childDepth, randNum);
						nodeQueue.back().setupNode(nodeIts, zipIts, leftNode());
						nodeQueue.emplace_back(1,parentNodesPosition(), childDepth, randNum);
						nodeQueue.back().setupNode(nodeIts, zipIts, rightNode());
					}
				}


				inline void createRootChildNodes(){
					nodeIterators nodeIts(nodeQueue.back().returnNodeIterators());
					zipperIterators<int,T> zipIts(nodeQueue.back().returnZipIterators());
					int childDepth = returnDepthOfNode()+1;
					if(nodeQueue.back().isLeftChildLarger()){
						nodeQueue.pop_back();
						//TODO: don't emplace_back if should be leaf node.
						nodeQueue.emplace_back(1,returnRootLocation(), childDepth,randNum);
						nodeQueue.back().setupNode(nodeIts, zipIts, rightNode());
						nodeQueue.emplace_back(1,returnRootLocation(), childDepth, randNum);
						nodeQueue.back().setupNode(nodeIts, zipIts, leftNode());
					}else{
						nodeQueue.pop_back();
						nodeQueue.emplace_back(1,returnRootLocation(), childDepth,randNum);
						nodeQueue.back().setupNode(nodeIts, zipIts, leftNode());
						nodeQueue.emplace_back(1,returnRootLocation(), childDepth,randNum);
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
					// process the node, i.e. calculate best split, ...
					nodeQueue.back().processNode();
					if (nodeQueue.back().isLeafNode()) {
						// label the processed node as a leaf.
						processLeafNode();
					}
					else {
						// label the processed node as internal.
						processInternalNode();
					}
				}


				inline void createBin(int numTrees, int randSeed){
					numOfTreesInBin = numTrees;
					randNum.initialize(randSeed);
					initializeStructures();
					for(; currTree < numOfTreesInBin; ++currTree){
						setSharedVectors(indicesHolder);
						loadFirstNode();	
						while(!nodeQueue.empty()){
							processNode();
						}
					}
					removeStructures();
				}

				inline void initializeStructures(){
					zipper.resize(fpSingleton::getSingleton().returnNumObservations());
					nodeIndices.resize(fpSingleton::getSingleton().returnNumObservations());
					for(int i = 0; i < fpSingleton::getSingleton().returnNumObservations(); ++i){
						nodeIndices[i] =i;
					}
					bin.resize(numOfTreesInBin+fpSingleton::getSingleton().returnNumClasses());
					makeLeafNodes();
				}


				inline void removeStructures(){
					std::vector<processingNodeBin<T,Q> >().swap( nodeQueue );
					//indicesHolder.removeObsIndexAndClassVec();
					std::vector<zipClassAndValue<int, T> >().swap( zipper );
					std::vector<int>().swap( nodeIndices);
				}


				inline int returnMaxDepth(){
					int maxDepth=0;
					for(auto& node : bin){
						// +1 accounts for the leaf nodes which are never created (optimization that cuts memory required for a forest in half)
						if(maxDepth < node.returnDepth()+1){
							maxDepth = node.returnDepth()+1;
						}
					}
					return maxDepth;
				}


				inline int returnNumLeafNodes(){
					return (int)bin.size() - fpSingleton::getSingleton().returnNumClasses() + numOfTreesInBin;
				}


				inline int returnLeafDepthSum(){
					int leafDepthSums=0;
					for(auto& node : bin){
						if(node.isInternalNodeFront()){
							if(node.returnLeftNodeID() < fpSingleton::getSingleton().returnNumClasses()){
								leafDepthSums += node.returnDepth()+1;
							}
							if(node.returnRightNodeID() < fpSingleton::getSingleton().returnNumClasses()){
								leafDepthSums += node.returnDepth()+1;
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

				inline void predictBinObservation(std::vector<T>& observation, std::vector<int>& preds){
					predictBinObservation(observation,preds,identity<Q>());
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


				inline void predictBinObservation(int observationNum, std::vector<int>& preds, identity<weightedFeature>){
					std::vector<int> currNode(numOfTreesInBin);
					int numberNotInLeaf;
					T featureVal;
					int weightNum;
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
								weightNum = 0;
								for(auto i : bin[currNode[q]].returnFeatureNumber().returnFeatures()){
									featureVal += fpSingleton::getSingleton().returnTestFeatureVal(i,observationNum)*bin[currNode[q]].returnFeatureNumber().returnWeights()[weightNum++];
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

				inline void predictBinObservation(std::vector<T>& observation, std::vector<int>& preds,identity<int> ){
					std::vector<int> currNode(numOfTreesInBin);
					int numberNotInLeaf;
					int featureNum;
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
								currNode[q] = bin[currNode[q]].fpBaseNode<T, Q>::nextNode(observation[featureNum]);
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


				inline void predictBinObservation(std::vector<T>& observation, std::vector<int>& preds, identity<std::vector<int> >){
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
									featureVal +=observation[i];
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


				//Prediction function for ternary sparse matrix
				inline void predictBinObservation(std::vector<T>& observation, std::vector<int>& preds, identity<weightedFeature>){
					std::vector<int> currNode(numOfTreesInBin);
					int numberNotInLeaf;
					T featureVal;
					int weightNum;
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
								weightNum = 0;
								for(auto i : bin[currNode[q]].returnFeatureNumber().returnFeatures()){
									featureVal +=observation[i]*bin[currNode[q]].returnFeatureNumber().returnWeights()[weightNum++];
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
				///////////////////////////////////
				/// Test Functions not to be used in production
				//////////////////////////////////


				inline std::vector< fpBaseNode<T,Q> >& exposeBinTest(){
					return bin;
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
