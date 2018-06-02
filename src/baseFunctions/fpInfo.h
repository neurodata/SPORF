#ifndef fpInfo_h
#define fpInfo_h

#include <string>
#include <memory>
#include <math.h>

namespace fp {

	class fpInfo{

		protected:
			// forest training information
			int numTreesInForest;
			int minParent = 1;
			int numClasses = -1;
			int numFeatures = -1;
			int mtry = -1;
			double fractionOfFeaturesToTest = -1.0;

			// forest trained characteristics
			int numberOfNodes;
			int maxDepth;
			int sumLeafNodeDepths;

			// The forest


		public:

			fpInfo();

			void setParameter(const std::string& parameterName, const std::string& parameterValue);

			void setParameter(const std::string& parameterName, const double parameterValue);

			void setParameter(const std::string& parameterName, const int parameterValue);

			void printAllParameters();

			void setNumClasses(int numC){
				numClasses = numC;
			}

			void setNumFeatures(int numF){
				numFeatures = numF;
			}

			int returnNumTrees(){
return numTreesInForest;
			}

			bool useDefaultMTRY(){
				return fractionOfFeaturesToTest == -1;
			}

			void setMTRY(){
				if(useDefaultMTRY()){
					mtry = sqrt(numFeatures);
				}else{
					mtry = fractionOfFeaturesToTest * numFeatures;
				}
			}

	}; // class fpInfo
} //namespace fp
#endif //fpInfo.h
