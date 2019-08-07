#ifndef rfunprocessedURFNode_h
#define rfunprocessedURFNode_h
#include "splitURF.h"
#include "../baseUnprocessedNodeUnsupervised.h"
#include "../stratifiedInNodeClassIndicesUnsupervised.h"
#include <vector>
#include <random>
#include <assert.h>

namespace fp{


	template <typename T> //
		class unprocessedURFNode : public baseUnprocessedNodeUnsupervised<T>{
			protected:

				splitURFInfo<T> bestSplitInfo;
				std::vector<int> featuresToTry;

				//std::random_device rd;
				//The next three should be static
				//std::mt19937 rng(rd());   
				//std::uniform_int_distribution<int> randomMtry(0,fpSingleton::getSingleton().returnMtry()-1);
				//std::uniform_int_distribution<int> randomFeature(0,fpSingleton::getSingleton().returnNumFeatures()-1);
				//Example: auto random_integer = uni(rng);

			public:
                                unprocessedURFNode(int numObsForRoot): baseUnprocessedNodeUnsupervised<T>::baseUnprocessedNodeUnsupervised(numObsForRoot), featuresToTry(fpSingleton::getSingleton().returnMtry()){}


                                unprocessedURFNode(int parentID, int dep, bool isLeft): baseUnprocessedNodeUnsupervised<T>::baseUnprocessedNodeUnsupervised(parentID, dep, isLeft){
}

				~unprocessedURFNode(){}
				

				inline int returnBestFeature(){
					return bestSplitInfo.returnFeatureNum();
				}

				inline double returnBestImpurity(){
					return bestSplitInfo.returnImpurity();
				}

				inline T returnBestCutValue(){
					return bestSplitInfo.returnSplitValue();
				}

				inline void setBestSplit(splitURFInfo<T> tempSplit){
					if(tempSplit.returnImpurity() >= 0){
						if(tempSplit.returnImpurity() < bestSplitInfo.returnImpurity()){
							bestSplitInfo = tempSplit;
						}
					}
				}

				inline void pickMTRY(){
					for (int i=0; i < fpSingleton::getSingleton().returnNumFeatures(); ++i){
						featuresToTry.push_back(i);
					}
                                        std::random_device rd; // obtain a random number from hardware
                                        std::mt19937 eng(rd()); // seed the generator

                                        int tempSwap;

                                        for(int locationToMove = 0; locationToMove < fpSingleton::getSingleton().returnMtry(); locationToMove++){
                                                std::uniform_int_distribution<> distr(locationToMove, fpSingleton::getSingleton().returnNumFeatures()-1);
                                                int randomPosition = distr(eng);
                                                tempSwap = featuresToTry[locationToMove];
                                                featuresToTry[locationToMove] = featuresToTry[randomPosition];
                                                featuresToTry[randomPosition] = tempSwap;
                                        }
                                        featuresToTry.resize(fpSingleton::getSingleton().returnMtry());
				}

				inline void loadFeatureHolder(){
					if(baseUnprocessedNodeUnsupervised<T>::obsIndices->useBin()){
						for(int q=0; q<baseUnprocessedNodeUnsupervised<T>::obsIndices->returnBinnedSize(); q++){
							fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back(),baseUnprocessedNodeUnsupervised<T>::obsIndices->returnBinnedInSample(q));
						}

						for(int i =0; i < baseUnprocessedNodeUnsupervised<T>::obsIndices->returnBinnedSize(); ++i){
							baseUnprocessedNodeUnsupervised<T>::featureHolder[i] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back(),baseUnprocessedNodeUnsupervised<T>::obsIndices->returnBinnedInSample(i));
						}
					}else{

						for(int q=0; q<baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSampleSize(); q++){
							fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back(),baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSample(q));
						}

						for(int i =0; i < baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSampleSize(); ++i){
							baseUnprocessedNodeUnsupervised<T>::featureHolder[i] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back(), baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSample(i));
						}
					}
				}

				inline void setupNode(){
					pickMTRY();
					baseUnprocessedNodeUnsupervised<T>::setHolderSizes();
					baseUnprocessedNodeUnsupervised<T>::setNodeImpurity(baseUnprocessedNodeUnsupervised<T>::calculateNodeImpurity());
				}


				inline bool goLeft(const int index){
					T featureVal = fpSingleton::getSingleton().returnFeatureVal(bestSplitInfo.returnFeatureNum(),index);

					if(featureVal <= bestSplitInfo.returnSplitValue()){
						return true;
					}else{
						return false;
					}
				}

				inline void deleteObsIndices(){
					delete baseUnprocessedNodeUnsupervised<T>::obsIndices;
					baseUnprocessedNodeUnsupervised<T>::obsIndices = NULL;
				}

				inline void moveDataLeftOrRight(){

					baseUnprocessedNodeUnsupervised<T>::leftIndices = new stratifiedInNodeClassIndicesUnsupervised();
					baseUnprocessedNodeUnsupervised<T>::rightIndices = new stratifiedInNodeClassIndicesUnsupervised();

					int lNum =0;
					int rNum =0;
					for (int i=0; i < baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSampleSize();++i){
						if(goLeft(baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSample(i))){
							++lNum;
							baseUnprocessedNodeUnsupervised<T>::leftIndices->addIndexToInSamples(baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSample(i));	
						}else{
							++rNum;
							baseUnprocessedNodeUnsupervised<T>::rightIndices->addIndexToInSamples(baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSample(i));	
						}
					}
                                        if(lNum <= 0 || rNum <= 0){
                                                lNum++;
                                                rNum--;
                                        }

					assert(lNum > 0);
					assert(rNum > 0);


					for (int i=0; i < baseUnprocessedNodeUnsupervised<T>::obsIndices->returnOutSampleSize();++i){
						if(goLeft(baseUnprocessedNodeUnsupervised<T>::obsIndices->returnOutSample(i))){
							baseUnprocessedNodeUnsupervised<T>::leftIndices->addIndexToOutSamples(baseUnprocessedNodeUnsupervised<T>::obsIndices->returnOutSample(i));	
						}else{
							baseUnprocessedNodeUnsupervised<T>::rightIndices->addIndexToOutSamples(baseUnprocessedNodeUnsupervised<T>::obsIndices->returnOutSample(i));	
						}
					}
					baseUnprocessedNodeUnsupervised<T>::leftIndices->setNodeImpurity(bestSplitInfo.returnImpurity());
					baseUnprocessedNodeUnsupervised<T>::rightIndices->setNodeImpurity(bestSplitInfo.returnImpurity());
					deleteObsIndices();
				}


				inline void findBestSplit(){
					//timeLogger logTime;
					splitURF<T> findSplit; //This is done twice
					//TODO This needs to change to real mtry
					//	std::vector<int> tempVec;
					//	tempVec.push_back(0);
					while(!featuresToTry.empty()){
						//not all featuresToTry will be populated.  This checks first.
						if(!featuresToTry.empty()){
							loadFeatureHolder();
							setBestSplit(findSplit.twoMeanSplit(baseUnprocessedNodeUnsupervised<T>::featureHolder ,featuresToTry.back()));
						}
						removeTriedMtry();
					}
					
				}

				inline void removeTriedMtry(){
					if(bestSplitInfo.perfectSplitFound()){
						featuresToTry.clear();
					}else{
						featuresToTry.pop_back();
					}
				}


		}; //unprocessedNode.h
}//namespace fp
#endif //unprocessedURFNode_h
