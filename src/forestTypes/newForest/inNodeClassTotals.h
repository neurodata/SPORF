#ifndef inNodeClassTotals_h
#define inNodeClassTotals_h

#include <vector>
#include "obsIndexAndClassTuple.h"
#include <iostream>

namespace fp{

	template<typename T>
	class inNodeClassTotals{
		protected:
			int maxClass;
			int totalNumObj;
			float impurity;
			std::vector<int> classVec;

		public:
			inNodeClassTotals() : maxClass(-1), totalNumObj(0), impurity(-1){}

			inline int returnLargestClass(){
				int largestClass=0; 
				for(int i = 1; i <= maxClass; ++i){
					if(classVec[i] > classVec[largestClass]){
						largestClass = i;
					}
				}
				return largestClass;
			}


			inline void findClassCardinalities(std::vector<obsIndexAndClassTuple<T> >& observations){
				for(int i = 0; i < (int)observations.size(); ++i){
					if(observations[i].returnClassOfObs() > maxClass){
						maxClass = observations[i].returnClassOfObs();
						classVec.resize(maxClass+1);//+1 because class labels start at 0.
					}
					++classVec[observations[i].returnClassOfObs()];
					++totalNumObj;
				}
			}


			inline int returnNumItems(){
				return totalNumObj;
			}

			inline float returnImpurity(){
				return impurity;
			}


			inline float calcAndReturnImpurity(){
				if(false){ //use gini impurity
					int sumClassTotalsSquared = 0;
					for(auto i : classVec){
						sumClassTotalsSquared+=i*i;
					}
					return 1.0-double(sumClassTotalsSquared)/double(totalNumObj*totalNumObj);
				}else{ //use what is this?
					impurity = 0.0;
					float classPercent;
					for(auto i : classVec){
						classPercent = float(i)/float(totalNumObj);

						impurity += float(i)*(1.0-classPercent);
					}
					return impurity;
				}
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

			inline void copyInNodeClassTotals(inNodeClassTotals nodeData){
				maxClass = nodeData.maxClass;
				totalNumObj = nodeData.totalNumObj;
				impurity = nodeData.impurity;
				classVec = nodeData.classVec;
			}

inline void copyProperties(inNodeClassTotals nodeData){
				maxClass = nodeData.maxClass;
				classVec.resize(maxClass+1);//+1 because class labels start at 0.
			}

	};


}//namespace fp
#endif //inNodeClassTotals_h
