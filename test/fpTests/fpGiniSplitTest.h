#include <vector>
#include <iostream>
#include "../../src/baseFunctions/fpSplit.h"

using namespace fp;


TEST(fpSplit, wholeNode )
{
	int numOfLabels= 6;
	std::vector<int> labels (numOfLabels, 4);

	split<double> findSplitTest1(labels);

	EXPECT_EQ(findSplitTest1.returnImpurity(), 0);

	std::fill(labels.begin(),labels.begin()+(numOfLabels/2), 2);

	split<double> findSplitTest2(labels);
	EXPECT_EQ(findSplitTest2.returnImpurity(), 0.5);
}


TEST(fpSplit, halfSplit )
{
	int numOfLabels= 100;
	std::vector<int> labels (numOfLabels, 4);
	std::fill(labels.begin(),labels.begin()+(numOfLabels/2), 2);

	std::vector<double> featureValues (numOfLabels,10);
	std::fill(featureValues.begin(), featureValues.begin()+(numOfLabels/2), 5);

	splitInfo<double> returnVals;

	split<double> findSplitTest1(labels);
	returnVals = findSplitTest1.giniSplit(featureValues,2);

	EXPECT_EQ(returnVals.returnSplitValue(), 7.5);
	EXPECT_EQ(returnVals.returnImpurity(), 0.0);
	EXPECT_EQ(returnVals.returnFeatureNum(), 2);
}


TEST(fpSplit, oneOffEachEnd )
{
	int numOfLabels= 6;
	////////////////////////////////////

	std::vector<int> labels (numOfLabels, 4);
	labels[0] = 1;

	std::vector<double> featureValues (numOfLabels,10);
	featureValues[0] = 5;

	splitInfo<double> returnVals1;

	split<double> findSplitTest1(labels);
	returnVals1 = findSplitTest1.giniSplit(featureValues,3);

	//////////////////////////////////////////

	std::vector<int> labels2 (numOfLabels, 4);
	labels2[numOfLabels-1] = 1;
	//	for(int i : labels2){
	//		std::cout << i << "\n";
	//		}

	std::vector<double> featureValues2 (numOfLabels,10);
	featureValues2[numOfLabels-1] = 5;
	//for(int i : featureValues2){
	//	for(int i=0; i < featureValues2.size(); ++i){
	//		std::cout << featureValues2[i] << "\n";
	//	}
	splitInfo<double> returnVals2;

	split<double> findSplitTest2(labels2);
	returnVals2 = findSplitTest2.giniSplit(featureValues2,4);

	EXPECT_EQ(returnVals1.returnSplitValue(), returnVals2.returnSplitValue());
	EXPECT_EQ(returnVals1.returnImpurity(), returnVals2.returnImpurity());
	EXPECT_EQ(returnVals1.returnFeatureNum(), 3);
	EXPECT_EQ(returnVals2.returnFeatureNum(), 4);
}


TEST(fpSplit, secondFeature )
{
	int numOfLabels= 6;
	std::vector<int> labels (numOfLabels, 4);
	std::fill(labels.begin(),labels.begin()+(numOfLabels/2), 2);

	std::vector<double> featureValues {10.0,5.0,10.0,5.0,10.0,5.0};

	splitInfo<double> returnVals;
	split<double> findSplitTest1(labels);

	returnVals = findSplitTest1.giniSplit(featureValues, 1);

	EXPECT_EQ(returnVals.returnSplitValue(), 7.5);
	EXPECT_GT(returnVals.returnImpurity(), 0.0);

	std::fill(featureValues.begin(),featureValues.end(), 8);
	std::fill(featureValues.begin(),featureValues.begin()+(numOfLabels/2), 4);

	returnVals = findSplitTest1.giniSplit(featureValues,10);

	EXPECT_EQ(returnVals.returnSplitValue(), 6);
	EXPECT_EQ(returnVals.returnImpurity(), 0.0);
	EXPECT_EQ(returnVals.returnFeatureNum(), 10);
}
