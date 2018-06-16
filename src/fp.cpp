#include "baseFunctions/fpUtils.h"
#include "fpSingleton/fpSingleton.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
	//const std::string csvFileName = "test/res/testCSV.csv";
	const std::string csvFileName = "./src/iris.csv";
	try{
		fp::fpForest forest;
		forest.setParameter("forestType", "rfBase");
		forest.setParameter("numTreesInForest", 10);
		forest.setParameter("columnWithY", 4);
		forest.setParameter("minParent", 4);
		forest.setParameter("numClasses", 5);
		forest.setParameter("mtry", 6);
		//forest.setParameter("CSVFileName", "test/res/testCSV.csv");
		forest.setParameter("CSVFileName", "src/iris.csv");

		forest.growForest();

		forest.printParameters();
		forest.printForestType();

	}catch(std::exception& e){
		std::cout << "standard error: " << e.what() << std::endl;
	}

}
