#ifndef inNodeClassTotals_h
#define inNodeClassTotals_h

#include <vector>
#include "nodeIterators.h"
#include <assert.h>
#include <iostream>

namespace fp{

	class inNodeClassTotals{
		protected:
			int totalNumObj;
			float impurity;
			std::vector<int> classVec;

		public:
			//TODO: This constructor will never be used.  Delete here and in tests.
			inNodeClassTotals(nodeIterators& observationIterators) : totalNumObj(0), impurity(-1){
				setClassCardinalities(observationIterators);
				setTotalNumObj();
			}

			inNodeClassTotals(int numClasses) : totalNumObj(0), impurity(-1){
				classVec.resize(numClasses);
			}

			inNodeClassTotals() : totalNumObj(0), impurity(-1){
				//used for root nodes
			}



			inline void setClassCardinalities(nodeIterators& observationIterators){
				observationIterators.setVecOfClassSizes(classVec);
			}

			inline void setTotalNumObj(){
				for(auto i : classVec){
					assert(i >= 0);
					totalNumObj+=i;
				}
			}


			//TODO: write test for this function
			inline void setupClassTotals(nodeIterators& observationIterators){
				setClassCardinalities(observationIterators);
				setTotalNumObj();
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
					return 1.0-float(sumClassTotalsSquared)/float(totalNumObj*totalNumObj);
				}else{ //use what is this?
					impurity = 0.0;
					float classPercent;
					for(auto i : classVec){
						classPercent = (float)i/(float)totalNumObj;

						impurity += (float)i*(1.0-classPercent);
					}
					return impurity;
				}
			}


			inline void decrementClass(int classNum){
				--classVec[classNum];
				--totalNumObj;
				assert(totalNumObj >=0);
			}


			inline void incrementClass(int classNum){
				++classVec[classNum];
				++totalNumObj;
				assert(totalNumObj >=0);
			}

			inline void resetClassTotals(){
				std::fill(classVec.begin(), classVec.end(), 0);
				totalNumObj=0;
			}

			inline void copyInNodeClassTotals(inNodeClassTotals nodeData){
				//maxClass = nodeData.maxClass;
				totalNumObj = nodeData.totalNumObj;
				//impurity = nodeData.impurity;
				classVec = nodeData.classVec;
			}

			inline bool isNodePure(){
				return impurity==0.0;
			}

			inline int returnMaxClass(){
				int largestClassSize = classVec[0];
				int largestClass = 0;
				for(int i = 1; i < (int)classVec.size(); ++i){
					if(classVec[i] > largestClassSize){
						largestClassSize = classVec[i];
						largestClass = i;
					}
				}
				return largestClass;
			}

			inline void setClassPercentages(std::vector<float>& classPercentages){
				for(auto i : classVec){
					classPercentages.push_back((float)i/(float)totalNumObj);
				}
			}

			inline bool isSizeLTMinParent(int minParent){
				return totalNumObj <= minParent;
			}
	};

}//namespace fp
#endif //inNodeClassTotals_h
