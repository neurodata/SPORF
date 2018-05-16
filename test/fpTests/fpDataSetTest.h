#include <string>
#include "../../src/fpDataSet.h"

#define COMMA()

TEST(fpDataSet, readFromHandle )
{
	const std::string csvFileName = "./test/res/testCSV.csv";
	csvHandle testHandle(csvFileName);
	rankedInput<double> inputData(testHandle);

	EXPECT_EQ(inputData.returnNumFeatures(), 3);
	EXPECT_EQ(inputData.returnNumObservations(), 4);
	EXPECT_EQ(inputData.returnFeatureValue(0,1), 4);
	EXPECT_EQ(inputData.returnFeatureValue(2,2), 9);
}

TEST(fpDataSet, readFromFileName )
{
	const std::string csvFileName = "./test/res/testCSV.csv";
	inputData<double, int> data(csvFileName,1);

	EXPECT_EQ(data.returnNumFeatures(), 2);
	EXPECT_EQ(data.returnNumObservations(), 4);
	EXPECT_EQ(data.returnFeatureValue(0,1), 4);
	EXPECT_EQ(data.returnFeatureValue(1,3), 12);
	EXPECT_EQ(data.returnClassOfObservation(2), 8);

	try{
		inputData<double , int> dataBad(csvFileName,4);
		//Shouldn't be called.
		EXPECT_EQ(1,2);
	}catch(std::exception& e){
		ASSERT_STREQ(e.what(),  "column with class labels does not exist.");
	}

	try{
		inputData<double , int> dataBad(csvFileName,3);
		//Shouldn't be called.
		EXPECT_EQ(1,2);
	}catch(std::exception& e){
		ASSERT_STREQ(e.what(),  "column with class labels does not exist.");
	}
}

