#include "../../../src/forestTypes/newForest/zipClassAndValue.h"

using namespace fp;


TEST(zipClassAndValue, basicFunctions)
{
	int firstClass = 5;
	double firstValue = 3.0;

	int secondClass = 6;
	double secondValue = 6.0;

	zipClassAndValue<int, double> firstTuple;
	zipClassAndValue<int, double> secondTuple;

	firstTuple.setPair(firstClass, firstValue);
	secondTuple.setPair(secondClass, secondValue);


	EXPECT_EQ(firstTuple.returnObsClass(), firstClass);
	EXPECT_EQ(firstTuple.returnFeatureVal(), firstValue);

	EXPECT_EQ(secondTuple.returnObsClass(), secondClass);
	EXPECT_EQ(secondTuple.returnFeatureVal(), secondValue);

	EXPECT_TRUE(firstTuple < secondTuple);
	EXPECT_FALSE(secondTuple < firstTuple);

	EXPECT_EQ(firstTuple.midVal(secondTuple), (firstValue+secondValue)/2);
	EXPECT_EQ(secondTuple.midVal(firstTuple), (firstValue+secondValue)/2);

	EXPECT_TRUE(firstTuple.checkInequality(secondTuple));


	secondTuple.setPair(secondClass, firstValue);
	EXPECT_FALSE(firstTuple.checkInequality(secondTuple));
}

