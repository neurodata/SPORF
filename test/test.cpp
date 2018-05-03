#include <cmath>
#include "gtest/gtest.h"
#include "../src/fpUtil.h"
#include <string>


TEST(testUtils, testReadCSV)
{
	const std::string csvFileName = "test/res/testCSV.csv";
	csvHandle testHandle(csvFileName);

	EXPECT_EQ(3, testHandle.returnNumColumns());
	EXPECT_EQ(4, testHandle.returnNumRows());
}


TEST(testUtils, testMalformedCSV)
{
	const std::string csvFileName = "test/res/testCSVBad.csv";
	//csvHandle testHandle;

	ASSERT_THROW(csvHandle testHandle(csvFileName), std::runtime_error);
}
