#include "../../../src/forestTypes/newForest/obsIndexAndClassTuple.h"

using namespace fp;


TEST(obsIndexAndClassTuple, basicFunctions)
{
	int firstClass = 5;
	int secondClass = 6;

	int firstIndex = 3;
	int secondIndex = 6;

	obsIndexAndClassTuple<int> firstTuple;
	obsIndexAndClassTuple<int> secondTuple;

	firstTuple.setObsIndex(firstIndex);
	firstTuple.setObsClass(firstClass);

	secondTuple.setObsIndex(secondIndex);
	secondTuple.setObsClass(secondClass);


	EXPECT_EQ(firstTuple.returnClassOfObs(), firstClass);
	EXPECT_EQ(firstTuple.returnIndexOfObs(), firstIndex);

	EXPECT_EQ(secondTuple.returnClassOfObs(), secondClass);
	EXPECT_EQ(secondTuple.returnIndexOfObs(), secondIndex);

	EXPECT_TRUE(firstTuple < secondTuple);
	EXPECT_FALSE(secondTuple < firstTuple);

}

