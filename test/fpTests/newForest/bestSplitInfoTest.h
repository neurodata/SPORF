#include "../../../src/forestTypes/newForest/bestSplitInfo.h"

using namespace fp;


TEST(bestSplitInfo, basicFunctions)
{

	float testImpurity = 2.0;
	float testSplitValue = 5.0;
	int testSplitFeature = 3;

	std::vector<int> testMultiFeature(5,10);

	bestSplitInfo<float, int> splitOne;
	//bestSplitInfo<float, int> splitTwo;

	bestSplitInfo<float, std::vector<int> > splitMultiOne;
	//bestSplitInfo<float, std::vector<int> > splitMultiTwo;

	splitOne.setSplitValue(testSplitValue);
	EXPECT_EQ(splitOne.returnSplitValue(), testSplitValue);

	splitOne.setImpurity(testImpurity);
	EXPECT_EQ(splitOne.returnImpurity(), testImpurity);

	splitOne.setFeature(testSplitFeature);
	EXPECT_EQ(splitOne.returnFeatureNum(), testSplitFeature);

	splitMultiOne.setFeature(testMultiFeature);
	EXPECT_EQ(splitMultiOne.returnFeatureNum()[1], 10);
	EXPECT_EQ(splitMultiOne.returnFeatureNum()[3], 10);
}

