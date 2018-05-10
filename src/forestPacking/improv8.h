//This is improvement 6.  Builds on improv4. Build tree depth first based on usage stats instead of pre-order.
//This improv bins top levels then does a pseudo pre-order.

#ifndef improv8_h
#define improv8_h

#include "treeStruct/padForest.h"
#include "treeStruct/inferenceSamples.h"
#include "treeStruct/padNodeStat.h"
#include "treeStruct/treeBin2.h"

class improv8: public padForest 
{
	private:
		treeBin2** forestRoots = NULL;
		int numOfBins;

	public:
		improv8(const std::string& forestCSVFileName, int source, const inferenceSamples& observations, int numberBins, int depthIntertwined);
		improv8(const std::string& forestFileName);
		~improv8();
		void makePredictions(const inferenceSamples& observations);
void makePrediction(double* observation, double* preds, int numFeatures, int numObservations, int numCores);
		int makePrediction(double*& observation);
		int makePrediction(double*& observation, int numCore);
		void writeForest(const std::string& forestFileName);
		void changeNumBins(int newBinNum){
			numOfBins = newBinNum;
		}
		int numbin(){
			return numOfBins;
		}
};


#endif //improv8_h
