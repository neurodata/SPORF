#ifndef forestConv_h
#define forestConv_h

#include "packedForest.h"
#include <stdexcept>
//#include <Rcpp.h> 
//#include <Rinternals.h>
#include <string>
#include <iostream>
#include <vector>

//using namespace fp;
//using namespace Rcpp;

template <typename T>
class RForest{

	protected:

	public:
		RForest(){
			std::cout << "created RForest\n";	
		}

		fp::fpForest<T> forest;

		inline void setParameterString(const std::string& parameterName, const std::string& parameterValue){
			forest.setParameter(parameterName, parameterValue);
		}

		inline void setParameterInt(const std::string& parameterName, int parameterValue){
			forest.setParameter(parameterName, parameterValue);
		}

		inline void setParameterDouble(const std::string& parameterName, double parameterValue){
			forest.setParameter(parameterName, parameterValue);
		}


		inline void growForestCSV(){
			forest.growForest();
		}

		/*
			 inline void growForestGivenX(const void* mat, int numObs, int numFeatures){
			 double* matP = (double*)mat + 5;
//double* matP = (double*)mat;
for(int i = 0; i < numObs*numFeatures; ++i){
std::cout << matP[i] << " ";
}
}
*/

inline void printParameters(){
	forest.printParameters();
}

inline void printForestType(){
	forest.printForestType();
}

inline float testAccuracy(){
	return forest.testAccuracy();
}

inline int predict(){
	std::cout << "predict\n";
	return 1;
}
};

#endif //forestConv_h

