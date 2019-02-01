#include "packedForest.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
	if (argc != 4) return -1;
	int alg = atoi(argv[1]);
	int dataSet = atoi(argv[2]);
	int numCores = atoi(argv[3]);

//		 fp::timeLogger logTime;
	/*
		 logTime.startFindSplitTimer();
		 fp::inNodeClassIndices test(100000000);
		 logTime.stopFindSplitTimer();

		 logTime.startSortTimer();
		 fp::stratifiedInNodeClassIndices testw(100000000);
		 logTime.stopSortTimer();

		 logTime.printGrowTime();
		 */	
	try{
		fp::fpForest<double> forest;

		switch(alg){
			case 1:
				forest.setParamString("forestType", "rerf");
				break;
			case 2:
				forest.setParamString("forestType", "rfBase");
				break;
			case 3:
				forest.setParamString("forestType", "rerf");
				forest.setParamInt("useBinning", 1000);
				break;
			case 4:
				forest.setParamString("forestType", "rfBase");
				forest.setParamInt("useBinning", 1000);
				break;
case 5:
				forest.setParamString("forestType", "inPlace");
				break;
case 6:
				forest.setParamString("forestType", "inPlaceRerF");
				break;

			default:
				std::cout << "unknown alg selected" << std::endl;
				return -1;
				break;
		}


		switch(dataSet){
			case 1: 
				forest.setParamString("CSVFileName", "res/iris.csv");
				forest.setParamInt("columnWithY", 4);
				break;
			case 2:
				forest.setParamString("CSVFileName", "res/higgs2.csv");
				forest.setParamInt("columnWithY", 0);
				break;
			case 3:
				forest.setParamString("CSVFileName", "res/mnist.csv");
				forest.setParamInt("columnWithY", 0);
				break;
			case 4:
				forest.setParamString("CSVFileName", "res/HIGGS.csv");
				forest.setParamInt("columnWithY", 0);
				break;
			default:
				std::cout << "unknown dataset selected" << std::endl;
				return -1;
				break;
		}


		forest.setParamInt("numTreesInForest", 10);
		forest.setParamInt("minParent", 1);
		forest.setParamInt("numCores", numCores);

		 //logTime.startFindSplitTimer();
		forest.growForest();
		 //logTime.stopFindSplitTimer();
		 //logTime.printGrowTime();

		forest.printParameters();
		forest.printForestType();

		std::cout << "error: " << forest.testAccuracy() << "\n";

	}catch(std::exception& e){
		std::cout << "standard error: " << e.what() << std::endl;
	}
}
