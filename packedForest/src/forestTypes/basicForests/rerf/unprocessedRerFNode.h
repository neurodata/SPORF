#ifndef rfunprocessedRerFNode_h
#define rfunprocessedRerFNode_h
#include "splitRerF.h"
#include "../baseUnprocessedNode.h"
#include <vector>
#include <random>
#include <assert.h>

namespace fp{


	template <typename T> //
		class unprocessedRerFNode : public baseUnprocessedNode<T>{
			protected:

				splitRerFInfo<T> bestSplitInfo;
				std::vector< std::vector<int> > featuresToTry;

				//std::random_device rd;
				//The next three should be static
				//std::mt19937 rng(rd());   
				//std::uniform_int_distribution<int> randomMtry(0,fpSingleton::getSingleton().returnMtry()-1);
				//std::uniform_int_distribution<int> randomFeature(0,fpSingleton::getSingleton().returnNumFeatures()-1);
				//Example: auto random_integer = uni(rng);

			public:
				unprocessedRerFNode(int numObsForRoot): baseUnprocessedNode<T>::baseUnprocessedNode(numObsForRoot), featuresToTry(fpSingleton::getSingleton().returnMtry()){}


				unprocessedRerFNode(int parentID, int dep, bool isLeft): baseUnprocessedNode<T>::baseUnprocessedNode(parentID, dep, isLeft), featuresToTry(fpSingleton::getSingleton().returnMtry()){}


				~unprocessedRerFNode(){}



				inline std::vector<int>& returnBestFeature(){
					return bestSplitInfo.returnFeatureNum();
				}

				inline double returnBestImpurity(){
					return bestSplitInfo.returnImpurity();
				}

				inline T returnBestCutValue(){
					return bestSplitInfo.returnSplitValue();
				}


				inline void setBestSplit(splitRerFInfo<T> tempSplit){
					if(tempSplit.returnImpurity() < bestSplitInfo.returnImpurity()){
						bestSplitInfo = tempSplit;
					}
				}

				inline void pickMTRY(){
					int rndMtry;
					int rndFeature;
					int mtryDensity = (int)((double)fpSingleton::getSingleton().returnMtry()*fpSingleton::getSingleton().returnMtryMult());
					for (int i=0; i < mtryDensity; ++i){
						rndMtry = std::rand() % fpSingleton::getSingleton().returnMtry();
						rndFeature = std::rand() % fpSingleton::getSingleton().returnNumFeatures();
						featuresToTry[rndMtry].push_back(rndFeature);
					}
				}

				//Note, this seems like a really intimidating function but it is just a 
				//basic prefetched gather operation.  Without prefetching this function
				//would be less than half as long.
				inline void loadFeatureHolder(){
				int numToPrefetch=globalPrefetchSize;
					if(baseUnprocessedNode<T>::obsIndices->useBin()){
						if(baseUnprocessedNode<T>::obsIndices->returnBinnedSize() < globalPrefetchSize){
							numToPrefetch = baseUnprocessedNode<T>::obsIndices->returnBinnedSize();
						}
						//load the first feature
						for(int q=0; q<numToPrefetch; q++){
							fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[0],baseUnprocessedNode<T>::obsIndices->returnBinnedInSample(q));
						}

						for(int i =0; i < baseUnprocessedNode<T>::obsIndices->returnBinnedSize()-numToPrefetch; ++i){
							fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[0],baseUnprocessedNode<T>::obsIndices->returnBinnedInSample(i+numToPrefetch));
							baseUnprocessedNode<T>::featureHolder[i] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[0],baseUnprocessedNode<T>::obsIndices->returnBinnedInSample(i));
						}

						for(int q=baseUnprocessedNode<T>::obsIndices->returnBinnedSize()-numToPrefetch; q<baseUnprocessedNode<T>::obsIndices->returnBinnedSize(); ++q){
							baseUnprocessedNode<T>::featureHolder[q] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[0],baseUnprocessedNode<T>::obsIndices->returnBinnedInSample(q));
						}

						//load all additional features
						if(featuresToTry.back().size()>1){

							for(unsigned int j =1; j < featuresToTry.back().size(); ++j){

								for(int q=0; q<numToPrefetch; q++){
									fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[j],baseUnprocessedNode<T>::obsIndices->returnBinnedInSample(q));
								}

								for(int i =0; i < baseUnprocessedNode<T>::obsIndices->returnBinnedSize()-numToPrefetch; ++i){
									fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[j],baseUnprocessedNode<T>::obsIndices->returnBinnedInSample(i+numToPrefetch));
									baseUnprocessedNode<T>::featureHolder[i] += fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[j],baseUnprocessedNode<T>::obsIndices->returnBinnedInSample(i));
								}

								for(int q=baseUnprocessedNode<T>::obsIndices->returnBinnedSize()-numToPrefetch; q<baseUnprocessedNode<T>::obsIndices->returnBinnedSize(); ++q){
									baseUnprocessedNode<T>::featureHolder[q] += fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[j],baseUnprocessedNode<T>::obsIndices->returnBinnedInSample(q));
								}

							}
						}
					}else{
						if(baseUnprocessedNode<T>::obsIndices->returnInSampleSize() <globalPrefetchSize){
							numToPrefetch = baseUnprocessedNode<T>::obsIndices->returnInSampleSize();
						}
						//load the first feature
						for(int q=0; q<numToPrefetch; q++){
							fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[0],baseUnprocessedNode<T>::obsIndices->returnInSample(q));
						}

						for(int i =0; i < baseUnprocessedNode<T>::obsIndices->returnInSampleSize()-numToPrefetch; ++i){
							fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[0],baseUnprocessedNode<T>::obsIndices->returnInSample(i+numToPrefetch));
							baseUnprocessedNode<T>::featureHolder[i] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[0],baseUnprocessedNode<T>::obsIndices->returnInSample(i));
						}

						for(int q=baseUnprocessedNode<T>::obsIndices->returnInSampleSize()-numToPrefetch; q<baseUnprocessedNode<T>::obsIndices->returnInSampleSize(); ++q){
							baseUnprocessedNode<T>::featureHolder[q] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[0],baseUnprocessedNode<T>::obsIndices->returnInSample(q));
						}


						//if more than one feature exists in MTRY load these now.
						if(featuresToTry.back().size()>1){
							for(int j =1; j < (int)featuresToTry.back().size(); ++j){

								for(int q=0; q<numToPrefetch; q++){
									fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[j],baseUnprocessedNode<T>::obsIndices->returnInSample(q));
								}

								for(int i =0; i < baseUnprocessedNode<T>::obsIndices->returnInSampleSize()-numToPrefetch; ++i){
									fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[j],baseUnprocessedNode<T>::obsIndices->returnInSample(i+numToPrefetch));
									baseUnprocessedNode<T>::featureHolder[i] += fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[j],baseUnprocessedNode<T>::obsIndices->returnInSample(i));
								}

								for(int q=baseUnprocessedNode<T>::obsIndices->returnInSampleSize()-numToPrefetch; q<baseUnprocessedNode<T>::obsIndices->returnInSampleSize(); ++q){
									baseUnprocessedNode<T>::featureHolder[q] += fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[j],baseUnprocessedNode<T>::obsIndices->returnInSample(q));
								}

							}
						}
					}
				}

				inline void setupNode(){
					pickMTRY();
					baseUnprocessedNode<T>::setHolderSizes();
					baseUnprocessedNode<T>::loadLabelHolder();
					baseUnprocessedNode<T>::setNodeImpurity(baseUnprocessedNode<T>::calculateNodeImpurity());
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
					delete baseUnprocessedNode<T>::obsIndices;
					baseUnprocessedNode<T>::obsIndices = NULL;
				}

				inline void moveDataLeftOrRight(){

					baseUnprocessedNode<T>::leftIndices = new stratifiedInNodeClassIndices();
					baseUnprocessedNode<T>::rightIndices = new stratifiedInNodeClassIndices();

					int lNum =0;
					int rNum =0;
					for (int i=0; i < baseUnprocessedNode<T>::obsIndices->returnInSampleSize();++i){
						if(goLeft(baseUnprocessedNode<T>::obsIndices->returnInSample(i))){
							++lNum;
							baseUnprocessedNode<T>::leftIndices->addIndexToInSamples(baseUnprocessedNode<T>::obsIndices->returnInSample(i));	
						}else{
							++rNum;
							baseUnprocessedNode<T>::rightIndices->addIndexToInSamples(baseUnprocessedNode<T>::obsIndices->returnInSample(i));	
						}
					}


					assert(lNum > 0);
					assert(rNum > 0);


					for (int i=0; i < baseUnprocessedNode<T>::obsIndices->returnOutSampleSize();++i){
						if(goLeft(baseUnprocessedNode<T>::obsIndices->returnOutSample(i))){
							baseUnprocessedNode<T>::leftIndices->addIndexToOutSamples(baseUnprocessedNode<T>::obsIndices->returnOutSample(i));
						}else{
							baseUnprocessedNode<T>::rightIndices->addIndexToOutSamples(baseUnprocessedNode<T>::obsIndices->returnOutSample(i));
						}
					}
					deleteObsIndices();
				}


				inline void findBestSplit(){
					//timeLogger logTime;
					splitRerF<T> findSplit(baseUnprocessedNode<T>::labelHolder); //This is done twice
					//TODO This needs to change to real mtry
					//	std::vector<int> tempVec;
					//	tempVec.push_back(0);
					while(!featuresToTry.empty()){
						//not all featuresToTry will be populated.  This checks first.
						if(!featuresToTry.back().empty()){
							//logTime.startGiniTimer();
							loadFeatureHolder();
							//logTime.stopGiniTimer();
							//tempVec = featuresToTry.back();
							setBestSplit(findSplit.giniSplit(baseUnprocessedNode<T>::featureHolder ,featuresToTry.back()));
							//setBestSplit(findSplit.giniSplit(featureHolder ,tempVec));
						}
						removeTriedMtry();
						//	featuresToTry.pop_back();
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
