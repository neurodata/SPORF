// #include "../../src/baseFunctions/fpForestBase.h"
#include "../../src/forestTypes/basicForests/rfClassification/fpForestClassificationBase.h"
#include "../../src/fpSingleton/fpSingleton.h"

TEST(testMaxDepth, rfBaseMaxDepth){
  fpSingleton::getSingleton().setParameter("forestType", "rfBase");
  fpSingleton::getSingleton().setParameter("CSVFileName", "../res/iris.csv");
  fpSingleton::getSingleton().setParameter("columnWithY", 4);
  fpSingleton::getSingleton().setParameter("numTreesInForest", 10);
  fpSingleton::getSingleton().setParameter("minParent", 1);
  fpSingleton::getSingleton().setParameter("numCores", 1);
  fpSingleton::getSingleton().setParameter("seed",-1661580697);
  
  for(int setDepth = 1; setDepth < 10; ++setDepth){
    fpSingleton::getSingleton().setParameter("maxDepth", setDepth);
    fp::fpForestClassificationBase<double> forest;
    forest.growForest();
    std::map<std::string, int> stats = forest.calcTreeStats();
    EXPECT_LE(stats["maxDepth"], setDepth);
  }
}