#include "../../src/forestTypes/basicForests/rfClassification/fpForestClassificationBase.h"
#include "../../src/forestTypes/basicForests/rfClassification/rfTree.h"
#include "../../src/fpSingleton/fpSingleton.h"
#include <vector>

TEST(testRFtree, testOOB){

	bool DEBUG = true;
    fp::fpForestClassificationBase<double> forest;
	fp::fpSingleton::getSingleton().setParameter("CSVFileName", "../res/iris.csv");
	fp::fpSingleton::getSingleton().setParameter("columnWithY", 4);
	fp::fpSingleton::getSingleton().setParameter("numTreesInForest", 1);
	fp::fpSingleton::getSingleton().setParameter("minParent", 1);
	fp::fpSingleton::getSingleton().setParameter("numCores", 1);
	fp::fpSingleton::getSingleton().setParameter("seed",-1661580692);
	fp::fpSingleton::getSingleton().setParameter("mtry",3);

    forest.growForest();
	std::vector<std::vector<double> > dataVec = forest.testOneTreeOOB();
	std::vector<int> dataInd = forest.testOneTreeOOBind();

	float internalResult = forest.reportOOB();
	std::map<std::string, int> nn = forest.testReturnNumCorrectAndNumOOB();

	int countCorrect = 0;

	for(int i = 0; i < dataVec.size(); i++){
		if(forest.predictClass(dataVec[i]) == fpSingleton::getSingleton().returnLabel(dataInd[i])){
			countCorrect++;
		}
	}

	float manualResult = (float) 1 - ((float) countCorrect / (float) dataInd.size());

	if(DEBUG){
		std::cout << "count correct " << countCorrect << std::endl; //DEBUG
		std::cout << "data Ind" << dataInd.size() << std::endl; //DEBUG
		std::cout << "manual " << manualResult << std::endl; //DEBUG
		std::cout << "internal " << internalResult << std::endl; //DEBUG
	}


	EXPECT_EQ(manualResult, internalResult);
}
