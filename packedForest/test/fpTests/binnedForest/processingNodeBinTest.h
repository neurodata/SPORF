#include "../../../src/forestTypes/binnedTree/processingNodeBin.h"
#include "../../../src/fpSingleton/fpSingleton.h"
#include "../../../src/baseFunctions/MWC.h"

#include <vector>
#include <string>
#include <cstdlib>

using namespace fp;
#define typeClass int

TEST(processingNodeBinTest, checkWeightedMtry)
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
	processingNodeBin<double, weightedFeature> pNB(1, 1, 1, randNumGen);

	pNB.calcMtryForNodeTest(wf);
	EXPECT_EQ((int)wf.size(), fpSingleton::getSingleton().returnMtry());
	for (auto i : wf)
	{
		for (auto j : i.returnFeatures())
		{
			EXPECT_GE(j, 0);
			EXPECT_LT(j, numFeatures);
		}
		for (auto j : i.returnWeights())
		{
			EXPECT_TRUE(j == -1 || j == 1);
		}
	}
}

TEST(testProcessingNodeBin, testMtryMult)
{
	int mtry = 2;

	fpSingleton::getSingleton().setParameter("CSVFileName", "../res/iris.csv");
	fpSingleton::getSingleton().setParameter("columnWithY", 4);
	fpSingleton::getSingleton().setParameter("mtry", mtry);
	fpSingleton::getSingleton().loadData();
	fpSingleton::getSingleton().setDataDependentParameters();

	int treeNum = 1;
	int nodeNum = 0;
	int depth = 0;

	randomNumberRerFMWC randNum;
	std::random_device rd;
	int seed = rd();
	randNum.initialize(seed);

	processingNodeBin<double, std::vector<int>> pNodeBin(treeNum, nodeNum, depth, randNum);

	for (double setMtryMult = 1; setMtryMult < 4; setMtryMult += 0.5)
	{
		fpSingleton::getSingleton().setParameter("mtryMult", setMtryMult);

		std::vector<std::vector<int>> f2Try;
		pNodeBin.calcMtryForNodeTest(f2Try);

		EXPECT_EQ(f2Try.size(), mtry);

		std::vector<int> features_sampled;
		for (auto i : f2Try)
		{
			for (auto ii : i)
			{
				features_sampled.push_back(ii);
				EXPECT_GE(ii, 0);
				EXPECT_LT(ii, fpSingleton::getSingleton().returnNumFeatures());
			}
		}
		EXPECT_EQ(features_sampled.size(), (int)(setMtryMult * mtry));

		//printing
		// std::cout << "mtryMult: " << fpSingleton::getSingleton().returnMtryMult() << "\n";
		// int mtry = 0;
		// for (auto i : f2Try)
		// {
		//   std::cout << mtry++ << ": ";
		//   for (auto ii : i)
		//   {
		//     std::cout << ii << ",";
		//   }
		//   std::cout << "\n";
		// }
	}
}