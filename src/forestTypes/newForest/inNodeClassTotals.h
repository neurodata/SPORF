#ifndef inNodeClassTotals_h
#define inNodeClassTotals_h

#include <vector>

namespace fp{

	class inNodeClassTotals{
		protected:
			int maxClass;
			int totalNumObj;
			double impurity;
			std::vector<int> classVec;

		public:
			inNodeClassTotals() : maxClass(-1), totalNumObj(0), impurity(-1){}

			inline int returnLargestClass(){
				int largestClass=-1; 
				int numInClass=-1;
				for(int i = 0; i <= maxClass; ++i){
					if(classVec[i] > numInClass){
						numInClass = classVec[i];
						largestClass = i;
					}
				}
				return largestClass;
			}


			void findClassCardinalities(std::vector<obsIndexAndClassTuple>::const_iterator begin,  std::vector<obsIndexAndClassTuple>::const_iterator end){
				for(auto i : labels){
					if(i.returnClassOfObs() > maxClass){
						maxClass = i;
						classVec.resize(i+1);//+1 because class labels start at 0.
					}
					++classVec[i.returnClassOfObs()];
					++totalNumObj;
				}
			}


			int returnNumItems(){
				return totalNumObj;
			}

			double returnImpurity(){
				return impurity;
			}

			/*
				 inline void setClassVecSize(int newSize){
				 classVec.resize(newSize);
				 std::fill(classVec.begin(), classVec.end(), 0.0);
				 }

				 inline int returnClassVecSize(){
				 return classVec.size();
				 }
				 */

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

			inline void copyProperties(inNodeClassTotals nodeData){
				maxClass = nodeData.maxClass;
				totalNumObj = nodeData.totalNumObj;
				impurity = nodeData.impurity;
				classVec = nodeData.classVec;
			}

	};


}//namespace fp
#endif //inNodeClassTotals_h
