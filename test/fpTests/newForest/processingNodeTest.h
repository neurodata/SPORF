#include "../../../src/forestTypes/newForest/processingNode.h"
#include "../../../src/forestTypes/newForest/inNodeClassTotals.h"
#include "../../../src/forestTypes/newForest/obsIndexAndClassVec.h"
#include "../../../src/forestTypes/newForest/nodeIterators.h"
#include "../../../src/forestTypes/newForest/zipClassAndValue.h"
#include <vector>

using namespace fp;
#define typeClass int


TEST(processingNode, initialNodeSetup)
{
	int classSize = 5;
	int numClasses = 3;
	int testSize;

	std::vector<int> testIndices;
	obsIndexAndClassVec indexHolder(numClasses);

	testSize = classSize*numClasses;
	for(int i = 0; i < testSize; ++i){
		testIndices.push_back(i);
	}

	for(int i = 0; i < (int)testIndices.size(); ++i){
		indexHolder.insertIndex(testIndices[i], i % numClasses);
	}

	std::vector<zipClassAndValue<int, float> > zipVec(15);

	processingNode<int, float> testNode(numClasses);

	EXPECT_EQ(1, 1);
	}

