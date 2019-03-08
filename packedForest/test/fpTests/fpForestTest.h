// This tests whether the seed works in single/parallel execution

#include "../../src/packedForest.h"

TEST(testAUCReturn, AUCReturnBinnedBase)
{
	fp::fpForest<double> forest;
	forest.setParameter("forestType", "binnedBase");
	forest.setParameter("CSVFileName", "res/iris.csv");
	forest.setParameter("columnWithY", 4);
	forest.setParameter("numTreesInForest", 10);
	forest.setParameter("minParent", 1);
	forest.setParameter("numCores", 1);
	forest.setParameter("seed",-1661580697);
	forest.growForest();

	std::vector<int> results;
	std::vector<double> testCase {5.1,3.5,1.4,0.2};
	results = forest.predictPost(testCase);

	EXPECT_EQ(results.size(), 3);
	EXPECT_EQ(results[0], 10);
	EXPECT_EQ(results[1], 0);
	EXPECT_EQ(results[2], 0);
}

TEST(testAUCReturn, AUCReturnBinnedBaseRerF)
{
	fp::fpForest<double> forest;
	forest.setParameter("forestType", "binnedBaseRerF");
	forest.setParameter("CSVFileName", "res/iris.csv");
	forest.setParameter("columnWithY", 4);
	forest.setParameter("numTreesInForest", 10);
	forest.setParameter("minParent", 1);
	forest.setParameter("numCores", 1);
	forest.setParameter("seed",-1661580697);
	forest.growForest();

	std::vector<int> results;
	std::vector<double> testCase {5.1,3.5,1.4,0.2};
	results = forest.predictPost(testCase);

	EXPECT_EQ(results.size(), 3);
	EXPECT_EQ(results[0], 10);
	EXPECT_EQ(results[1], 0);
	EXPECT_EQ(results[2], 0);
}

TEST(testAUCReturn, AUCReturnrfBase)
{
	fp::fpForest<double> forest;
	forest.setParameter("forestType", "rfBase");
	forest.setParameter("CSVFileName", "res/iris.csv");
	forest.setParameter("columnWithY", 4);
	forest.setParameter("numTreesInForest", 10);
	forest.setParameter("minParent", 1);
	forest.setParameter("numCores", 1);
	forest.setParameter("seed",-1661580697);
	forest.growForest();

	std::vector<int> results;
	std::vector<double> testCase {5.1,3.5,1.4,0.2};
	results = forest.predictPost(testCase);

	EXPECT_EQ(results.size(), 3);
	EXPECT_EQ(results[0], 10);
	EXPECT_EQ(results[1], 0);
	EXPECT_EQ(results[2], 0);
}

TEST(testAUCReturn, AUCReturnrerf)
{
	fp::fpForest<double> forest;
	forest.setParameter("forestType", "rerf");
	forest.setParameter("CSVFileName", "res/iris.csv");
	forest.setParameter("columnWithY", 4);
	forest.setParameter("numTreesInForest", 10);
	forest.setParameter("minParent", 1);
	forest.setParameter("numCores", 1);
	forest.setParameter("seed",-1661580697);
	forest.growForest();

	std::vector<int> results;
	std::vector<double> testCase {5.1,3.5,1.4,0.2};
	results = forest.predictPost(testCase);

	EXPECT_EQ(results.size(), 3);
	EXPECT_EQ(results[0], 10);
	EXPECT_EQ(results[1], 0);
	EXPECT_EQ(results[2], 0);
}
