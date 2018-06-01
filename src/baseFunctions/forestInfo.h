#ifndef forestInfo_h
#define forestInfo_h

#include "fpUtils.h"
#include "../rfClassification/fpForestClassificationBase.h"
#include <string>
#include <iostream>
#include <math.h>

namespace fpForest {

	// data input
	std::string forestCSVFileName;
	int columnWithY = -1;
	inputData<double, int>* fpData;

	// forest training information
	int numTreesInForest = 100;
	int minParent = 1;
	int numClasses = -1;
	int numFeatures = -1;
	int mtry = -1;
	double fractionOfFeaturesToTest = -1.0;

	// forest trained characteristics
	int numberOfNodes;
	int maxDepth;
	int sumLeafNodes;

	// The forest
	fpForestBase* forest;
//	fpForestClassificationBase<double> forest;


	// namespace functions


	void setParameter(const std::string& parameterName, const std::string& parameterValue){
		if(parameterName == "CSVFileName"){
			forestCSVFileName = parameterValue;
		}else{
			throw std::runtime_error("Unknown parameter type.(string)");
		}
	}


	void setParameter(const std::string& parameterName, const double parameterValue){
		if(parameterName == "numTreesInForest"){
			numTreesInForest = (int)parameterValue;
		}else if(parameterName == "columnWithY"){
			columnWithY = (int)parameterValue;
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


	void setParameter(const std::string& parameterName, const int parameterValue){
		if(parameterName == "numTreesInForest"){
			numTreesInForest = parameterValue;
		}else if(parameterName == "columnWithY"){
			columnWithY = parameterValue;
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


	void printAllParameters(){
		std::cout << "numTreesInForest -> " << numTreesInForest << "\n";
		std::cout << "CSV file name -> " <<  forestCSVFileName << "\n";
		std::cout << "columnWithY -> " << columnWithY << "\n";
		std::cout << "minParent -> " << minParent << "\n";
		std::cout << "numClasses -> " << numClasses << "\n";
		std::cout << "numFeatures -> " << numFeatures << "\n";
		std::cout << "mtry -> " << mtry << "\n";
		std::cout << "fractionOfFeaturesToTest -> " << fractionOfFeaturesToTest << "\n";
	}

	void setDataRelatedParameters(inputData<double, int>* data){
		numClasses = data->returnNumClasses();
		numFeatures = data->returnNumFeatures();
		if(fractionOfFeaturesToTest == -1){
			mtry = sqrt(numFeatures);
		}else{
			mtry = fractionOfFeaturesToTest * numFeatures;
		}
	}

	void fpLoadData(){
		if(!forestCSVFileName.empty() || columnWithY != -1){
			fpData = new inputData<double,int>(forestCSVFileName, columnWithY);
		}else {
			throw std::runtime_error("Unable to read data." );
		}
		setDataRelatedParameters(fpData);
	}






}
#endif //forestInfo.h
