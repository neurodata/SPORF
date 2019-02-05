#ifndef fpSplit_h
#define fpSplit_h

#include "../../../baseFunctions/timeLogger.h"
#include "splitInfo.h"
#include "../labeledData.h"
#include "../classTotals.h"
#include "../../../baseFunctions/pdqsort.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <utility>
#include <assert.h>

namespace fp{

	template<typename T>
		class fpSplit {
			protected:
				double overallImpurity;
				classTotals leftClasses;
				classTotals rightClasses;

				std::vector<int> labels;
				std::vector< labeledData<T> > combinedDataLabels;

				inline void setCombinedVecSize(){
					combinedDataLabels.resize(labels.size());
				}

				inline void zipDataLabels(const std::vector<T>& featureVals){
					for(unsigned int i=0; i<labels.size(); ++i){
						combinedDataLabels[i].setPair(featureVals[i],labels[i]);
					}
				}

				inline T midVal(int leftIndex){
					return combinedDataLabels[leftIndex].midVal(combinedDataLabels[leftIndex+1]);
				}

				inline bool checkInequalityWithNext(int leftIndex){
					return combinedDataLabels[leftIndex].checkInequality(combinedDataLabels[leftIndex+1]);
				}

				inline void setupForNextRun(){
					//setup for next possible run
					leftClasses.incrementClass(combinedDataLabels.back().returnDataLabel());
					rightClasses.decrementClass(combinedDataLabels.back().returnDataLabel());
					
					assert(rightClasses.returnNumItems() == 0);
					std::swap(rightClasses, leftClasses);
					assert(leftClasses.returnNumItems() == 0);
				}

			public:
				fpSplit(const std::vector<int>& labelVector): labels(labelVector){
					rightClasses.findNumClasses(labels);
					leftClasses.setClassVecSize(rightClasses.returnClassVecSize());
					setCombinedVecSize();
					overallImpurity = rightClasses.calcAndReturnImpurity();
				}

				inline double returnImpurity(){
					return overallImpurity;
				}


				inline splitInfo<T> giniSplit(const std::vector<T>& featureVals, int featureNum){
					double tempImpurity;
					int numLabels = (int)labels.size();
				//	timeLogger logTime;

					// initialize return value
					splitInfo<T> currSplitInfo;

					// zip data and labels
					zipDataLabels(featureVals);

					// sort feature Vals
					pdqsort_branchless(combinedDataLabels.begin(), combinedDataLabels.end());
					//std::sort(combinedDataLabels.begin(), combinedDataLabels.end());

					// find split
					for(int i=0; i<numLabels-1; ++i){
						leftClasses.incrementClass(combinedDataLabels[i].returnDataLabel());
						rightClasses.decrementClass(combinedDataLabels[i].returnDataLabel());

						if(checkInequalityWithNext(i)){
							tempImpurity = leftClasses.calcAndReturnImpurity() + rightClasses.calcAndReturnImpurity();
							
							if(tempImpurity < currSplitInfo.returnImpurity() && tempImpurity != overallImpurity){
								currSplitInfo.setImpurity(tempImpurity);
								currSplitInfo.setSplitValue(midVal(i));
								currSplitInfo.setLeftImpurity(leftClasses.returnImpurity());
								currSplitInfo.setRightImpurity(rightClasses.returnImpurity());
					currSplitInfo.setFeatureNum(featureNum);
							}
						}
					}

					assert(currSplitInfo.returnImpurity() != overallImpurity);
					//logTime.startFindSplitTimer();
					setupForNextRun();
					//logTime.stopFindSplitTimer();

					return currSplitInfo;
				}
		};

}//namespace fp
#endif //fpSplit_h
