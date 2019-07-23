#include "../../../src/forestTypes/unsupervisedForests/urerf/unprocessedURerFNode.h"
#include "../../../src/fpSingleton/fpSingleton.h"

using namespace fp;

TEST(checkunprocessedRerFNode, basicSetTests )
{
int numObjects = 150;
baseUnprocessedURerFNode<float> testNode(numObjects);

	EXPECT_TRUE(testNode.returnIsLeftNode());
	EXPECT_EQ(testNode.returnInSampleSize(), numObjects);
}


TEST(checkunprocessedURerFNode, transferIndices )
{
int numObjects = 150;
baseUnprocessedURerFNode<float> testNode(numObjects);

	EXPECT_TRUE(testNode.returnIsLeftNode());
	EXPECT_EQ(testNode.returnInSampleSize(), numObjects);
}

