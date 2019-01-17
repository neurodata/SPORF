#include "../../../src/forestTypes/rerf/unprocessedRerFNode.h"


using namespace fp;

TEST(checkunprocessedRerFNode, basicSetTests )
{
int numObjects = 150;
baseUnprocessedNode<float> testNode(numObjects);

	EXPECT_TRUE(testNode.returnIsLeftNode());
	EXPECT_EQ(testNode.returnInSampleSize(), numObjects);
}
