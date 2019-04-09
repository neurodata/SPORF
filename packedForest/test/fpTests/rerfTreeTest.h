#include "../../src/forestTypes/basicForests/rerf/fpRerFBase.h"
#include "../../src/forestTypes/basicForests/rerf/rerfTree.h"
#include "../../src/fpSingleton/fpSingleton.h"
#include <vector>

TEST(testRerfTree, testOOB) {
  int totalCorrect = 0;
  bool yay = false;
  fp::rerfTree<double> tree;

  fp::fpSingleton::getSingleton().setParameter("CSVFileName", "../res/iris.csv");
  fp::fpSingleton::getSingleton().setParameter("columnWithY", 4);
  fp::fpSingleton::getSingleton().setParameter("numTreesInForest", 1);
  fp::fpSingleton::getSingleton().setParameter("minParent", 1);
  fp::fpSingleton::getSingleton().setParameter("numCores", 1);
  fp::fpSingleton::getSingleton().setParameter("seed", -1661580697);

  // needed when doing this from the tree level.
  fp::fpSingleton::getSingleton().setParameter("mtry", 1);
  fp::fpSingleton::getSingleton().loadData();
  fp::fpSingleton::getSingleton().loadTestData();

  // Must use growTreeTest or the OutSampleIndieces get deleted
  // after processing the nodes.
  std::vector<int> outSampleIndices = tree.growTreeTest();

  std::vector<int> predictedClass(outSampleIndices.size(), -1);

  for (long unsigned int i = 0; i < outSampleIndices.size(); i++) {
    predictedClass[i] = tree.predictObservation(outSampleIndices[i]);
    bool tmp = predictedClass[i] == outSampleIndices[i] / 50;
    //std::cout << "out sample index" << outSampleIndices[i] << "\n";
    //std::cout << "Pred class" << predictedClass[i] << "\n";
    //std::cout << tmp << "\n";
    if (tmp) {
      totalCorrect++;
    }
  }

  float internalResult = tree.returnOOB();
  float manualResult = (float)totalCorrect / (float)outSampleIndices.size();

  // compare internally computed OOB accuracy with
  // the manually computed within tollerance of 10^-9.
  yay = (internalResult - manualResult) < 0.000000001;

  EXPECT_TRUE(yay);
}
