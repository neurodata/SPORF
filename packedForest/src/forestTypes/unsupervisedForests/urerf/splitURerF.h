#ifndef splitURerF_h
#define splitURerF_h

#include "splitURerFInfo.h"
#include "../../../baseFunctions/pdqsort.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <assert.h>
#include <utility>
namespace fp{
	template<typename T>
		class splitURerF{
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
				inline splitURerFInfo<T> twoMeanSplit(const std::vector<T>& featureVal, const std::vector<int>& featureNums){
                                        
					// initialize return value
					splitURerFInfo<T> currSplitInfo;
					createData(featureVal);

					// sort feature Vals
					std::vector<T> errVecLeft;
					std::vector<T> errVecRight;
					auto pbegin = featureValsVec.begin();
					auto pend = featureValsVec.end();
					std::sort(featureValsVec.begin(), featureValsVec.end());
					pbegin = featureValsVec.begin();
					pend = featureValsVec.end();
					int sizeX = featureValsVec.size();
					featureValsVec.erase(std::remove(pbegin, pend, 0), pend);
					int sizeNNZ = featureValsVec.size();
					int sizeZ = sizeX - sizeNNZ;
					T meanRight, sumLeft=0, meanLeft, cutPoint=0;
					T errCurr = 0;
					T minErr = std::numeric_limits<T>::infinity();
					T minErrLeft = std::numeric_limits<T>::infinity();
					T minErrRight = std::numeric_limits<T>::infinity();
					T sumRight = std::accumulate(featureValsVec.begin(), featureValsVec.end(), 0.0);
					pbegin = featureValsVec.begin();
					pend = featureValsVec.end();
					std::vector<T> errVec(pbegin, pend);
					
					if (sizeNNZ - 1 <= 0){
						currSplitInfo.setImpurity(-1);
						currSplitInfo.addFeatureNums(featureNums);
						currSplitInfo.setSplitValue(0);
						currSplitInfo.setLeftImpurity(0);
                                                currSplitInfo.setRightImpurity(0);
						return currSplitInfo;
					}
					if( fabs(featureValsVec[0] - featureValsVec[sizeX-1])<0.00001){
						currSplitInfo.setImpurity(-1);
						currSplitInfo.addFeatureNums(featureNums);
						currSplitInfo.setSplitValue(0);
						currSplitInfo.setLeftImpurity(0);
						currSplitInfo.setRightImpurity(0);
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
							int leftSize = sizeZ + index;
							int rightSize = sizeNNZ - index;
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
					currSplitInfo.setSplitValue(cutPoint);
					currSplitInfo.setLeftImpurity(minErrLeft);
					currSplitInfo.setRightImpurity(minErrRight);
					currSplitInfo.addFeatureNums(featureNums);	
					return currSplitInfo;
				}
		};

}//namespace fp
#endif //splitRerF_h
