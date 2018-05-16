#include "fpReadCSV.h"
#include "fpDataSet.h"
#include "fpDetermineOOB.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
	const std::string csvFileName = "test/res/testCSV.csv";
try{

	rankedInput<double> inputData(csvFileName);

	std::cout << "0,1 " << inputData.returnFeatureValue(0,1) << std::endl;
	std::cout << "2,2 " << inputData.returnFeatureValue(2,2) << std::endl;
	inputData.printCSVStats();
	inputData.printTableOfValues();

	OOB test(10);
	test.printOOB();


}catch(std::exception& e){
	std::cout << "standard error: " << e.what() << std::endl;
}

}
