#ifndef fpInfo_h
#define fpInfo_h

#include <string>
#include <memory>
#include <math.h>

namespace fp {

	class fpInfo{

		protected:
			int numTreesInForest;
			int minParent = 1;
			int numClasses = -1;
			int numObservations = -1;
			int numFeatures = -1;
			int mtry = -1;
			int columnWithY = -1;

			int numberOfNodes;
			int maxDepth;
			int sumLeafNodeDepths;

			double fractionOfFeaturesToTest = -1.0;

			std::string forestType;
			std::string CSVFileName;

			// The forest


		public:
			fpInfo();

			void setParameter(const std::string& parameterName, const std::string& parameterValue);

			void setParameter(const std::string& parameterName, const double parameterValue);

			void setParameter(const std::string& parameterName, const int parameterValue);

			void printAllParameters();
			void printForestType();

			inline std::string& returnCSVFileName(){
return CSVFileName;
			}

inline std::string& returnForestType(){
		return forestType;
	}

			inline int returnColumnWithY(){
return columnWithY;
			}

inline int returnNumClasses(){
return numClasses;
			}

inline int returnMinParent(){
return minParent;
}

inline int returnNumFeatures(){
return numFeatures;
			}
inline int returnNumObservations(){
return numObservations;
			}

			inline void setNumClasses(int numC){
				numClasses = numC;
			}

			inline void setNumFeatures(int numF){
				numFeatures = numF;
			}

			inline void setNumObservations(int numO){
				numObservations = numO;
			}

			inline int returnNumTrees(){
return numTreesInForest;
			}

			inline bool loadDataFromCSV(){
				if(!CSVFileName.empty() || columnWithY != -1){
					return true;
				}
				return false;
			}

inline int returnMtry(){
return mtry;
}
			inline bool useDefaultMTRY(){
				return fractionOfFeaturesToTest == -1;
			}

			inline void setMTRY(){
				if(mtry == -1){
				if(useDefaultMTRY()){
					mtry = sqrt(numFeatures);
				}else{
					mtry = fractionOfFeaturesToTest * numFeatures;
				}
				}
			}

	}; // class fpInfo
} //namespace fp
#endif //fpInfo.h
