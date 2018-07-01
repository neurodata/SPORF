#ifndef fpSplit_h
#define fpSplit_h

#include "timeLogger.h"
#include <iostream>
#include <vector>
#include <algorithm>

namespace fp{

	template<typename T>
		class splitInfo
		{
			protected:
				float impurity;
				float leftImpurity;
				float rightImpurity;
				int featureNum;
				T splitValue;

			public:
				splitInfo(): impurity(3), leftImpurity(-1), rightImpurity(-1), featureNum(-1), splitValue(0){}

				inline void setSplitValue(T sVal){
					splitValue = sVal;
				}

				inline T returnSplitValue(){
					return splitValue;
				}

				inline void setImpurity(float gVal){
					impurity = gVal;
				}
				inline float returnImpurity(){
					return impurity;
				}

				inline void setLeftImpurity(float gVal){
					impurity = gVal;
				}
				inline float returnLeftImpurity(){
					return leftImpurity;
				}

				inline void setRightImpurity(float gVal){
					impurity = gVal;
				}
				inline float returnRightImpurity(){
					return rightImpurity;
				}

				inline void setFeatureNum(int fNum){
					featureNum = fNum;
				}

				inline int returnFeatureNum(){
					return featureNum;
				}
		};


	template<typename T>
		class labeledData
		{
			protected:
				T dataElement;
				int dataLabel;

			public:

				inline bool operator < (const labeledData<T>& otherData) const
				{
					return dataElement < otherData.dataElement;
				}

				inline int returnDataLabel(){
					return dataLabel;
				}

				inline T returnDataElement(){
					return dataElement;
				}

				inline T midVal (const labeledData<T>& otherData) const
				{
					return (dataElement + otherData.dataElement)/2.0;
				}

				inline bool checkInequality(const labeledData<T>& otherData){
					return dataElement != otherData.dataElement;
				}

				void setPair(T dElement, int dLab){
					dataElement = dElement;
					dataLabel = dLab;
				}
		};


	class classTotals{
		protected:
			int maxClass;
			float totalNumObj;
			float impurity;
			std::vector<float> classVec;

		public:
			classTotals() : maxClass(-1), totalNumObj(0), impurity(-1){}

			inline int returnLargestClass(){
				int largestClass=-1; 
				int numInClass=0;
				for(int i = 0; i <= maxClass; ++i){
					if(classVec[i] > numInClass){
						numInClass = classVec[i];
						largestClass = i;
					}
				}

				return largestClass;
			}

			void findNumClasses(std::vector<int>& labs){
				for(auto i : labs){
					if(i>maxClass){
						maxClass = i;
						classVec.resize(i+1);
					}
					++classVec[i];
					++totalNumObj;
				}
			}

			float returnNumItems(){
				return totalNumObj;
			}

			float returnImpurity(){
				return impurity;
			}

			inline void setClassVecSize(int newSize){
				classVec.resize(newSize);
				std::fill(classVec.begin(), classVec.end(), 0.0);
			}

			inline int returnClassVecSize(){
				return classVec.size();
			}

			inline float calcAndReturnImpurity(){
				float currImpurity = 0.0;
				for(auto i : classVec){
					currImpurity+=float(i*i)/(totalNumObj*totalNumObj);
				}
				impurity = 1-currImpurity;
				return impurity;
			}

			inline void decrementClass(int classNum){
				--classVec[classNum];
				--totalNumObj;
			}

			inline void incrementClass(int classNum){
				++classVec[classNum];
				++totalNumObj;
			}

			inline void resetClassTotals(){
				std::fill(classVec.begin(), classVec.end(), 0);
				totalNumObj=0;
			}
	};


	template<typename T>
		class split{
			protected:
				float overallImpurity;
				classTotals leftClasses;
				classTotals rightClasses;

				std::vector<int> labels;
				std::vector< labeledData<T> > combinedDataLabels;

				inline void setCombinedVecSize(){
					combinedDataLabels.resize(labels.size());
				}

				inline void zipDataLabels(std::vector<T> featureVals){
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
				split(std::vector<int>& labelVector): labels(labelVector){
					rightClasses.findNumClasses(labels);
					leftClasses.setClassVecSize(rightClasses.returnClassVecSize());
					this->setCombinedVecSize();
					overallImpurity = rightClasses.calcAndReturnImpurity();
				}

				inline float returnImpurity(){
					return overallImpurity;
				}



				splitInfo<T> giniSplit(const std::vector<T>& featureVals, int featureNum){
					float tempImpurity;
					int numLabels = labels.size();
					timeLogger logTime;

					logTime.startFindSplitTimer();
					// initialize return value
					splitInfo<T> currSplitInfo;
					currSplitInfo.setFeatureNum(featureNum);

					// zip data and labels
					zipDataLabels(featureVals);

					// sort feature Vals
					std::sort(combinedDataLabels.begin(), combinedDataLabels.end());

					logTime.startGiniTimer();
					// find split
					for(int i=0; i<numLabels-1; ++i){
						leftClasses.incrementClass(combinedDataLabels[i].returnDataLabel());
						rightClasses.decrementClass(combinedDataLabels[i].returnDataLabel());

						if(checkInequalityWithNext(i)){
							tempImpurity = leftClasses.calcAndReturnImpurity() + rightClasses.calcAndReturnImpurity();
							if(tempImpurity < currSplitInfo.returnImpurity()){
								currSplitInfo.setImpurity(tempImpurity);
								currSplitInfo.setSplitValue(midVal(i));
								currSplitInfo.setLeftImpurity(leftClasses.returnImpurity());
								currSplitInfo.setRightImpurity(rightClasses.returnImpurity());
							}

						}
					}
					logTime.stopGiniTimer();

					setupForNextRun();

					logTime.stopFindSplitTimer();
					return currSplitInfo;
				}

		};

}//namespace fp
#endif //fpSplit_h
