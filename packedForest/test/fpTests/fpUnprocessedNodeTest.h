#include <vector>
#include <iostream>
#include "../../src/treeConstructor/unprocessedNode.h"

using namespace fp;

TEST(unprocessed, initialize )
{
	int nodeSize = 100;
	int numFeatures = 5;
	int mtry = 3;
	unprocessedNode testUNode(nodeSize);

EXPECT_EQ(testUNode.returnInSampleSize(), nodeSize);
testUNode.pickMTRY(numFeatures,mtry);
EXPECT_EQ(testUNode.returnMTRYVectorSize(),3);

}

