#include <string>
#include "../../src/fpDataSet.h"

TEST(fpDataSet, readFromHandle )
{
	const std::string csvFileName = "./test/res/testCSV.csv";
	csvHandle<double> testHandle(csvFileName);
	rankedInput<double> inputData(testHandle);

	EXPECT_EQ(inputData.returnNumFeatures(), 3);
	EXPECT_EQ(inputData.returnNumObservations(), 4);
	EXPECT_EQ(inputData.returnFeatureValue(0,1), 4);
	EXPECT_EQ(inputData.returnFeatureValue(2,2), 9);
}

TEST(fpDataSet, readFromFileName )
{
	const std::string csvFileName = "./test/res/testCSV.csv";
	rankedInput<double> inputData(csvFileName);

	EXPECT_EQ(inputData.returnNumFeatures(), 3);
	EXPECT_EQ(inputData.returnNumObservations(), 4);
	EXPECT_EQ(inputData.returnFeatureValue(0,1), 4);
	EXPECT_EQ(inputData.returnFeatureValue(2,2), 9);
}
