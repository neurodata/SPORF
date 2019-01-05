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
	std::string p2 = "res/iris.csv";
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

	testRootNode.resetLeftNode();
	EXPECT_EQ(testRootNode.exposePropertiesOfLeftNode().returnNumItems(), 0);

	testRootNode.resetRightNode();
	EXPECT_EQ(testRootNode.exposePropertiesOfRightNode().returnNumItems(), 150);
	EXPECT_EQ(testRootNode.exposePropertiesOfRightNode().calcAndReturnImpurity(), testRootNode.exposePropertiesOfThisNode().returnImpurity());

}


TEST(processingNodeTest, loadStandardRFZip)
{

	std::srand(std::time(0));
	std::string p1 = "CSVFileName";
	std::string p2 = "res/iris.csv";
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
	testRootNode.loadWorkingSet(testFeature, numClasses);

	float accumulator = 0;
	for(int i = 0; i < testSize; ++i){
		float testFromZip = (float)(testRootNode.exposeZipIters().returnZipBegin()+i)->returnFeatureVal();
		float testFromData = (float)fpSingleton::getSingleton().returnFeatureVal(testFeature,i);
		EXPECT_EQ(testFromZip, testFromData);
		accumulator += testFromZip;
	}


	testRootNode.sortWorkingSet();

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
	std::string p2 = "res/iris.csv";
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
	testRootNode.loadWorkingSet(testFeature, numClasses);
	testRootNode.sortWorkingSet();

	testRootNode.findBestSplit(testFeature);

	EXPECT_EQ(testRootNode.exposeBestSplit().returnFeatureNum(), testFeature);
	EXPECT_EQ(testRootNode.exposePropertiesOfRightNode().returnNumItems(), 1);
	EXPECT_EQ(testRootNode.exposePropertiesOfLeftNode().returnNumItems(), 149);

	EXPECT_EQ(testRootNode.exposeBestSplit().returnImpurity(), 50);
}
