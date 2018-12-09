#ifndef baseUnprocessedNode_h
#define baseUnprocessedNode_h
//#include "splitRerF.h"
#include "classTotals.h"
#include <vector>
#include <random>

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

				//std::vector<int> featuresToTry;
				std::vector<T> featureHolder;
				std::vector<int> labelHolder;


				/*
					 splitRerFInfo<T> bestSplitInfo;
					 std::vector< std::vector<int> > featuresToTry;
					 */

				//std::random_device rd;
				//The next three should be static
				//std::mt19937 rng(rd());   
				//std::uniform_int_distribution<int> randomMtry(0,fpSingleton::getSingleton().returnMtry()-1);
				//std::uniform_int_distribution<int> randomFeature(0,fpSingleton::getSingleton().returnNumFeatures()-1);
				//Example: auto random_integer = uni(rng);

			public:
				baseUnprocessedNode(int numObsForRoot):  parentID(0), depth(0), isLeftNode(true){
					obsIndices = new stratifiedInNodeClassIndices(numObsForRoot);
					//		featuresToTry.resize(fpSingleton::getSingleton().returnMtry());
					leftIndices=NULL;
					rightIndices=NULL;
				}

				baseUnprocessedNode(int parentID, int dep, bool isLeft):parentID(parentID),depth(dep),isLeftNode(isLeft){
					//		featuresToTry.resize(fpSingleton::getSingleton().returnMtry());
					leftIndices=NULL;
					rightIndices=NULL;
				}

				~baseUnprocessedNode(){
					//	delete leftIndices;
					//	delete rightIndices;
				}

				inline stratifiedInNodeClassIndices* returnLeftIndices(){
					return leftIndices;
				}

				inline stratifiedInNodeClassIndices* returnRightIndices(){
					return rightIndices;
				}

				inline int returnParentID(){
					return parentID;
				}

				inline double returnNodeImpurity(){
					return nodeImpurity;
				}

				inline bool returnIsLeftNode(){
					return isLeftNode;
				}

				inline int returnDepth(){
					return depth;
				}

				/*
					 inline std::vector<int> returnBestFeature(){
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

				inline int returnInSampleSize(){
					return obsIndices->returnInSampleSize();
				}

				inline int returnOutSampleSize(){
					return obsIndices->returnOutSampleSize();
				}

				inline int returnOutSampleError(int classNum){
					int totalRight=0;
					//	//for(int i : obsIndices->returnOutSamples()){
					//	for(unsigned int i =0; i <  obsIndices->returnSize(); ++i){
					//		if (i==classNum){
					//		++totalRight;
					//		}
					//		}
					return totalRight;
				}


				/*
					 inline void pickMTRY(){
				//for (int i=0; i<fpSingleton::getSingleton().returnNumFeatures(); ++i) featuresToTry.push_back(i);
				//std::random_shuffle ( featuresToTry.begin(), featuresToTry.end() );
				//featuresToTry.resize(fpSingleton::getSingleton().returnMtry());
				int rndMtry;
				int rndFeature;
				/*
				for (int i=0; i < fpSingleton::getSingleton().returnMtry(); ++i){
//rndMtry = std::rand() % fpSingleton::getSingleton().returnMtry();
rndFeature = std::rand() % fpSingleton::getSingleton().returnNumFeatures();
featuresToTry[i].push_back(rndFeature);
//featuresToTry[randomMtry(rng)].push_back(randomFeature(rng));
}
*/

/*
	 for (int i=0; i < fpSingleton::getSingleton().returnMtry(); ++i){
	 rndMtry = std::rand() % fpSingleton::getSingleton().returnMtry();
	 rndFeature = std::rand() % fpSingleton::getSingleton().returnNumFeatures();
	 featuresToTry[rndMtry].push_back(rndFeature);
//featuresToTry[randomMtry(rng)].push_back(randomFeature(rng));
}
}

inline int returnMTRYVectorSizeDelete(){
return featuresToTry.size();
}

*/
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


inline void loadFeatureHolder(){
	if(obsIndices->useBin()){
		for(int q=0; q<obsIndices->returnBinnedSize(); q++){
			fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[0],obsIndices->returnBinnedInSample(q));
		}

		for(int i =0; i < obsIndices->returnBinnedSize(); ++i){
			featureHolder[i] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[0],obsIndices->returnBinnedInSample(i));
		}
		if(featuresToTry.back().size()>1){
			for(unsigned int j =1; j < featuresToTry.back().size(); ++j){
				for(int q=0; q<obsIndices->returnBinnedSize(); q++){
					fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[j],obsIndices->returnBinnedInSample(q));
				}
				for(int i =0; i < obsIndices->returnBinnedSize(); ++i){
					featureHolder[i] += fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[j],obsIndices->returnBinnedInSample(i));
				}
			}
		}
	}else{

		for(int q=0; q<obsIndices->returnInSampleSize(); q++){
			fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[0],obsIndices->returnInSample(q));
		}

		for(int i =0; i < obsIndices->returnInSampleSize(); ++i){
			featureHolder[i] = fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[0],obsIndices->returnInSample(i));
		}
		if(featuresToTry.back().size()>1){
			for(unsigned int j =1; j < featuresToTry.back().size(); ++j){
				for(int q=0; q<obsIndices->returnInSampleSize(); q++){
					fpSingleton::getSingleton().prefetchFeatureVal(featuresToTry.back()[j],obsIndices->returnInSample(q));
				}

				for(int i =0; i < obsIndices->returnInSampleSize(); ++i){
					featureHolder[i] += fpSingleton::getSingleton().returnFeatureVal(featuresToTry.back()[j],obsIndices->returnInSample(i));
				}
			}
		}
	}
}

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


	 inline void setBestSplit(splitRerFInf/<T> tempSplit){
	 if(tempSplit.returnImpurity() < bestSplitInfo.returnImpurity()){
	 bestSplitInfo = tempSplit;
	 }
	 }


*/
inline void loadIndices(stratifiedInNodeClassIndices* indices){
	obsIndices = indices;
}


inline bool goLeft(const int& index){
	int inIndex = index;
	std::vector<int> featureNum = bestSplitInfo.returnFeatureNum();
	if(featureNum.empty()){
		std::cout << "found it\n";
		exit(1);
	}
	double featureVal = fpSingleton::getSingleton().returnFeatureVal(featureNum[0],inIndex);
	if(featureNum.size() > 1){
		for(unsigned int j = 1; j < featureNum.size(); ++j){
			featureVal += fpSingleton::getSingleton().returnFeatureVal(featureNum[j],inIndex);
		}
	}

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
/*


	 inline void findBestSplit(){
//timeLogger logTime;
splitRerF<T> findSplit(labelHolder); //This is done twice
//TODO This needs to change to real mtry
std::vector<int> tempVec;
tempVec.push_back(0);
while(!featuresToTry.empty()){
//not all featuresToTry will be populated.  This checks first.
if(!featuresToTry.back().empty()){
//logTime.startGiniTimer();
loadFeatureHolder();
//logTime.stopGiniTimer();
//tempVec = featuresToTry.back();
setBestSplit(findSplit.giniSplit(featureHolder ,featuresToTry.back()));
//setBestSplit(findSplit.giniSplit(featureHolder ,tempVec));
}
featuresToTry.pop_back();
}
}

*/
inline int returnMaxClass(){
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
