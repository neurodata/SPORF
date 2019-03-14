#include "../../../src/forestTypes/binnedTree/processingNode.h"

#include <vector>
#include <string>
#include <cstdlib>

using namespace fp;
#define typeClass int


TEST(processingBinnedNodeTest, checkWeightedMtry)
{
	std::srand(std::time(0));
	std::string p1 = "CSVFileName";
	std::string p2 = "../res/iris.csv";
	fpSingleton::getSingleton().setParameter(p1, p2);
	fpSingleton::getSingleton().setParameter("columnWithY", 4);
	fpSingleton::getSingleton().loadData();
	fpSingleton::getSingleton().setDataDependentParameters();

	int numClasses = fpSingleton::getSingleton().returnNumClasses();
	int testSize = fpSingleton::getSingleton().returnNumObservations();
	int numFeatures = fpSingleton::getSingleton().returnNumFeatures();

	std::vector<weightedFeature> wf;
	randomNumberRerFMWC randNumGen;
	processingNodeBin<double, weightedFeature> pNB(1,1,1,randNumGen);

	pNB.calcMtryForNodeTest(wf);
	EXPECT_EQ((int)wf.size(),fpSingleton::getSingleton().returnMtry());
	for(auto i : wf){
		for(auto j : i.returnFeatures()){
			EXPECT_GE(j,0);
				EXPECT_LT(j,numFeatures);
		}
		for(auto j : i.returnWeights()){
			EXPECT_TRUE(j == -1 || j == 1);
		}
	}
}
