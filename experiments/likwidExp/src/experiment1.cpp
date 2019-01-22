#include "../../../src/packedForest.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
	if (argc != 4) return -1;
	int alg = atoi(argv[1]);
	int dataSet = atoi(argv[2]);
	int numCores = atoi(argv[3]);

	/*
		 fp::timeLogger logTime;
		 logTime.startFindSplitTimer();
		 fp::inNodeClassIndices test(100000000);
		 logTime.stopFindSplitTimer();

		 logTime.startSortTimer();
		 fp::stratifiedInNodeClassIndices testw(100000000);
		 logTime.stopSortTimer();

		 logTime.printGrowTime();
		 */	
	LIKWID_MARKER_INIT;
	try{
		fp::fpForest<double> forest;

		switch(alg){
			case 1:
				forest.setParameter("forestType", "rerf");
				break;
			case 2:
				forest.setParameter("forestType", "rfBase");
				break;
			case 3:
				forest.setParameter("forestType", "rerf");
				forest.setParameter("useBinning", 1000);
				break;
			case 4:
				forest.setParameter("forestType", "rfBase");
				forest.setParameter("useBinning", 1000);
				break;
			case 5:
				forest.setParameter("forestType", "inPlace");
				break;
			default:
				std::cout << "unknown alg selected" << std::endl;
				return -1;
				break;
		}


		switch(dataSet){
			case 1: 
				forest.setParameter("CSVFileName", "../../res/iris.csv");
				forest.setParameter("columnWithY", 4);
				break;
			case 2:
				forest.setParameter("CSVFileName", "../../res/higgs2.csv");
				forest.setParameter("columnWithY", 0);
				break;
			case 3:
				forest.setParameter("CSVFileName", "../../res/mnist.csv");
				forest.setParameter("columnWithY", 0);
				break;
			case 4:
				forest.setParameter("CSVFileName", "../../res/higgsData.csv");
				forest.setParameter("columnWithY", 0);
				break;
			default:
				std::cout << "unknown dataset selected" << std::endl;
				return -1;
				break;
		}


		forest.setParameter("numTreesInForest", 10);
		forest.setParameter("minParent", 1);
		forest.setParameter("numCores", numCores);

		std::cout << "growing";
		forest.growForest();

		std::cout << "grew" << std::endl;
		forest.printParameters();
		forest.printForestType();

	}catch(std::exception& e){
		std::cout << "standard error: " << e.what() << std::endl;
	}
	LIKWID_MARKER_CLOSE;
}
