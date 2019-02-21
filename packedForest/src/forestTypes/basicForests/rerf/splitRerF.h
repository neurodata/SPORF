#ifndef splitRerF_h
#define splitRerF_h

#include "splitRerFInfo.h"
#include "../labeledData.h"
#include "../classTotals.h"
#include "../../../baseFunctions/pdqsort.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <assert.h>
#include <utility>

namespace fp{

	template<typename T>
		class splitRerF{
			protected:
				double overallImpurity;
				classTotals leftClasses;
				classTotals rightClasses;

				std::vector<int> labels;
				std::vector< labeledData<T> > combinedDataLabels;

				inline void setCombinedVecSize(){
					combinedDataLabels.resize(labels.size());
				}

				inline void zipDataLabels(std::vector<T> featureVals){
					for(int i=0; i<(int)labels.size(); ++i){
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
				splitRerF(const std::vector<int>& labelVector): labels(labelVector){
					rightClasses.findNumClasses(labels);
					leftClasses.setClassVecSize(rightClasses.returnClassVecSize());
					setCombinedVecSize();
					overallImpurity = rightClasses.calcAndReturnImpurity();
				}

				inline double returnImpurity(){
					return overallImpurity;
				}


				inline splitRerFInfo<T> giniSplit(const std::vector<T>& featureVals, const std::vector<int>& featureNums){
					double tempImpurity;
					int numLabels = (int)labels.size();
					//	timeLogger logTime;

					// initialize return value
					splitRerFInfo<T> currSplitInfo;

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
								currSplitInfo.addFeatureNums(featureNums);
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
#endif //splitRerF_h
