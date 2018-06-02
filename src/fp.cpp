#include "baseFunctions/fpUtils.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
	const std::string csvFileName = "test/res/testCSV.csv";
	try{
		fp::fpForest forest;
		forest.setParameter("forestType", "rfBase");
		forest.setParameter("numTreesInForest", 2);
		forest.setParameter("columnWithY", 1);
		forest.setParameter("minParent", 4);
		forest.setParameter("numClasses", 5);
		forest.setParameter("mtry", 6);
		forest.setParameter("CSVFileName", "test/res/testCSV.csv");
	
	forest.growForest();

		forest.printParameters();
		forest.printType();
		/*
		rankedInput<double> inputDataOld(csvFileName);

		std::cout << "0,1 " << inputDataOld.returnFeatureValue(0,1) << std::endl;
		std::cout << "2,2 " << inputDataOld.returnFeatureValue(2,2) << std::endl;
		inputDataOld.printCSVStats();
		inputDataOld.printTableOfValues();

		OOB test(10);
		test.printOOB();

		//double is the data type of X and int is the datatype of Y.
		inputData<double,int> data(csvFileName,2-1);
		data.printXValues();
		data.printYValues();
*/
	}catch(std::exception& e){
		std::cout << "standard error: " << e.what() << std::endl;
	}

}
