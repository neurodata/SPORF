#include "../../src/forestTypes/basicForests/rfClassification/fpForestClassificationBase.h"
#include "../../src/forestTypes/basicForests/rfClassification/rfTree.h"
#include "../../src/fpSingleton/fpSingleton.h"
#include <vector>

TEST(testRFtree, testOOB){
	//fp::fpForest<double> forest;
  fp::fpForestClassificationBase<float> forest;
	//forest.setParameter("forestType", "rfBase");
  fp::fpSingleton::getSingleton().setParameter("CSVFileName", "../res/iris.csv");
  fp::fpSingleton::getSingleton().setParameter("columnWithY", 4);
  fp::fpSingleton::getSingleton().setParameter("numTreesInForest", 1);
  fp::fpSingleton::getSingleton().setParameter("minParent", 1);
  fp::fpSingleton::getSingleton().setParameter("numCores", 1);
  fp::fpSingleton::getSingleton().setParameter("seed",-1661580697);
  //fp::fpSingleton::getSingleton().loadData();

  //fp::rfTree<float> tree;

  //std::cout << fp::fpSingleton::getSingleton().returnNumObservations();

  //std::vector<int> outSamples = tree.growTreeTest();

  //for (auto outSamp : outSamples){
  //  std::cout << outSamp << ",";
  //}

  
  std::vector<int> outSamples = forest.growForestTest();

  std::cout << "Out Sample Indices are:\n";

  for (auto i = 0; i < outSamples.size(); ++i){
    std::cout << outSamples[i] << ",";
  }
  std::cout << "\n";

  //std::vector<float> testObs = { 5.1, 3.5, 1.4, 0.2 };

  //std::vector<int> testObs = {};
  //std::vector<int> outPredictions = forest.predictClass();
  //std::cout << "Out Sample Predictions are:\n";
  //for (auto i = 0; i < outPredictions.size(); ++i){
  //  std::cout << outPredictions[i] << ",";
  //}
  //std::cout << "\n";

  float oobAcc = forest.reportOOB();
  std::cout << "Reporting OOB accuracy\n" << oobAcc << "\n";

	//std::vector<int> results;
	//std::vector<double> testCase {5.1,3.5,1.4,0.2};
	//results = forest.predictPost(testCase);
}