#include "../../../src/forestTypes/newForest/processingNode.h"
#include "../../../src/forestTypes/newForest/inNodeClassTotals.h"
#include "../../../src/forestTypes/newForest/obsIndexAndClassVec.h"
#include "../../../src/forestTypes/newForest/nodeIterators.h"
#include "../../../src/forestTypes/newForest/zipClassAndValue.h"
//#include "../../../src/packedForest.h"
#include "../../../src/fpSingleton/fpSingleton.h"

#include <vector>
#include <string>
#include <cstdlib>

using namespace fp;
#define typeClass int


TEST(processingNodeTest, setupRootNodeRF)
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
	int numFeatures = fpSingleton::getSingleton().returnNumFeatures();

	std::vector<int> testIndices;
	obsIndexAndClassVec indexHolder(numClasses);

	for(int i = 0; i < testSize; ++i){
		testIndices.push_back(i);
	}

	for(int i = 0; i < (int)testIndices.size(); ++i){
		indexHolder.insertIndex(testIndices[i], fpSingleton::getSingleton().returnLabel(i));
	}
	std::vector<zipClassAndValue<int, float> > zipVec(testSize);

	int treeNum = 1;
	int nodeNum = 0;
	processingNode<float, int> testRootNode(treeNum,nodeNum);
	testRootNode.setupRoot(indexHolder, zipVec);

	EXPECT_EQ(testRootNode.exposeTreeNum(), treeNum);
	EXPECT_EQ(testRootNode.exposeParentNode(), nodeNum);
	for(auto i : testRootNode.exposeMtry()){
		EXPECT_TRUE((i >= 0) && (i <=numFeatures)); 
	}
	EXPECT_FALSE(testRootNode.exposePropertiesOfThisNode().isNodePure());
	EXPECT_EQ(testRootNode.exposePropertiesOfThisNode().returnNumItems(), testSize);

	int j = testRootNode.exposePropertiesOfThisNode().returnImpurity();
	EXPECT_TRUE((j >= 100) && (j <=101)); 


	std::vector<int> testClassSize;
	testRootNode.exposeNodeIndices().setVecOfClassSizes(testClassSize);
	for(auto& i : testClassSize){
		EXPECT_EQ(i, testSize/numClasses);
	}

	EXPECT_EQ(testRootNode.exposeZipIters().returnZipBegin(), zipVec.begin());
	EXPECT_EQ(testRootNode.exposeZipIters().returnZipEnd(), zipVec.end());

	testRootNode.resetLeftNodeTest();
	EXPECT_EQ(testRootNode.exposePropertiesOfLeftNode().returnNumItems(), 0);

	testRootNode.resetRightNodeTest();
	EXPECT_EQ(testRootNode.exposePropertiesOfRightNode().returnNumItems(), 150);
	EXPECT_EQ(testRootNode.exposePropertiesOfRightNode().calcAndReturnImpurity(), testRootNode.exposePropertiesOfThisNode().returnImpurity());

}


TEST(processingNodeTest, loadStandardRFZip)
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

	std::vector<int> testIndices;
	obsIndexAndClassVec indexHolder(numClasses);

	for(int i = 0; i < testSize; ++i){
		testIndices.push_back(i);
	}

	for(int i = 0; i < (int)testIndices.size(); ++i){
		indexHolder.insertIndex(testIndices[i], fpSingleton::getSingleton().returnLabel(i));
	}
	std::vector<zipClassAndValue<int, float> > zipVec(testSize);

	int treeNum = 1;
	int nodeNum = 0;
	processingNode<float, int> testRootNode(treeNum,nodeNum);
	testRootNode.setupRoot(indexHolder, zipVec);

	int testFeature = 0;
	testRootNode.loadWorkingSetTest(testFeature);

	float accumulator = 0;
	for(int i = 0; i < testSize; ++i){
		float testFromZip = (float)(testRootNode.exposeZipIters().returnZipBegin()+i)->returnFeatureVal();
		float testFromData = (float)fpSingleton::getSingleton().returnFeatureVal(testFeature,i);
		EXPECT_EQ(testFromZip, testFromData);
		accumulator += testFromZip;
	}


	testRootNode.sortWorkingSetTest();

	float previousValue = std::numeric_limits<float>::min();
	float sortedAccumulator = 0;
	for(int i = 0; i < testSize; ++i){
		float testFromZip = (float)(testRootNode.exposeZipIters().returnZipBegin()+i)->returnFeatureVal();
		EXPECT_TRUE(testFromZip > previousValue);
		sortedAccumulator += testFromZip;
	}
	EXPECT_EQ((int)accumulator, (int)sortedAccumulator);
}


TEST(processingNodeTest, findBestSplit)
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
	//int numFeatures = fpSingleton::getSingleton().returnNumFeatures();

	std::vector<int> testIndices;
	obsIndexAndClassVec indexHolder(numClasses);

	for(int i = 0; i < testSize; ++i){
		testIndices.push_back(i);
	}

	for(int i = 0; i < (int)testIndices.size(); ++i){
		indexHolder.insertIndex(testIndices[i], fpSingleton::getSingleton().returnLabel(i));
	}
	std::vector<zipClassAndValue<int, float> > zipVec(testSize);

	int treeNum = 1;
	int nodeNum = 0;
	processingNode<float, int> testRootNode(treeNum,nodeNum);
	testRootNode.setupRoot(indexHolder, zipVec);

	int testFeature = 3;
	testRootNode.loadWorkingSetTest(testFeature);
	testRootNode.sortWorkingSetTest();

	testRootNode.findBestSplitTest(testFeature);

	EXPECT_EQ(testRootNode.exposeBestSplit().returnFeatureNum(), testFeature);
	EXPECT_EQ(testRootNode.exposePropertiesOfRightNode().returnNumItems(), 1);
	EXPECT_EQ(testRootNode.exposePropertiesOfLeftNode().returnNumItems(), 149);

	EXPECT_EQ(testRootNode.exposeBestSplit().returnImpurity(), 50);

	int testFeatureTwo = 2;
	testRootNode.calcBestSplitInfoForNode(testFeatureTwo);

	EXPECT_EQ(testRootNode.exposeBestSplit().returnFeatureNum(), testFeature);
	EXPECT_EQ(testRootNode.exposePropertiesOfRightNode().returnNumItems(), 1);
	EXPECT_EQ(testRootNode.exposePropertiesOfLeftNode().returnNumItems(), 149);

	EXPECT_EQ(testRootNode.exposeBestSplit().returnImpurity(), 50);

	testRootNode.exposeBestSplit().setImpurity(std::numeric_limits<float>::max());
	testRootNode.calcBestSplitInfoForNode(testFeatureTwo);

	EXPECT_EQ(testRootNode.exposeBestSplit().returnFeatureNum(), testFeatureTwo);
	EXPECT_EQ(testRootNode.exposePropertiesOfRightNode().returnNumItems(), 1);
	EXPECT_EQ(testRootNode.exposePropertiesOfLeftNode().returnNumItems(), 149);

	EXPECT_EQ(testRootNode.exposeBestSplit().returnImpurity(), 50);
}


TEST(processingNodeTest, moveLeftRight)
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

	std::vector<int> testIndices;
	obsIndexAndClassVec indexHolder(numClasses);

	for(int i = 0; i < testSize; ++i){
		testIndices.push_back(i);
	}

	for(int i = 0; i < (int)testIndices.size(); ++i){
		indexHolder.insertIndex(testIndices[i], fpSingleton::getSingleton().returnLabel(i));
	}
	std::vector<zipClassAndValue<int, float> > zipVec(testSize);

	int treeNum = 1;
	int nodeNum = 0;
	processingNode<float, int> testRootNode(treeNum,nodeNum);
	testRootNode.setupRoot(indexHolder, zipVec);

	int testFeature = 3;
	testRootNode.loadWorkingSetTest(testFeature);
	testRootNode.sortWorkingSetTest();

	testRootNode.findBestSplitTest(testFeature);

	//std::vector<std::vector<int>::iterator > splitLocations;

	testRootNode.setVecOfSplitLocationsTest(testFeature);

	//left side check
	int classLabel =0;
	EXPECT_EQ(testRootNode.exposeNodeIndices().returnSplitIterator(classLabel)-testRootNode.exposeNodeIndices().returnBeginIterator(classLabel), 50);
	/*
		 for(int i = 0; i < 50; ++i){
		 std::cout << *(testRootNode.exposeNodeIndices().returnBeginIterator(classLabel)+i) << " ";
		 }
		 std::cout << "\n";
		 */
	classLabel =1;
	EXPECT_EQ(testRootNode.exposeNodeIndices().returnSplitIterator(classLabel)-testRootNode.exposeNodeIndices().returnBeginIterator(classLabel), 0);
	classLabel =2;
	EXPECT_EQ(testRootNode.exposeNodeIndices().returnSplitIterator(classLabel)-testRootNode.exposeNodeIndices().returnBeginIterator(classLabel), 0);

	//right side check
	classLabel =0;
	EXPECT_EQ(testRootNode.exposeNodeIndices().returnEndIterator(classLabel)-testRootNode.exposeNodeIndices().returnSplitIterator(classLabel), 0);
	classLabel =1;
	EXPECT_EQ(testRootNode.exposeNodeIndices().returnEndIterator(classLabel)-testRootNode.exposeNodeIndices().returnSplitIterator(classLabel), 50);
	/*
		 for(int i = 0; i < 50; ++i){
		 std::cout << *(testRootNode.exposeNodeIndices().returnBeginIterator(classLabel)+i) << " ";
		 }
		 std::cout << "\n";
		 */
	classLabel =2;
	EXPECT_EQ(testRootNode.exposeNodeIndices().returnEndIterator(classLabel)-testRootNode.exposeNodeIndices().returnSplitIterator(classLabel), 50);
	/*
		 for(int i = 0; i < 50; ++i){
		 std::cout << *(testRootNode.exposeNodeIndices().returnBeginIterator(classLabel)+i) << " ";
		 }
		 std::cout << "\n";
		 */
}


TEST(processingNodeTest, moveLeft)
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

	std::vector<int> testIndices;
	obsIndexAndClassVec indexHolder(numClasses);

	for(int i = 0; i < testSize; ++i){
		testIndices.push_back(i);
	}

	for(int i = 0; i < (int)testIndices.size(); ++i){
		indexHolder.insertIndex(testIndices[i], fpSingleton::getSingleton().returnLabel(i));
	}
	std::vector<zipClassAndValue<int, float> > zipVec(testSize);

	int treeNum = 1;
	int nodeNum = 0;
	processingNode<float, int> testRootNode(treeNum,nodeNum);
	testRootNode.setupRoot(indexHolder, zipVec);

	int testFeature = 3;

	while(!testRootNode.exposeMtry().empty()){
		testRootNode.exposeMtry().pop_back();
	}

	testRootNode.calcBestSplitInfoForNode(testFeature);
	testRootNode.setVecOfSplitLocationsTest(testFeature);

	testRootNode.setAsInternalNode();
	EXPECT_FALSE(testRootNode.isLeafNode());
testRootNode.setAsLeafNode();
	EXPECT_TRUE(testRootNode.isLeafNode());


	treeNum = treeNum+1;
	nodeNum = nodeNum+1;
	processingNode<float, int> testNextNode(treeNum,nodeNum);
	testNextNode.setupNode(testRootNode, true);

	EXPECT_EQ(testNextNode.exposeTreeNum(), treeNum);
	EXPECT_EQ(testNextNode.exposeParentNode(), nodeNum);
	for(auto i : testNextNode.exposeMtry()){
		EXPECT_EQ(i,0); 
	}
	EXPECT_TRUE(testNextNode.exposePropertiesOfThisNode().isNodePure());
	EXPECT_EQ(testNextNode.exposePropertiesOfThisNode().returnNumItems(), 50);

	int j = testNextNode.exposePropertiesOfThisNode().returnImpurity();
	EXPECT_EQ(j,0); 


	std::vector<int> testClassSize;
	testNextNode.exposeNodeIndices().setVecOfClassSizes(testClassSize);
	EXPECT_EQ(testClassSize[0], 50);
	EXPECT_EQ(testClassSize[1], 0);
	EXPECT_EQ(testClassSize[2], 0);

	EXPECT_EQ(testNextNode.exposeZipIters().returnZipEnd()-testNextNode.exposeZipIters().returnZipBegin(), 50);
}


TEST(processingNodeTest, moveRight)
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
	int numFeatures = fpSingleton::getSingleton().returnNumFeatures();

	std::vector<int> testIndices;
	obsIndexAndClassVec indexHolder(numClasses);

	for(int i = 0; i < testSize; ++i){
		testIndices.push_back(i);
	}

	for(int i = 0; i < (int)testIndices.size(); ++i){
		indexHolder.insertIndex(testIndices[i], fpSingleton::getSingleton().returnLabel(i));
	}
	std::vector<zipClassAndValue<int, float> > zipVec(testSize);

	int treeNum = 1;
	int nodeNum = 0;
	processingNode<float, int> testRootNode(treeNum,nodeNum);
	testRootNode.setupRoot(indexHolder, zipVec);

	int testFeature = 3;

	while(!testRootNode.exposeMtry().empty()){
		testRootNode.exposeMtry().pop_back();
	}

	testRootNode.calcBestSplitInfoForNode(testFeature);
	testRootNode.setVecOfSplitLocationsTest(testFeature);


	treeNum = treeNum+1;
	nodeNum = nodeNum+1;
	processingNode<float, int> testNextNode(treeNum,nodeNum);
	testNextNode.setupNode(testRootNode, false);

	EXPECT_EQ(testNextNode.exposeTreeNum(), treeNum);
	EXPECT_EQ(testNextNode.exposeParentNode(), nodeNum);
	for(auto i : testNextNode.exposeMtry()){
		EXPECT_TRUE((i >= 0) && (i <=numFeatures)); 
	}
	EXPECT_FALSE(testNextNode.exposePropertiesOfThisNode().isNodePure());
	EXPECT_EQ(testNextNode.exposePropertiesOfThisNode().returnNumItems(), 100);
	EXPECT_EQ(testNextNode.returnNodeSize(), 100);

	int j = testNextNode.exposePropertiesOfThisNode().returnImpurity();
	EXPECT_EQ(j,50); 


	std::vector<int> testClassSize;
	testNextNode.exposeNodeIndices().setVecOfClassSizes(testClassSize);
	EXPECT_EQ(testClassSize[0], 0);
	EXPECT_EQ(testClassSize[1], 50);
	EXPECT_EQ(testClassSize[2], 50);

	EXPECT_EQ(testNextNode.exposeZipIters().returnZipEnd()-testNextNode.exposeZipIters().returnZipBegin(), 100);
}
