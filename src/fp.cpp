#include "packedForest.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
		    if (argc != 3) return -1;
				    int dataSet = atoi(argv[1]);
						int numCores = atoi(argv[2]);

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
	try{
		fp::fpForest<double> forest;

		if(false){
			forest.setParameter("forestType", "rerf");
		}else{
			forest.setParameter("forestType", "rfBase");
		}


		forest.setParameter("numTreesInForest", 100);
		forest.setParameter("minParent", 1);
	//	int dataSet = 3;

		switch(dataSet){
			case 1: 
				forest.setParameter("CSVFileName", "res/iris.csv");
				forest.setParameter("columnWithY", 4);
				break;
			case 2:
				forest.setParameter("CSVFileName", "res/higgs2.csv");
				forest.setParameter("columnWithY", 0);
				break;
			case 3:
				forest.setParameter("CSVFileName", "res/mnist.csv");
				forest.setParameter("columnWithY", 0);
				break;
			case 4:
				forest.setParameter("CSVFileName", "res/higgsData.csv");
				forest.setParameter("columnWithY", 0);
				break;
		}

//				forest.setParameter("useBinning", 1000);
				forest.setParameter("numCores", numCores);



		//forest.setParameter("CSVFileName", "test/res/testCSV.csv");
		//		forest.setParameter("numClasses", 5);
		//		forest.setParameter("mtry", 100);



		forest.growForest();

		forest.printParameters();
		forest.printForestType();

		std::cout << "error: " << forest.testAccuracy() << "\n";

	}catch(std::exception& e){
		std::cout << "standard error: " << e.what() << std::endl;
	}
}
