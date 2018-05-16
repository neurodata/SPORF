#include "fpReadCSV.h"
#include "fpDataSet.h"
#include "fpDetermineOOB.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
	const std::string csvFileName = "test/res/testCSV.csv";
try{

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



}catch(std::exception& e){
	std::cout << "standard error: " << e.what() << std::endl;
}

}
