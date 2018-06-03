#ifndef fpSplit_h
#define fpSplit_h

#include <iostream>
#include <vector>
#include <algorithm>

namespace fp{

	template<typename T>
		class splitInfo
		{
			protected:
				T splitValue;
				float impurity;
				int leftSize;

			public:
				splitInfo(): impurity(2){}
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

inline void setLeftSize(int lSize){
					leftSize = lSize;
				}

				inline float returnLeftSize(){
					return leftSize;
				}
		};


	template<typename T>
		class labeledData
		{
			protected:
				T dataElement;
				int dataLabel;

			public:
				//labeledData(T dataE, int dataL): dataElement(dataE), dataLabel(dataL){}

				bool operator < (const labeledData<T>& otherData) const
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
			std::vector<float> classVec;

		public:
			classTotals() : maxClass(-1), totalNumObj(0){}

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

			inline void setClassVecSize(int newSize){
				classVec.resize(newSize);
				std::fill(classVec.begin(), classVec.end(), 0.0);
			}

			inline int returnClassVecSize(){
				return classVec.size();
			}

			inline float returnImpurity(){
				float impurity = 0.0;
				for(auto i : classVec){
					impurity+=(i*i)/(totalNumObj*totalNumObj);
				}
				return 1-impurity;
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

			public:
				split(std::vector<int>& labelVector): labels(labelVector){
					rightClasses.findNumClasses(labels);
					leftClasses.setClassVecSize(rightClasses.returnClassVecSize());
					this->setCombinedVecSize();
					overallImpurity = rightClasses.returnImpurity();
				}

				void setCombinedVecSize(){
					combinedDataLabels.resize(labels.size());
				}

				float returnImpurity(){
					return overallImpurity;
				}

				void zipDataLabels(std::vector<T>& featureVals){
					for(unsigned int i=0; i<labels.size(); ++i){
						combinedDataLabels[i].setPair(featureVals[i],labels[i]);
					}
				}

				T midVal(int leftIndex){
					return combinedDataLabels[leftIndex].midVal(combinedDataLabels[leftIndex+1]);
				}

				bool checkInequalityWithNext(int leftIndex){
					return combinedDataLabels[leftIndex].checkInequality(combinedDataLabels[leftIndex+1]);
				}

				splitInfo<T> giniSplit(std::vector<T>& featureVals){
					float tempImpurity;
					int numLabels = labels.size();


					// initialize return value
					splitInfo<T> currSplitInfo;

					// zip data and labels
					zipDataLabels(featureVals);

					// sort feature Vals
					std::sort(combinedDataLabels.begin(), combinedDataLabels.end());

					for(int i=0; i<numLabels-1; ++i){
						leftClasses.incrementClass(combinedDataLabels[i].returnDataLabel());
						rightClasses.decrementClass(combinedDataLabels[i].returnDataLabel());


						if(checkInequalityWithNext(i)){
							tempImpurity = leftClasses.returnImpurity() + rightClasses.returnImpurity();
							if(tempImpurity < currSplitInfo.returnImpurity()){
								currSplitInfo.setImpurity(tempImpurity);
								currSplitInfo.setSplitValue(midVal(i));
								currSplitInfo.setLeftSize(i+1);
							}
						}
					}

					//setup for next possible run
					leftClasses.incrementClass(combinedDataLabels.back().returnDataLabel());
					rightClasses.decrementClass(combinedDataLabels.back().returnDataLabel());

					rightClasses = leftClasses;
					leftClasses.resetClassTotals();

					return currSplitInfo;
				}

		};

}//namespace fp
#endif //fpSplit_h
