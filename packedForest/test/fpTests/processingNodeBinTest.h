#include "../../src/fpSingleton/fpSingleton.h"
#include "../../src/forestTypes/binnedTree/processingNode.h"
#include "../../src/baseFunctions/MWC.h"
#include <vector>

TEST(testProcessingNodeBin, testMtryMult)
{
  int mtry = 2;

  fpSingleton::getSingleton().setParameter("CSVFileName", "../res/iris.csv");
  fpSingleton::getSingleton().setParameter("columnWithY", 4);
  fpSingleton::getSingleton().setParameter("mtry", mtry);
  fpSingleton::getSingleton().loadData();
  fpSingleton::getSingleton().setDataDependentParameters();

  int treeNum = 1;
  int nodeNum = 0;
  int depth = 0;

  randomNumberRerFMWC randNum;
  std::random_device rd;
  int seed = rd();
  randNum.initialize(seed);

  processingNodeBin<double, std::vector<int>> pNodeBin(treeNum, nodeNum, depth, randNum);

  for (double setMtryMult = 1; setMtryMult < 4; setMtryMult+=0.5)
  {
    fpSingleton::getSingleton().setParameter("mtryMult", setMtryMult);

    std::vector<std::vector<int>> f2Try;
    pNodeBin.calcMtryForNodeTest(f2Try);

    EXPECT_EQ(f2Try.size(), mtry);

    std::vector<int> features_sampled;
    for (auto i : f2Try)
    {
      for (auto ii : i)
      {
        features_sampled.push_back(ii);
        EXPECT_GE(ii, 0);
        EXPECT_LT(ii, fpSingleton::getSingleton().returnNumFeatures());
      }
    }
    EXPECT_EQ(features_sampled.size(), (int)(setMtryMult * mtry));

    //printing
    // std::cout << "mtryMult: " << fpSingleton::getSingleton().returnMtryMult() << "\n";
    // int mtry = 0;
    // for (auto i : f2Try)
    // {
    //   std::cout << mtry++ << ": ";
    //   for (auto ii : i)
    //   {
    //     std::cout << ii << ",";
    //   }
    //   std::cout << "\n";
    // }
  }
}