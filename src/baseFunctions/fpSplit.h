#ifndef fpSplit_h
#define fpSplit_h
//#include <iostream>
//#include <random>
#include <vector>
#include <algorithm>

namespace fp{

	template<T>
		class splitInfo
		{
			protected:
				T splitValue;
				float gini;
				int featureNum;

			public:
				inline void setSplitValue(T sVal){
					splitValue = sVal;
				}
				inline T returnSplitValue(){
					return splitValue;
				}

				inline void setGini(float gVal){
					gini = gVal;
				}
				inline float returnGiniValue(){
					return gini;
				}

				inline void setFeatureNum(float fVal){
					gini = fVal;
				}
				inline int returnFeatureNum(){
					return featureNum;
				}
		};

	template<T>
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

				inline T midVal (const labeledData<T>& otherData) const
				{
					return (dataElement + otherData.dataElement)/2;
				}

				void setPair(T dElement, int dLab){
					dataElement = dElement;
					dataLabel = dLab;
				}
		};

	template<T>
		class split{
			protected:
				int maxClass;
				std::vector<int> leftClasses;
				std::vector<int> rightClasses;

				std::vector<T> featureVals;
				std::vector<int> labels;

				std::vector< labeledData<T> > combinedDataLabels;

			public:
				split():maxClass(0){}

				void setCombinedVecSize(){
combinedDataLabels.resize(labels.size());
				}

				void findNumClasses(){
					rightClasses.resize(1);
					for(auto i : labels){
						if(i>maxClass){
maxClass = i;
					rightClasses.resize(i);
						}
++rightClasses[i];
					}
					leftClasses.resize(maxClass);
				}


				splitInfo<T> giniSplit(){

					// initialize return value
					splitInfo<T> currSplitInfo;

					// zip data and labels
					for(unsigned i : indices(labels)){
						combinedDataLabels[i].setPair(featureVals[i],labels[i]);
					}

					// sort feature Vals
					std::sort(combinedDataLabels.begin(), combinedDataLabels.end());

					// setup left and right sides

					return currSplitInfo;
				}

		};

}//namespace fp
#endif //fpSplit_h
