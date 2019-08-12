#ifndef rfunprocessedURerFNode_h
#define rfunprocessedURerFNode_h
#include "splitURerF.h"
#include "../baseUnprocessedNodeUnsupervised.h"
#include "../stratifiedInNodeClassIndicesUnsupervised.h"
#include <vector>
#include <random>
#include <assert.h>

namespace fp{


	template <typename T> //
		class unprocessedURerFNode : public baseUnprocessedNodeUnsupervised<T>{
			protected:

				splitURerFInfo<T> bestSplitInfo;
				std::vector< std::vector<int> > featuresToTry;

				//std::random_device rd;
				//The next three should be static
				//std::mt19937 rng(rd());   
				//std::uniform_int_distribution<int> randomMtry(0,fpSingleton::getSingleton().returnMtry()-1);
				//std::uniform_int_distribution<int> randomFeature(0,fpSingleton::getSingleton().returnNumFeatures()-1);
				//Example: auto random_integer = uni(rng);

			public:
				unprocessedURerFNode(int numObsForRoot): baseUnprocessedNodeUnsupervised<T>::baseUnprocessedNodeUnsupervised(numObsForRoot), featuresToTry(fpSingleton::getSingleton().returnMtry()){}


				unprocessedURerFNode(int parentID, int dep, bool isLeft): baseUnprocessedNodeUnsupervised<T>::baseUnprocessedNodeUnsupervised(parentID, dep, isLeft), featuresToTry(fpSingleton::getSingleton().returnMtry()){}


				~unprocessedURerFNode(){}
				


				inline std::vector<int>& returnBestFeature(){
					return bestSplitInfo.returnFeatureNum();
				}

				inline double returnBestImpurity(){
					return bestSplitInfo.returnImpurity();
				}

				inline T returnBestCutValue(){
					return bestSplitInfo.returnSplitValue();
				}


				inline void setBestSplit(splitURerFInfo<T> tempSplit){
					if(tempSplit.returnImpurity() >= 0){
						if(tempSplit.returnImpurity() < bestSplitInfo.returnImpurity()){
							bestSplitInfo = tempSplit;
						}
					}
				}

				inline void pickMTRY(){
					int rndMtry;
					int rndFeature;
					for (int i=0; i < fpSingleton::getSingleton().returnMtry(); ++i){
						rndMtry = std::rand() % fpSingleton::getSingleton().returnMtry();
						rndFeature = std::rand() % fpSingleton::getSingleton().returnNumFeatures();
						featuresToTry[rndMtry].push_back(rndFeature);
					}
				}

				inline void loadFeatureHolder(){
					if(baseUnprocessedNodeUnsupervised<T>::obsIndices->useBin()){
						for(int q=0; q<baseUnprocessedNodeUnsupervised<T>::obsIndices->returnBinnedSize(); q++){
							fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[0],baseUnprocessedNodeUnsupervised<T>::obsIndices->returnBinnedInSample(q));
						}

						for(int i =0; i < baseUnprocessedNodeUnsupervised<T>::obsIndices->returnBinnedSize(); ++i){
							baseUnprocessedNodeUnsupervised<T>::featureHolder[i] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[0],baseUnprocessedNodeUnsupervised<T>::obsIndices->returnBinnedInSample(i));
						}
						if(featuresToTry.back().size()>1){
							for(unsigned int j =1; j < featuresToTry.back().size(); ++j){
								for(int q=0; q<baseUnprocessedNodeUnsupervised<T>::obsIndices->returnBinnedSize(); q++){
									fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[j],baseUnprocessedNodeUnsupervised<T>::obsIndices->returnBinnedInSample(q));
								}
								for(int i =0; i < baseUnprocessedNodeUnsupervised<T>::obsIndices->returnBinnedSize(); ++i){
									baseUnprocessedNodeUnsupervised<T>::featureHolder[i] += fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[j],baseUnprocessedNodeUnsupervised<T>::obsIndices->returnBinnedInSample(i));
								}
							}
						}
					}else{

						for(int q=0; q<baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSampleSize(); q++){
							fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[0],baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSample(q));
						}

						for(int i =0; i < baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSampleSize(); ++i){
							baseUnprocessedNodeUnsupervised<T>::featureHolder[i] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[0],baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSample(i));
						}
						if(featuresToTry.back().size()>1){
							for(int j =1; j < (int)featuresToTry.back().size(); ++j){
								for(int q=0; q<baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSampleSize(); q++){
									fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[j],baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSample(q));
								}

								for(int i =0; i < baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSampleSize(); ++i){
									baseUnprocessedNodeUnsupervised<T>::featureHolder[i] += fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[j],baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSample(i));
								}
							}
						}
					}
				}

				inline void setupNode(){
					pickMTRY();
					baseUnprocessedNodeUnsupervised<T>::setHolderSizes();
					baseUnprocessedNodeUnsupervised<T>::setNodeImpurity(baseUnprocessedNodeUnsupervised<T>::calculateNodeImpurity());
				}


				inline bool goLeft(const int index){
					T featureVal = 0;

					for(auto j : bestSplitInfo.returnFeatureNum()){
						featureVal += fpSingleton::getSingleton().returnFeatureVal(j,index);
					}
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

					assert(lNum > 0);
					assert(rNum > 0);


					for (int i=0; i < baseUnprocessedNodeUnsupervised<T>::obsIndices->returnOutSampleSize();++i){
						if(goLeft(baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSample(i))){
							baseUnprocessedNodeUnsupervised<T>::leftIndices->addIndexToOutSamples(baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSample(i));	
						}else{
							baseUnprocessedNodeUnsupervised<T>::rightIndices->addIndexToOutSamples(baseUnprocessedNodeUnsupervised<T>::obsIndices->returnInSample(i));	
						}
					}
					 baseUnprocessedNodeUnsupervised<T>::leftIndices->setNodeImpurity(bestSplitInfo.returnImpurity());
                                        baseUnprocessedNodeUnsupervised<T>::rightIndices->setNodeImpurity(bestSplitInfo.returnImpurity());
					deleteObsIndices();
				}


				inline void findBestSplit(){
					//timeLogger logTime;
					splitURerF<T> findSplit; //This is done twice
					//TODO This needs to change to real mtry
					//	std::vector<int> tempVec;
					//	tempVec.push_back(0);
					
					while(!featuresToTry.empty()){
						//not all featuresToTry will be populated.  This checks first.
						if(!featuresToTry.back().empty()){
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
#endif //unprocessedNode_h
