#include <string>
#include "../../src/fpReadCSV.h"

TEST(fpReadCSV, testReadValsCSV)
{
	const std::string csvFileName = "test/res/testCSV.csv";
	csvHandle testHandle(csvFileName);

	EXPECT_EQ(testHandle.returnNextElement<double>(), 1);

	double temp;
	for(int i = 0; i < (testHandle.returnNumRows()*testHandle.returnNumColumns())-1;i++){
	temp = testHandle.returnNextElement<double>();
	}
	EXPECT_EQ(temp, 12);
}


TEST(fpReadCSV, testReadCSV)
{
	const std::string csvFileName = "test/res/testCSV.csv";
	csvHandle testHandle(csvFileName);

	EXPECT_EQ(3, testHandle.returnNumColumns());
	EXPECT_EQ(4, testHandle.returnNumRows());
}


TEST(fpReadCSV, testMalformedCSV)
{
	const std::string csvFileName = "test/res/testCSVBad.csv";
//	csvHandle<double> testHandle;

	ASSERT_THROW(csvHandle testHandle(csvFileName), std::runtime_error);
}

TEST(fpReadCSV, unreadableFile)
{
	const std::string csvFileName = "test/res/testCSVNoExist.csv";

	ASSERT_THROW(csvHandle testHandle(csvFileName), std::runtime_error);
}
