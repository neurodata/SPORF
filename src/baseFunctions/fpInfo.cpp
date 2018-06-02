#include "fpInfo.h"
#include <math.h>
#include <iostream>

namespace fp {

	fpInfo::fpInfo(): numTreesInForest(100),
	minParent(1),	numClasses(-1), numFeatures(-1),
	mtry(-1), fractionOfFeaturesToTest(-1.0),
	numberOfNodes(0), maxDepth(0),sumLeafNodeDepths(0){}

//void fpInfo::print(){
//				forest->printForestType();
//			}

		//	void fpForest::growForest(){
		//	fpLoadData();


		//	}

	void fpInfo::setParameter(const std::string& parameterName, const std::string& parameterValue){
		if(false){
		;
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
		}else {
			throw std::runtime_error("Unknown parameter type.(int)");
		}
	}

void fpInfo::printAllParameters(){
		std::cout << "numTreesInForest -> " << numTreesInForest << "\n";
		std::cout << "minParent -> " << minParent << "\n";
		std::cout << "numClasses -> " << numClasses << "\n";
		std::cout << "numFeatures -> " << numFeatures << "\n";
		std::cout << "mtry -> " << mtry << "\n";
		std::cout << "fractionOfFeaturesToTest -> " << fractionOfFeaturesToTest << "\n";
	}
/*
	void fpInfo::setDataRelatedParameters(inputData<double, int>* data){
		numClasses = data->returnNumClasses();
		numFeatures = data->returnNumFeatures();
		if(fractionOfFeaturesToTest == -1){
			mtry = sqrt(numFeatures);
		}else{
			mtry = fractionOfFeaturesToTest * numFeatures;
		}
*/
	
}//namespace fp
