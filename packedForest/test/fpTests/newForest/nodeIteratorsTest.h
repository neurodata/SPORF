#include "../../../src/forestTypes/newForest/nodeIterators.h"

#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <algorithm>

using namespace fp;


TEST(nodeIterators, basicFunctions)
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

	nodeIterators testInitial(numClasses);
	testInitial.setInitialIterators(indexHolder);


	std::vector<int> testVector;

	testInitial.setVecOfClassSizes(testVector);
	for(int i = 0; i < numClasses; ++i){
		EXPECT_EQ(testVector[i], classSize);
	}


	for(int i = 0; i < numClasses; ++i){
		EXPECT_EQ(indexHolder.returnClassVector(i).begin(), testInitial.returnBeginIterator(i));
		EXPECT_EQ(indexHolder.returnClassVector(i).end(), testInitial.returnEndIterator(i));
	}

	for(int i = 0; i < numClasses; ++i){
		testInitial.insertBeginIterator(indexHolder.returnClassVector(i).begin()+1, i);
		testInitial.insertEndIterator(indexHolder.returnClassVector(i).end()-1, i);
	}

	for(int i = 0; i < numClasses; ++i){
		EXPECT_EQ(indexHolder.returnClassVector(i).begin()+1, testInitial.returnBeginIterator(i));
		EXPECT_EQ(indexHolder.returnClassVector(i).end()-1, testInitial.returnEndIterator(i));
	}



}

