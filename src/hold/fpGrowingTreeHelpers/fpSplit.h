#ifndef split_h
#define split_h

#include <vector>
#include "fpClassTotals.h"
#include "growingTreeElement.h"
#include "fpSplitInfo.h"

namespace fp {


template<typename T>
		class fpSplit{
			protected:
				double overallImpurity;
				
				fpClassTotals leftClasses;
				fpClassTotals rightClasses;

				std::vector<int> labels;
				std::vector< growingTreeElement<T> > combinedDataLabels;

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
					//TODO ensure rightClasses is zero at this point.

					rightClasses = leftClasses;
					leftClasses.resetClassTotals();
				}

			public:
				fpSplit(const std::vector<int>& labelVector): labels(labelVector){
					rightClasses.findNumClasses(labels);
					leftClasses.setClassVecSize(rightClasses.returnClassVecSize());
					this->setCombinedVecSize();
					overallImpurity = rightClasses.calcAndReturnImpurity();
				}

				inline double returnImpurity(){
					return overallImpurity;
				}


				fpSplitInfo<T> giniSplit(const std::vector<T>& featureVals, int featureNum){
					double tempImpurity;
					int numLabels = labels.size();
				//	timeLogger logTime;

					// initialize return value
					fpSplitInfo<T> currSplitInfo;
					currSplitInfo.setFeatureNum(featureNum);

					// zip data and labels
					zipDataLabels(featureVals);

					// sort feature Vals
					std::sort(combinedDataLabels.begin(), combinedDataLabels.end());

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
							}
						}
					}

					if(currSplitInfo.returnImpurity() == overallImpurity){
						std::cout << "it happened\n";
exit(1); //should never happen.  Why?
								currSplitInfo.setImpurity(-1);
					}
					//logTime.startFindSplitTimer();
					setupForNextRun();
					//logTime.stopFindSplitTimer();

					return currSplitInfo;
				}
		
		};

} //namespace
#endif //split
