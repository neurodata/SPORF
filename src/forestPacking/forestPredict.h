//This is improvement 6.  Builds on improv4. Build tree depth first based on usage stats instead of pre-order.
//This improv bins top levels then does a pseudo pre-order.

#ifndef forestPredict_h
#define forestPredict_h

#include "treeStruct/padForest.h"
#include "treeStruct/inferenceSamples.h"
#include "treeStruct/padNodeStat.h"
#include "treeStruct/treeBin2.h"
#include <vector>

//' @export forestPredict
class forestPredict : public padForest 
{
	private:
		treeBin2** forestRoots = NULL;
		int numOfBins;

	public:
		forestPredict(const std::string& forestFileName);
		int makePrediction(const std::vector<double>& observation);
		//int makePrediction(double*& observation, int numCore);
		
};


#endif //forestPredict_h
