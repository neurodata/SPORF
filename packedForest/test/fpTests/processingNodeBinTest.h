#include "../../src/fpSingleton/fpSingleton.h"
#include "../../src/forestTypes/binnedTree/processingNode.h"
#include "../../src/baseFunctions/MWC.h"
#include <vector>

TEST(testProcessingNodeBin, testMtryMult)
{
  fpSingleton::getSingleton().setParameter("CSVFileName", "../res/iris.csv");
  fpSingleton::getSingleton().setParameter("columnWithY", 4);
  fpSingleton::getSingleton().setParameter("numTreesInForest", 10);
  fpSingleton::getSingleton().setParameter("minParent", 1);
  fpSingleton::getSingleton().setParameter("numCores", 1);
  fpSingleton::getSingleton().setParameter("mtry", 2);
  // fpSingleton::getSingleton().setParameter("forestType", "binnedBaseRerF");

  randomNumberRerFMWC randNum;
  randNum.initialize(-1661580697);
  int treeNum = 1;
  int nodeNum = 0;
  int depth = 0;
  processingNodeBin<double, std::vector<int>> pNodeBin(treeNum, nodeNum, depth, randNum);

  for (int setMtryMult = 1; setMtryMult < 10; ++setMtryMult)
  {
    fpSingleton::getSingleton().setParameter("mtryMult", setMtryMult);

    std::vector<int> featuresToTry(2);
    pNodeBin.calcMtryForNodeTest(featuresToTry);
  }
}