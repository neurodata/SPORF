#ifndef fpClassTotals_h
#define fpClassTotals_h

#include <vector>

namespace fp{

class fpClassTotals{
		protected:
			int maxClass;
			int totalNumObj;
			double impurity;
			std::vector<float> classVec;

		public:
			fpClassTotals() : maxClass(-1), totalNumObj(0), impurity(-1){}

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

			int returnNumItems(){
				return totalNumObj;
			}

			double returnImpurity(){
				return impurity;
			}

			inline void setClassVecSize(int newSize){
				classVec.resize(newSize);
				std::fill(classVec.begin(), classVec.end(), 0.0);
			}

			inline int returnClassVecSize(){
				return classVec.size();
			}

			inline double calcAndReturnImpurity(){
				if(false){ //use gini impurity
					int sumClassTotalsSquared = 0;
					for(auto i : classVec){
						sumClassTotalsSquared+=i*i;
					}
					return 1.0-double(sumClassTotalsSquared)/double(totalNumObj*totalNumObj);
				}else{ //use what is this?
					double impSum = 0;
					double classPercent;
					for(auto i : classVec){
						classPercent = double(i)/double(totalNumObj);

						impSum += double(i)*(1.0-classPercent);
					}
					return impSum;
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
	};

}//namespace fp
#endif //fpClassTotals
