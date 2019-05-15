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


TEST(processingNodeBinTest, checkStructuredRerF_2d)
{
	std::srand(std::time(0));
	std::string p1 = "CSVFileName";
	std::string p2 = "../res/mnist.csv";
	fpSingleton::getSingleton().setParameter(p1, p2);
	fpSingleton::getSingleton().setParameter("mtry", 1);
	fpSingleton::getSingleton().setParameter("methodToUse", 2);
	fpSingleton::getSingleton().setParameter("imageHeight", 28);
	fpSingleton::getSingleton().setParameter("imageWidth", 28);
	fpSingleton::getSingleton().setParameter("patchHeightMax", 28);
	fpSingleton::getSingleton().setParameter("patchHeightMin", 1);
	fpSingleton::getSingleton().setParameter("patchWidthMax", 28);
	fpSingleton::getSingleton().setParameter("patchWidthMin", 1);
	fpSingleton::getSingleton().loadData();
	fpSingleton::getSingleton().setDataDependentParameters();
	fpSingleton::getSingleton().checkDataDependentParameters();

	int numClasses = fpSingleton::getSingleton().returnNumClasses();
	int testSize = fpSingleton::getSingleton().returnNumObservations();
	int numFeatures = fpSingleton::getSingleton().returnNumFeatures();

	std::vector<weightedFeature> wf;
	randomNumberRerFMWC randNumGen;
	processingNodeBin<double, weightedFeature> pNB(1, 1, 1, randNumGen);

	// Get the 28x28 patch at the top left and make sure we don't fall
	// off the edge.
	wf.resize(fpSingleton::getSingleton().returnMtry());
	EXPECT_EQ((int)wf.size(), fpSingleton::getSingleton().returnMtry());

	std::vector<int> patchParams = {28, 28, 0};
	pNB.randMatImagePatchTest(wf, patchParams);

	for (auto i : wf) {
		for (auto j : i.returnFeatures()) {
			EXPECT_GE(j, 0);
			EXPECT_LT(j, numFeatures);
		}
	}

	// Get the 5x5 patch on the lower right edge of the image.
	// check that we don't fall off the edge.
	std::vector<weightedFeature> wf2;
	wf2.resize(fpSingleton::getSingleton().returnMtry());
	patchParams = {5, 5, 667};
	pNB.randMatImagePatchTest(wf2, patchParams);

	for (auto i : wf2) {
		for (auto j : i.returnFeatures()) {
			EXPECT_GE(j, 667);
			EXPECT_LT(j, numFeatures);
		}
	}
}

