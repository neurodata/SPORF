#include "fpInfo.h"
#include <math.h>
#include <iostream>

namespace fp {

	fpInfo::fpInfo(): numTreesInForest(100),
	minParent(1),	numClasses(-1), numObservations(-1), numFeatures(-1),
	mtry(-1), columnWithY(-1), 
	numberOfNodes(0), maxDepth(0),sumLeafNodeDepths(0), fractionOfFeaturesToTest(-1.0), binSize(0), numCores(1){}


	void fpInfo::setParameter(const std::string& parameterName, const std::string& parameterValue){
		if(parameterName == "forestType"){
			forestType = parameterValue;
		}else if(parameterName == "CSVFileName"){
			CSVFileName = parameterValue;
		}else{
			throw std::runtime_error("Unknown parameter type.(string)");
		}
	}


	void fpInfo::setParameter(const std::string& parameterName, const double parameterValue){
		if(parameterName == "numTreesInForest"){
			numTreesInForest = (int)parameterValue;
		}else if(parameterName == "minParent"){
			minParent = (int)parameterValue;
		}else if(parameterName == "numClasses"){
			numClasses = (int)parameterValue;
		}else if(parameterName == "mtry"){
			mtry = (int)parameterValue;
		}else if(parameterName == "fractionOfFeaturesToTest"){
			fractionOfFeaturesToTest = parameterValue;
		}else if(parameterName == "columnWithY"){
			columnWithY = (int)parameterValue;
		}else if(parameterName == "useBinning"){
			binSize = (int)parameterValue;
		}else if(parameterName == "numCores"){
			numCores = (int)parameterValue;
		}else {
			throw std::runtime_error("Unknown parameter type.(double)");
		}
	}


	void fpInfo::setParameter(const std::string& parameterName, const int parameterValue){
		if(parameterName == "numTreesInForest"){
			numTreesInForest = parameterValue;
		}else if(parameterName == "minParent"){
			minParent = parameterValue;
		}else if(parameterName == "numClasses"){
			numClasses = parameterValue;
		}else if(parameterName == "mtry"){
			mtry = parameterValue;
		}else if(parameterName == "fractionOfFeaturesToTest"){
			fractionOfFeaturesToTest = (double)parameterValue;
		}else if(parameterName == "columnWithY"){
			columnWithY = parameterValue;
		}else if(parameterName == "useBinning"){
			binSize = parameterValue;
		}else if(parameterName == "numCores"){
			numCores = parameterValue;
		}else {
			throw std::runtime_error("Unknown parameter type.(int)");
		}
	}

	void fpInfo::printAllParameters(){
		std::cout << "numTreesInForest -> " << numTreesInForest << "\n";
		std::cout << "minParent -> " << minParent << "\n";
		std::cout << "numClasses -> " << numClasses << "\n";
		std::cout << "numObservations -> " << numObservations << "\n";
		std::cout << "numFeatures -> " << numFeatures << "\n";
		std::cout << "mtry -> " << mtry << "\n";
		std::cout << "fractionOfFeaturesToTest -> " << fractionOfFeaturesToTest << "\n";
		std::cout << "CSV file name -> " <<  CSVFileName << "\n";
		std::cout << "columnWithY -> " << columnWithY << "\n";
		std::cout << "Type of Forest -> " << forestType << "\n";
		std::cout << "binSize -> " << binSize << "\n";
		std::cout << "numCores -> " << numCores << "\n";
	}


	void fpInfo::printForestType(){
		std::cout << forestType;
	}

}//namespace fp
