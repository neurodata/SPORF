#include "baseFunctions/fpUtils.h"
#include "fpSingleton/fpSingleton.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
	//const std::string csvFileName = "test/res/testCSV.csv";
//	const std::string csvFileName = "./src/iris.csv";
	try{
		fp::fpForest forest;
		forest.setParameter("forestType", "rfBase");
		forest.setParameter("numTreesInForest", 10);
		forest.setParameter("minParent", 1);
		forest.setParameter("numClasses", 5);
//		forest.setParameter("mtry", 100);
//		forest.setParameter("CSVFileName", "test/res/testCSV.csv");
//		forest.setParameter("CSVFileName", "res/iris.csv");
//		forest.setParameter("columnWithY", 4);
		forest.setParameter("CSVFileName", "res/mnist.csv");
		forest.setParameter("columnWithY", 0);

		forest.growForest();

		forest.printParameters();
		forest.printForestType();

		std::cout << "error: " << forest.testAccuracy() << "\n";

	}catch(std::exception& e){
		std::cout << "standard error: " << e.what() << std::endl;
	}

}
