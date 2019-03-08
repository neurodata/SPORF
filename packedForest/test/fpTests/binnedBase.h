#include "../../src/forestTypes/binnedTree/binnedBase.h"
#include "../../src/fpSingleton/fpSingleton.h"
#include <map>

TEST(testBinnedBase, testBaseMaxDepth){
  fpSingleton::getSingleton().setParameter("CSVFileName", "../res/iris.csv");
  fpSingleton::getSingleton().setParameter("columnWithY", 4);
  fpSingleton::getSingleton().setParameter("numTreesInForest", 10);
  fpSingleton::getSingleton().setParameter("minParent", 1);
  fpSingleton::getSingleton().setParameter("numCores", 1);
  fpSingleton::getSingleton().setParameter("seed",-1661580697);
  
  for(int setDepth = 1; setDepth < 10; ++setDepth){
    fpSingleton::getSingleton().setParameter("maxDepth", setDepth);
    fp::binnedBase<double, int> forest;
    forest.growForest();
    std::map<std::string, int> stats = forest.calcBinStats();
    EXPECT_LE(stats["maxDepth"], setDepth);
  }
}

TEST(testBinnedBase, testRerFMaxDepth){
  fpSingleton::getSingleton().setParameter("CSVFileName", "../res/iris.csv");
  fpSingleton::getSingleton().setParameter("columnWithY", 4);
  fpSingleton::getSingleton().setParameter("numTreesInForest", 10);
  fpSingleton::getSingleton().setParameter("minParent", 1);
  fpSingleton::getSingleton().setParameter("numCores", 1);
  fpSingleton::getSingleton().setParameter("seed",-1661580697);
  
  for(int setDepth = 1; setDepth < 10; ++setDepth){
    fpSingleton::getSingleton().setParameter("maxDepth", setDepth);
    fp::binnedBase<double, std::vector<int>> forest;
    forest.growForest();
    std::map<std::string, int> stats = forest.calcBinStats();
    EXPECT_LE(stats["maxDepth"], setDepth);
  }
}