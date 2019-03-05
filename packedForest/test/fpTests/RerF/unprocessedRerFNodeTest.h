#include "../../../src/forestTypes/basicForests/rerf/unprocessedRerFNode.h"
#include "../../../src/fpSingleton/fpSingleton.h"

using namespace fp;

TEST(checkunprocessedRerFNode, basicSetTests )
{
int numObjects = 150;
baseUnprocessedNode<float> testNode(numObjects);

	EXPECT_TRUE(testNode.returnIsLeftNode());
	EXPECT_EQ(testNode.returnInSampleSize(), numObjects);
}


TEST(checkunprocessedRerFNode, transferIndices )
{
int numObjects = 150;
baseUnprocessedNode<float> testNode(numObjects);

	EXPECT_TRUE(testNode.returnIsLeftNode());
	EXPECT_EQ(testNode.returnInSampleSize(), numObjects);
}
