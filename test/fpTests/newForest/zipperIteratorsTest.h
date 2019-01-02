#include "../../../src/forestTypes/newForest/zipperIterators.h"
#include "../../../src/forestTypes/newForest/zipClassAndValue.h"

#include <vector>

using namespace fp;


TEST(zipperIterators, basicFunctions)
{
	std::vector<zipClassAndValue<int, float> > zipVec(15);

	zipperIterators<int,float> zipOne;

	zipOne.setZipIterators(zipVec);

	EXPECT_EQ(zipOne.returnZipBegin(), zipVec.begin());
	EXPECT_EQ(zipOne.returnZipEnd(), zipVec.end());

	zipperIterators<int,float> zipTwo;

	zipTwo.setZipBegin(zipVec.begin());
	zipTwo.setZipEnd(zipVec.end());

	EXPECT_EQ(zipTwo.returnZipBegin(), zipVec.begin());
	EXPECT_EQ(zipTwo.returnZipEnd(), zipVec.end());

}

