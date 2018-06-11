#include <string>
#include "../../src/fpSingleton/fpDataSet.h"

#define COMMA()

TEST(fpDataSet, readFromHandle )
{
	const std::string csvFileName = "./res/testCSV.csv";
	csvHandle testHandle(csvFileName);
	rankedInput<double> inputData(testHandle);

	EXPECT_EQ(inputData.returnNumFeatures(), 3);
	EXPECT_EQ(inputData.returnNumObservations(), 4);
	EXPECT_EQ(inputData.returnFeatureValue(0,1), 4);
	EXPECT_EQ(inputData.returnFeatureValue(2,2), 9);
}

TEST(fpDataSet, readFromFileName )
{
	const std::string csvFileName = "./res/testCSV.csv";
	inputData<double, int> data(csvFileName,1);

	EXPECT_EQ(data.returnNumFeatures(), 2);
	EXPECT_EQ(data.returnNumObservations(), 4);
	EXPECT_EQ(data.returnFeatureValue(0,1), 4);
	EXPECT_EQ(data.returnFeatureValue(1,3), 12);
	EXPECT_EQ(data.returnClassOfObservation(2), 8);

}

TEST(fpDataSet, wrongLabelLocation )
{
	const std::string csvFileName = "./res/testCSV.csv";

		try{
		inputData<double , int> dataBad1(csvFileName,3);
		//Shouldn't be called.
		EXPECT_EQ(1,2);
	}catch(std::exception& e){
		ASSERT_STREQ(e.what(),  "column with class labels does not exist.");
	}

	try{
		inputData<double , int> dataBad(csvFileName,4);
		//Shouldn't be called.
		EXPECT_EQ(1,2);
	}catch(std::exception& e){
		ASSERT_STREQ(e.what(),  "column with class labels does not exist.");
	}

}


TEST(fpDataSet, checkYInput )
{
	const std::string csvFileName = "./res/testCSV.csv";
	inputData<double, int> dataYtest(csvFileName,0);


	EXPECT_EQ(dataYtest.returnNumClasses(), 10);

	try{
		dataYtest.checkY();
		//Shouldn't be called.
		EXPECT_EQ(1,2);
	}catch(std::exception& e){
		ASSERT_STREQ(e.what(),  "Not all classes represented in input.");
	}

const std::string csvFileName1 = "./res/testCSVAllClass.csv";
	inputData<double, int> dataYtest2(csvFileName1,0);
try{
		dataYtest2.checkY();
	}catch(std::exception& e){
		//Shouldn't be called.
		EXPECT_EQ(1,2);
	}

const std::string csvFileName2 = "./res/testCSVAllClassBad.csv";
	inputData<double, int> dataYtest3(csvFileName2,0);
try{
		dataYtest3.checkY();
		//Shouldn't be called.
		EXPECT_EQ(1,2);
	}catch(std::exception& e){
		ASSERT_STREQ(e.what(),  "Not all classes represented in input.");
	}
}
