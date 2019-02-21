#include "../../../src/forestTypes/newForest/processingNode.h"
#include "../../../src/forestTypes/newForest/inNodeClassTotals.h"
#include "../../../src/forestTypes/newForest/obsIndexAndClassVec.h"
#include "../../../src/forestTypes/newForest/treeStruct.h"
#include "../../../src/fpSingleton/fpSingleton.h"

#include <vector>
#include <string>
#include <cstdlib>

using namespace fp;
#define typeClass int


TEST(treeStructTest, setupRootNode)
{
	std::srand(std::time(0));
	std::string p1 = "CSVFileName";
	std::string p2 = "../res/iris.csv";
	fpSingleton::getSingleton().setParameter(p1, p2);
	fpSingleton::getSingleton().setParameter("columnWithY", 4);
	fpSingleton::getSingleton().loadData();
	fpSingleton::getSingleton().setDataDependentParameters();

	int numClasses = fpSingleton::getSingleton().returnNumClasses();
	int testSize = fpSingleton::getSingleton().returnNumObservations();
//	int numFeatures = fpSingleton::getSingleton().returnNumFeatures();

	std::vector<int> testIndices;
	obsIndexAndClassVec indexHolder(numClasses);

	for(int i = 0; i < testSize; ++i){
		testIndices.push_back(i);
	}

	for(int i = 0; i < (int)testIndices.size(); ++i){
		indexHolder.insertIndex(testIndices[i], fpSingleton::getSingleton().returnLabel(i));
	}
	std::vector<zipClassAndValue<int, float> > zipVec(testSize);

//	int treeNum = 1;
//	int nodeNum = 0;
	treeStruct<float, int> testTree(indexHolder,zipVec);
//	EXPECT_TRUE(testTree.shouldProcessNode());

	EXPECT_EQ((int)testTree.exposeTreeTest().size(), 4);//three classes + root node.
	EXPECT_EQ((int)testTree.exposeNodeQueueTest().size(), 2);//children of root
	EXPECT_EQ(testTree.parentNodesPosition(), (int)testTree.exposeTreeTest().size()-1);

	EXPECT_TRUE(testTree.exposeNodeQueueTest()[0].returnIsLeftNode() != testTree.exposeNodeQueueTest()[1].returnIsLeftNode());

	fpSingleton::getSingleton().deleteData();	
}	

TEST(treeStructTest, growTree)
{
	std::srand(std::time(0));
	std::string p1 = "CSVFileName";
	std::string p2 = "../res/iris.csv";
	fpSingleton::getSingleton().setParameter(p1, p2);
	fpSingleton::getSingleton().setParameter("columnWithY", 4);
	fpSingleton::getSingleton().loadData();
	fpSingleton::getSingleton().setDataDependentParameters();

	int numClasses = fpSingleton::getSingleton().returnNumClasses();
	int testSize = fpSingleton::getSingleton().returnNumObservations();
//	int numFeatures = fpSingleton::getSingleton().returnNumFeatures();

	std::vector<int> testIndices;
	obsIndexAndClassVec indexHolder(numClasses);

	for(int i = 0; i < testSize; ++i){
		testIndices.push_back(i);
	}

	for(int i = 0; i < (int)testIndices.size(); ++i){
		indexHolder.insertIndex(testIndices[i], fpSingleton::getSingleton().returnLabel(i));
	}
	std::vector<zipClassAndValue<int, float> > zipVec(testSize);

	treeStruct<float, int> testTree(indexHolder,zipVec);
	testTree.createTree();	

//	fpSingleton::getSingleton().deleteData();	
}
