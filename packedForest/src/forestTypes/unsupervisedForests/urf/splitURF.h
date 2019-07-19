#ifndef splitURF_h
#define splitURF_h

#include "splitURFInfo.h"
#include "../../../baseFunctions/pdqsort.h"
#include "../../../baseFunctions/timeLogger.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <assert.h>
#include <utility>
namespace fp{
	template<typename T>
		class splitURF{
			protected:
				std::vector<T> featureValsVec;

				static T computeSampleVariance(const double mean, const std::vector<T>& v){
					double accum = 0.0;
					std::for_each (std::begin(v), std::end(v), [&](const double d) {
    						accum += (d - mean) * (d - mean);
					});
    					return accum;
				}

				inline void createData(const std::vector<T> featureVals){
					featureValsVec.clear();
					auto siz_vec = featureVals.size();
					for(unsigned int i=0; i<siz_vec; ++i){
						featureValsVec.push_back(featureVals.at(i));
					}
				}

			public:
				inline splitURFInfo<T> twoMeanSplit(const std::vector<T>& featureVal, int featureNum){
                                        // initialize return value
                                        splitURFInfo<T> currSplitInfo;
                                        createData(featureVal);

                                        // sort feature Vals
                                        std::vector<T> errVecLeft;
                                        std::vector<T> errVecRight;
                                        auto pbegin = featureValsVec.begin();
					auto pend = featureValsVec.end();
					
					//std::sort(featureValsVec.begin(), featureValsVec.end());
                                        int sizeX = featureValsVec.size();
					pdqsort_branchless(featureValsVec.begin(), featureValsVec.end());
				        if(featureValsVec[0] == featureValsVec[sizeX-1]){
						currSplitInfo.setImpurity(-1);
				                currSplitInfo.setFeatureNums(featureNum);
                                                currSplitInfo.setSplitValue(0);
                                                currSplitInfo.setLeftImpurity(0);
                                                currSplitInfo.setRightImpurity(0);
						return currSplitInfo;
					}
                                        pbegin = featureValsVec.begin();
					pend = featureValsVec.end();
					featureValsVec.erase(std::remove(featureValsVec.begin(), featureValsVec.end(), 0), featureValsVec.end());
                                        int sizeNNZ = featureValsVec.size();
                                        int sizeZ = sizeX - sizeNNZ;
					int leftSize;
					int rightSize;
                                        T meanRight, sumLeft=0, meanLeft=0, cutPoint=0;
                                        T minErr = std::numeric_limits<T>::infinity();
					T errCurr=0; 
                                        T minErrLeft = std::numeric_limits<T>::infinity();
                                        T minErrRight = std::numeric_limits<T>::infinity();
					T sumRight = std::accumulate(featureValsVec.begin(), featureValsVec.end(), 0.0);
					pbegin = featureValsVec.begin();
					pend = featureValsVec.end();
                                        std::vector<T> errVec(pbegin, pend);

					if (sizeNNZ - 1 <= 0){
						return currSplitInfo;
					}


					if (sizeZ) {
                                                        meanRight = sumRight / (T)sizeNNZ;
                                                        minErr = computeSampleVariance(meanRight, errVec);
							cutPoint = featureValsVec.at(0) / 2;
                                          }


                                          if (sizeNNZ - 1) {
                                                        int index = 1;
							int sizeIt = featureValsVec.size()-1;
                                                        for(int iter = 0; iter < sizeIt; ++iter) {
                                                                leftSize = sizeZ + index;
                                                                rightSize = sizeNNZ - index;
                                                                sumLeft = sumLeft + featureValsVec[iter];
                                                                sumRight = sumRight - featureValsVec[iter];
                                                                meanLeft = sumLeft / (double)leftSize;
                                                                meanRight = sumRight / (double)rightSize;
								auto last = pbegin;
								std::advance(last, index);
								std::vector<T> newVec(pbegin, last);
								auto errLeft = computeSampleVariance(meanLeft, newVec) + (sizeZ * meanLeft * meanLeft);
								std::vector<T> newVec2(last, pend);
								auto errRight = computeSampleVariance(meanRight, newVec2);
                                                        	errCurr = errLeft + errRight;
                                                                if (errCurr < minErr) {
                                                                        cutPoint = (featureValsVec[iter] + featureValsVec[iter+1]) / 2;
                                                                        minErrLeft = errLeft;
                                                                        minErrRight = errRight;
                                                                        minErr = errCurr;
                                                                }
                                                                ++index;
                                                        }
                                          }
						currSplitInfo.setImpurity(minErr);
                                          	currSplitInfo.setFeatureNums(featureNum);	
                                          	currSplitInfo.setSplitValue(cutPoint);
                                          	currSplitInfo.setLeftImpurity(minErrLeft);
                                          	currSplitInfo.setRightImpurity(minErrRight);
					
							 
					  return currSplitInfo;
				}
		};

}//namespace fp
#endif //splitURF_h
