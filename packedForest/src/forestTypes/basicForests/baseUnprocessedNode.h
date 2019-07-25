#ifndef baseUnprocessedNode_h
#define baseUnprocessedNode_h
#include "classTotals.h"
#include "stratifiedInNodeClassIndices.h"
#include "../../baseFunctions/buildSpecific.h"
#include <vector>

namespace fp{


	template <typename T> //
		class baseUnprocessedNode{
			protected:
				int parentID;
				int depth;
				double nodeImpurity; //lower impurity is better
				bool isLeftNode; //in order to set parent node with location


				stratifiedInNodeClassIndices* obsIndices;
				stratifiedInNodeClassIndices* leftIndices;
				stratifiedInNodeClassIndices* rightIndices;

				std::vector<T> featureHolder;
				std::vector<int> labelHolder;
				
			public:
				baseUnprocessedNode(int numObsForRoot):  parentID(0), depth(0), isLeftNode(true){
					obsIndices = new stratifiedInNodeClassIndices(numObsForRoot);
				}

				baseUnprocessedNode(int parentID, int dep, bool isLeft): parentID(parentID), depth(dep), isLeftNode(isLeft){}

				virtual ~baseUnprocessedNode(){}
				

				inline stratifiedInNodeClassIndices* returnLeftIndices(){
					return leftIndices;
				}

				inline stratifiedInNodeClassIndices* returnRightIndices(){
					return rightIndices;
				}

				inline int returnParentID(){
					return parentID;
				}

				inline int returnDepth(){
					return depth;
				}
				inline double returnNodeImpurity(){
					return nodeImpurity;
				}

				inline void setNodeImpurity(double nodeImp){
					nodeImpurity = nodeImp;
				}

				inline bool isNodePure(){
					return nodeImpurity == 0;
				}

				inline bool returnIsLeftNode(){
					return isLeftNode;
				}

				inline int returnInSampleSize(){
					return obsIndices->returnInSampleSize();
				}

				inline int returnOutSampleSize(){
					return obsIndices->returnOutSampleSize();
				}

				inline int returnOutSampleCorrect(int classNum){
 					return obsIndices->returnOutSamplesInClass(classNum);
				}

				inline std::vector<int> returnOutSampleIdsFromLeaf(){
					 return obsIndices->returnOutSample();
				}


				/*


					 inline int returnBestFeature(){
					 return bestSplitInfo.returnFeatureNum();
					 }

					 inline double returnBestImpurity(){
					 return bestSplitInfo.returnImpurity();
					 }

					 inline T returnBestCutValue(){
					 return bestSplitInfo.returnSplitValue();
					 }

*/
				inline void setHolderSizes(){
					obsIndices->initializeBinnedSamples();
					if(obsIndices->useBin()){
						labelHolder.resize(obsIndices->returnBinnedSize());
						featureHolder.resize(obsIndices->returnBinnedSize());
					}else{
						labelHolder.resize(obsIndices->returnInSampleSize());
						featureHolder.resize(obsIndices->returnInSampleSize());
					}
				}


				inline void loadLabelHolder(){
					if(obsIndices->useBin()){
						for(int i =0; i < obsIndices->returnBinnedSize(); ++i){
							labelHolder[i] = fpSingleton::getSingleton().returnLabel(obsIndices->returnBinnedInSample(i));
						}
					}else{
						for(int i =0; i < obsIndices->returnInSampleSize(); ++i){
							labelHolder[i] = fpSingleton::getSingleton().returnLabel(obsIndices->returnInSample(i));
						}
					}
				}


				/*
					 inline void loadFeatureHolder(){
					 if(obsIndices->useBin()){
					 for(int q=0; q<obsIndices->returnBinnedSize(); q++){
					 fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back(),obsIndices->returnBinnedInSample(q));
					 }

					 for(int i =0; i < obsIndices->returnBinnedSize(); ++i){
					 featureHolder[i] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back(),obsIndices->returnBinnedInSample(i));
					 }
					 }else{

					 for(int q=0; q<obsIndices->returnInSampleSize(); q++){
					 fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back(),obsIndices->returnInSample(q));
					 }

					 for(int i =0; i < obsIndices->returnInSampleSize(); ++i){
					 featureHolder[i] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back(),obsIndices->returnInSample(i));
					 }
					 }
					 }

*/
				inline float calculateNodeImpurity(){
					return obsIndices->returnImpurity();
				}


				/*
					 inline void setupNode(){
					 pickMTRY();
					 setHolderSizes();
					 loadLabelHolder();
					 nodeImpurity = calculateNodeImpurity();
					 }


					 inline void setBestSplit(splitInfo<T> tempSplit){
					 if(tempSplit.returnImpurity() < bestSplitInfo.returnImpurity()){
					 bestSplitInfo = tempSplit;
					 }
					 }

*/

				inline void loadIndices(stratifiedInNodeClassIndices* indices){
					obsIndices = indices;
				}
				/*


					 inline bool goLeft(const int& index){
					 int inIndex = index;
					 int featureNum = bestSplitInfo.returnFeatureNum();
					 if(featureNum == -1){
					 std::cout << "found it\n";
					 exit(1);
					 }
					 double featureVal = fpSingleton::getSingleton().returnFeatureVal(featureNum,inIndex);
					 double splitVal = bestSplitInfo.returnSplitValue();
					 if(featureVal < splitVal ){
					 return true;
					 }else{
					 return false;
					 }
					 }

					 inline void moveDataLeftOrRight(){
					 if(leftIndices !=NULL){
					 delete leftIndices;
					 }
					 if(rightIndices !=NULL){
					 delete rightIndices;
					 }

					 leftIndices = new stratifiedInNodeClassIndices();
					 rightIndices = new stratifiedInNodeClassIndices();

					 int lNum =0;
					 int rNum =0;
					 for (int i=0; i < obsIndices->returnInSampleSize();++i){
					 if(goLeft(obsIndices->returnInSample(i))){
					 ++lNum;
					 leftIndices->addIndexToInSamples(obsIndices->returnInSample(i));	
					 }else{
					 ++rNum;
					 rightIndices->addIndexToInSamples(obsIndices->returnInSample(i));	
					 }
					 }

					 for (int i=0; i < obsIndices->returnOutSampleSize();++i){
					 if(goLeft(obsIndices->returnInSample(i))){
					 leftIndices->addIndexToOutSamples(obsIndices->returnInSample(i));	
					 }else{
					 rightIndices->addIndexToOutSamples(obsIndices->returnInSample(i));	
					 }
					 }

					 delete obsIndices;
					 obsIndices = NULL;
					 }


					 inline void findBestSplit(){
				//timeLogger logTime;
				fpSplit<T> findSplit(labelHolder); //This is done twice
				while(!featuresToTry.empty()){
				//logTime.startGiniTimer();
				loadFeatureHolder();
				//logTime.stopGiniTimer();
				setBestSplit(findSplit.giniSplit(featureHolder ,featuresToTry.back()));

				featuresToTry.pop_back();
				}
				}

*/

				inline int returnMaxClass(){
					if(labelHolder.empty()){
						std::cout << "no labels in holder\n";
						exit(1);
					}
					if(nodeImpurity==0){
						return labelHolder[0];
					} else {
						classTotals findMaxClass;
						findMaxClass.findNumClasses(labelHolder);
						return findMaxClass.returnLargestClass();
					}
				}



		}; //unprocessedNode.h
}//namespace fp
#endif //baseUnprocessedNode_h
