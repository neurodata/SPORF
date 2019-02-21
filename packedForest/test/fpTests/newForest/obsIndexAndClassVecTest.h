#include "../../../src/forestTypes/newForest/obsIndexAndClassVec.h"

#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <algorithm>

using namespace fp;


TEST(obsIndexAndClassVec, basicFunctions)
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

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle (testIndices.begin(), testIndices.end(), std::default_random_engine(seed));

	for(int i = 0; i < (int)testIndices.size(); ++i){
		indexHolder.insertIndex(testIndices[i], i % numClasses);
	}

	for(int i = 0; i < numClasses; ++i){
		EXPECT_EQ((int)indexHolder.returnClassVector(i).size(), classSize);
	}

	indexHolder.sortVectors();

	for(int j = 0; j < numClasses; ++j){
		for(int i = 1; i < classSize; ++i){
			EXPECT_LT(indexHolder.returnClassVector(j)[i-1], indexHolder.returnClassVector(j)[i]);
		}
	}

	indexHolder.resetVectors();

	for(int j = 0; j < numClasses; ++j){
			EXPECT_EQ((int)indexHolder.returnClassVector(j).size(), 0);
	}
}

