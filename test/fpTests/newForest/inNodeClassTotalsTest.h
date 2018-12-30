#include "../../../src/forestTypes/newForest/inNodeClassTotals.h"
#include <vector>

using namespace fp;
#define typeClass int


TEST(inNodeClassTotals, loadInNodeClassTotals)
{

	int numInClassOne = 5;
	int numInClassTwo = 7;
	int numInClassFour = 2;

	std::vector<obsIndexAndClassTuple<typeClass> > inputTupleVector;
	inNodeClassTotals<typeClass> classes;


	for(int i = 0; i < numInClassTwo; ++i){
		inputTupleVector.emplace_back();
		inputTupleVector.back().setObsClass(2);
		inputTupleVector.back().setObsIndex(i);
	}



	for(int i = 0; i < numInClassOne; ++i){
		inputTupleVector.emplace_back();
		inputTupleVector.back().setObsClass(1);
		inputTupleVector.back().setObsIndex(i);
	}

	for(int i = 0; i < numInClassFour; ++i){
		inputTupleVector.emplace_back();
		inputTupleVector.back().setObsClass(4);
		inputTupleVector.back().setObsIndex(i);
	}

	classes.findClassCardinalities(inputTupleVector);

	EXPECT_EQ(classes.returnNumItems(), numInClassOne+numInClassTwo+numInClassFour);
	EXPECT_EQ(classes.returnLargestClass(), 2);

	classes.resetClassTotals();

	EXPECT_EQ(classes.returnNumItems(), 0);
	EXPECT_EQ(classes.returnLargestClass(), 0);//all classes are 0 so defaults to first class

}




TEST(inNodeClassTotals, impurity)
{

	int numInClassOne = 5;
	int numInClassTwo = 5;
	int numInClassFour = 5;


	std::vector<obsIndexAndClassTuple<typeClass> > inputTupleVector;
	inNodeClassTotals<typeClass> classes;

	for(int i = 0; i < numInClassOne; ++i){
		inputTupleVector.emplace_back();
		inputTupleVector.back().setObsClass(1);
		inputTupleVector.back().setObsIndex(i);
	}

	classes.findClassCardinalities(inputTupleVector);

	EXPECT_EQ(classes.returnNumItems(), numInClassOne);
	EXPECT_EQ(classes.calcAndReturnImpurity(), 0);
	EXPECT_EQ(classes.returnImpurity(), 0);

	for(int i = 0; i < numInClassTwo; ++i){
		inputTupleVector.emplace_back();
		inputTupleVector.back().setObsClass(2);
		inputTupleVector.back().setObsIndex(i);
	}

	for(int i = 0; i < numInClassFour; ++i){
		inputTupleVector.emplace_back();
		inputTupleVector.back().setObsClass(4);
		inputTupleVector.back().setObsIndex(i);
	}


	classes.resetClassTotals();
	classes.findClassCardinalities(inputTupleVector);

	EXPECT_EQ(classes.returnNumItems(), numInClassOne+numInClassTwo+numInClassFour);
	EXPECT_GT(classes.calcAndReturnImpurity(), 9.95);
	EXPECT_LT(classes.returnImpurity(), 10.05);

}


TEST(inNodeClassTotals, copyAndDecrement)
{

	int numInClassOne = 5;
	int numInClassTwo = 5;


	std::vector<obsIndexAndClassTuple<typeClass> > inputTupleVector;

	inNodeClassTotals<typeClass> classes;
	inNodeClassTotals<typeClass> classesCopy;

	inNodeClassTotals<typeClass> classesEmpty;

	for(int i = 0; i < numInClassOne; ++i){
		inputTupleVector.emplace_back();
		inputTupleVector.back().setObsClass(1);
		inputTupleVector.back().setObsIndex(i);
	}

	for(int i = 0; i < numInClassTwo; ++i){
		inputTupleVector.emplace_back();
		inputTupleVector.back().setObsClass(2);
		inputTupleVector.back().setObsIndex(i);
	}

	classes.findClassCardinalities(inputTupleVector);
classesCopy.copyInNodeClassTotals(classes);

	EXPECT_EQ(classesCopy.returnNumItems(), numInClassOne + numInClassTwo);
	EXPECT_EQ(classesCopy.calcAndReturnImpurity(), 5);
	EXPECT_EQ(classesCopy.returnImpurity(), 5);

	classesEmpty.copyProperties(classesCopy);
for(int i = 0; i < numInClassOne; ++i){
	classesCopy.decrementClass(2);
	classesEmpty.incrementClass(2);
}


	EXPECT_EQ(classesCopy.returnNumItems(), numInClassOne);
	EXPECT_EQ(classesCopy.calcAndReturnImpurity(), 0);
	EXPECT_EQ(classesCopy.returnImpurity(), 0);

	EXPECT_EQ(classesEmpty.returnNumItems(), numInClassTwo);
	EXPECT_EQ(classesEmpty.calcAndReturnImpurity(), 0);
	EXPECT_EQ(classesEmpty.returnImpurity(), 0);
}
