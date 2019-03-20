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
  fpSingleton::getSingleton().setParameter("forestType", "binnedBaseRerF");

  randomNumberRerFMWC randNum;
  randNum.initialize(-1661580697);

  for (int setMtryMult = 1; setMtryMult < 10; ++setMtryMult)
  {
    fpSingleton::getSingleton().setParameter("mtryMult", setMtryMult);
    processingNodeBin<double, std::vector<int>> pNodeBin(0, 0, 0, randNum);
    // std::vector<std::vector<int>> mtry;
    pNodeBin.calcMtryForNode(&pNodeBin.mtry);
  }
}