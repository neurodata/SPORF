#include "../../../src/forestTypes/newForest/inNodeClassTotals.h"
#include "../../../src/forestTypes/newForest/obsIndexAndClassVec.h"
#include "../../../src/forestTypes/newForest/nodeIterators.h"
#include <vector>

using namespace fp;
#define typeClass int


TEST(inNodeClassTotals, constructorTwo)
{
	int numClasses = 5;

	inNodeClassTotals testTotals(numClasses);

	EXPECT_EQ(testTotals.returnNumItems(), 0);
	testTotals.incrementClass(0);
	EXPECT_EQ(testTotals.returnNumItems(), 1);
	testTotals.incrementClass(1);
	EXPECT_EQ(testTotals.returnNumItems(), 2);
	testTotals.decrementClass(0);
	EXPECT_EQ(testTotals.returnNumItems(), 1);
	testTotals.decrementClass(1);
	EXPECT_EQ(testTotals.returnNumItems(), 0);
}


TEST(inNodeClassTotals, constructorOne)
{

	int numInClassOne = 5;
	int numInClassTwo = 7;
	int numInClassFour = 2;
	int numClasses = 5;

	obsIndexAndClassVec inputVec(numClasses);

	for(int i = 0; i < numInClassTwo; ++i){
		inputVec.insertIndex(i,2);
	}

	for(int i = 0; i < numInClassOne; ++i){
		inputVec.insertIndex(i,1);
	}

	for(int i = 0; i < numInClassFour; ++i){
		inputVec.insertIndex(i,4);
	}

	nodeIterators iterators(numClasses);
	iterators.setInitialIterators(inputVec);

	inNodeClassTotals testTotals(iterators);

	EXPECT_EQ(testTotals.returnNumItems(), numInClassOne+numInClassTwo+numInClassFour);
	EXPECT_EQ(testTotals.returnMaxClass(), 2);

	std::vector<float> classPers;
	testTotals.setClassPercentages(classPers);

	EXPECT_EQ(classPers[0], (float)0/(float)(numInClassOne+numInClassTwo+numInClassFour));
	EXPECT_EQ(classPers[1], (float)numInClassOne/(float)(numInClassOne+numInClassTwo+numInClassFour));
	EXPECT_EQ(classPers[2], (float)numInClassTwo/(float)(numInClassOne+numInClassTwo+numInClassFour));
	EXPECT_EQ(classPers[3], (float)0/(float)(numInClassOne+numInClassTwo+numInClassFour));
	EXPECT_EQ(classPers[4], (float)numInClassFour/(float)(numInClassOne+numInClassTwo+numInClassFour));
}


TEST(inNodeClassTotals, impurity)
{

	int numInClassOne = 5;
	int numInClassTwo = 5;
	int numInClassFour = 5;
	int numClasses = 5;

	obsIndexAndClassVec inputVec(numClasses);

	for(int i = 0; i < numInClassTwo; ++i){
		inputVec.insertIndex(i,2);
	}


	nodeIterators iterators(numClasses);
	iterators.setInitialIterators(inputVec);

	inNodeClassTotals testTotals(iterators);


	EXPECT_EQ(testTotals.returnNumItems(), numInClassOne);
	EXPECT_EQ(testTotals.calcAndReturnImpurity(), 0);
	EXPECT_EQ(testTotals.returnImpurity(), 0);
	EXPECT_TRUE(testTotals.isNodePure());

	for(int i = 0; i < numInClassOne; ++i){
		inputVec.insertIndex(i,1);
	}

	for(int i = 0; i < numInClassFour; ++i){
		inputVec.insertIndex(i,4);
	}


	nodeIterators iteratorsTwo(numClasses);
	iteratorsTwo.setInitialIterators(inputVec);

	inNodeClassTotals testTotalsTwo(iteratorsTwo);


	EXPECT_EQ(testTotalsTwo.returnNumItems(), numInClassOne+numInClassTwo+numInClassFour);
	EXPECT_GT(testTotalsTwo.calcAndReturnImpurity(), 9.95);
	EXPECT_LT(testTotalsTwo.returnImpurity(), 10.05);
	EXPECT_FALSE(testTotalsTwo.isNodePure());



	//	classes.resetClassTotals();
	//	classes.findClassCardinalities(inputTupleVector);

}


TEST(inNodeClassTotals, copyClassTotals)
{

	int numInClassOne = 5;
	int numInClassTwo = 5;
	int numInClassFour = 5;
	int numClasses = 5;


	obsIndexAndClassVec inputVec(numClasses);

	for(int i = 0; i < numInClassTwo; ++i){
		inputVec.insertIndex(i,2);
	}

	for(int i = 0; i < numInClassOne; ++i){
		inputVec.insertIndex(i,1);
	}

	for(int i = 0; i < numInClassFour; ++i){
		inputVec.insertIndex(i,4);
	}

	nodeIterators iterators(numClasses);
	iterators.setInitialIterators(inputVec);

	inNodeClassTotals testTotals(iterators);
	inNodeClassTotals testTotalsTwo(numClasses);

	testTotalsTwo.copyInNodeClassTotals(testTotals);


	EXPECT_EQ(testTotalsTwo.returnNumItems(), testTotals.returnNumItems());
	EXPECT_EQ(testTotalsTwo.calcAndReturnImpurity(), testTotals.calcAndReturnImpurity());
	EXPECT_GT(testTotalsTwo.returnImpurity(), 9.95);
	EXPECT_LT(testTotalsTwo.returnImpurity(), 10.05);


	testTotalsTwo.resetClassTotals();
	EXPECT_EQ(testTotalsTwo.returnNumItems(), 0);
	testTotalsTwo.incrementClass(1);
	EXPECT_EQ(testTotalsTwo.calcAndReturnImpurity(), 0);
	testTotalsTwo.incrementClass(2);
	EXPECT_EQ(testTotalsTwo.calcAndReturnImpurity(), 1);
	testTotalsTwo.incrementClass(2);
	EXPECT_GT(testTotalsTwo.calcAndReturnImpurity(), 1.3);
	EXPECT_LT(testTotalsTwo.returnImpurity(), 1.35);

}
