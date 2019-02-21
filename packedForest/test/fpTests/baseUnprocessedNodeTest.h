#include <string>
#include <vector>
#include <algorithm>
#include "../../src/forestTypes/basicForests/baseUnprocessedNode.h"
//#include "../../src/baseFunctions/fpUtils.h"

using namespace fp;

TEST(checkBaseUnprocessedNode, basicSetTests )
{
int numObjects = 150;
baseUnprocessedNode<double> testNode(numObjects);

	EXPECT_TRUE(testNode.returnIsLeftNode());
	EXPECT_EQ(testNode.returnInSampleSize(), numObjects);
}
