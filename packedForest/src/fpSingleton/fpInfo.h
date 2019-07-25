#ifndef fpInfo_h
#define fpInfo_h

#include <string>
#include <memory>
#include <math.h>
#include <limits>
#include <iostream>
#include "../baseFunctions/MWC.h"

namespace fp {

	/**
	 * fpInfo holds all parameters for the forests.  These parameters should be
	 * set before calling fpForest to run forest creation.
	 */

	class fpInfo{

		protected:
			int numTreesInForest;
			int minParent;
			int numClasses;
			int numObservations;
			int numFeatures;
			int mtry;
			double mtryMult;
			int columnWithY;

			// For use when picking a RandMat implementation
			int methodToUse;

			// For Structured-RerF
			int imageHeight;
			int imageWidth;
			int patchHeightMin;
			int patchHeightMax;
			int patchWidthMin;
			int patchWidthMax;

			int numberOfNodes;
			int maxDepth;
			int sumLeafNodeDepths;

			double fractionOfFeaturesToTest;

			int binSize;
			int binMin;
			int numCores;

			std::string forestType;
			std::string CSVFileName;

			int seed;
			randomNumberRerFMWC randNum;

			int numTreeBins;
			bool useRowMajor;


		public:

			inline void resetInfo(){
				numTreesInForest=100;
				minParent=1;
				numClasses=-1;
				numObservations=-1;
				numFeatures=-1;
				mtry=-1;
				mtryMult=1;
				columnWithY=-1;
				numberOfNodes=0;
				maxDepth=std::numeric_limits<int>::max();
				sumLeafNodeDepths=0;
				fractionOfFeaturesToTest=-1.0; 
				binSize=0;
				numCores=1;
				seed=-1;
				numTreeBins=-1;
				methodToUse = 1; // Should this default to 1?
				imageHeight = 0;
				imageWidth = 0;
				patchHeightMin = 0;
				patchHeightMax = 0;
				patchWidthMin = 0;
				patchWidthMax = 0;
				forestType.clear();
				CSVFileName.clear();
				//initRandom();
			}


			inline int returnNumTreeBins(){
				return numTreeBins;
			}

			inline void setNumTreeBins(int numTB){
				numTreeBins = numTB;
			}

			inline bool returnUseBinning(){
				return binMin;
			}

			inline int returnBinSize(){
				return binSize;
			}

			inline int returnBinMin(){
				return binMin;
			}

			inline std::string& returnCSVFileName(){
				return CSVFileName;
			}

			inline std::string& returnForestType(){
				return forestType;
			}

			inline int returnColumnWithY() const{
				return columnWithY;
			}

			inline int returnNumClasses() const{
				return numClasses;
			}

			inline int returnMinParent() const{
				return minParent;
			}
			inline int returnMaxDepth() const{
				return maxDepth;
			}
		
			inline int returnNumFeatures() const{
				return numFeatures;
			}
			inline int returnNumObservations() const{
				return numObservations;
			}

			inline void setNumClasses(const int& numC){
				numClasses = numC;
			}

			inline void setNumFeatures(const int& numF){
				numFeatures = numF;
			}

			inline void setNumObservations(const int& numO){
				numObservations = numO;
			}

			inline int returnNumTrees() const{
				return numTreesInForest;
			}

			inline int returnNumThreads() const{
				return numCores;
			}

			inline bool loadDataFromCSV(){
				if(!CSVFileName.empty() || columnWithY != -1){
					return true;
				}
				return false;
			}

			inline int returnMtry(){
				return mtry;
			}
			inline double returnMtryMult(){
				return mtryMult;
			}

			inline bool useDefaultMTRY(){
				return fractionOfFeaturesToTest < 0;
			}
			inline bool returnUseRowMajor(){
				return useRowMajor;
			}

			inline void setMTRY(){
				if(mtry == -1){
					if(useDefaultMTRY()){
						mtry = sqrt(numFeatures);
					}else{
						mtry = fractionOfFeaturesToTest * numFeatures;
					}
				}
			}


			inline int returnMethodToUse(){
			  return methodToUse;
			}

			// For Structured RerF
			inline int returnImageHeight(){
			  return imageHeight;
			}

			inline int returnImageWidth(){
			  return imageWidth;
			}

			inline int returnPatchHeightMax(){
			  return patchHeightMax;
			}

			inline int returnPatchHeightMin(){
			  return patchHeightMin;
			}

			inline int returnPatchWidthMax(){
			  return patchWidthMax;
			}

			inline int returnPatchWidthMin(){
			  return patchWidthMin;
			}


			////////////////////////////////////////
			//Random Number Generator
			///////////////////////////////////////
			inline void initRandom(){
				if(seed == -1){
					std::random_device rd;
					seed = rd();
				}
				randNum.initialize(seed);
			}

			inline int genRandom(int range){
				return randNum.gen(range);
			}



			fpInfo(): numTreesInForest(100),
			minParent(1),	numClasses(-1), numObservations(-1), numFeatures(-1),
			mtry(-1),mtryMult(1), columnWithY(-1),
			methodToUse(1), imageHeight(0), imageWidth(0), patchHeightMin(0), patchHeightMax(0), patchWidthMin(0), patchWidthMax(0),
			numberOfNodes(0), maxDepth(std::numeric_limits<int>::max()),sumLeafNodeDepths(0), fractionOfFeaturesToTest(-1.0), binSize(0),binMin(0),numCores(1),seed(-1),numTreeBins(-1),  useRowMajor(true){}



			void setParameter(const std::string& parameterName, const std::string& parameterValue){
				if(parameterName == "forestType"){
					forestType = parameterValue;
				}else if(parameterName == "CSVFileName"){
					CSVFileName = parameterValue;
				}else{
					throw std::runtime_error("Unknown parameter type.(string)");
				}
			}


			void setParameter(const std::string& parameterName, const double parameterValue){
				if(parameterName == "numTreesInForest"){
					numTreesInForest = (int)parameterValue;
				}else if(parameterName == "minParent"){
					minParent = (int)parameterValue;
				}else if(parameterName == "maxDepth"){
					maxDepth = (int)parameterValue;
				}else if(parameterName == "numClasses"){
					numClasses = (int)parameterValue;
				}else if(parameterName == "mtry"){
					mtry = (int)parameterValue;
				}else if(parameterName == "mtryMult"){
					mtryMult = parameterValue;
				}else if(parameterName == "fractionOfFeaturesToTest"){
					fractionOfFeaturesToTest = parameterValue;
				}else if(parameterName == "columnWithY"){
					columnWithY = (int)parameterValue;
				}else if(parameterName == "binSize"){
					binSize = (int)parameterValue;
				}else if(parameterName == "binMin"){
					binMin = (int)parameterValue;
				}else if(parameterName == "numCores"){
					numCores = (int)parameterValue;
				}else if(parameterName == "seed"){
					seed = (int)parameterValue;
				}else if(parameterName == "numTreeBins"){
					numTreeBins = (int)parameterValue;
				}else if(parameterName == "useRowMajor"){
					useRowMajor = (bool)parameterValue;
				}else {
					throw std::runtime_error("Unknown parameter type.(double)");
				}
			}


			void setParameter(const std::string& parameterName, const int parameterValue){
				if(parameterName == "numTreesInForest"){
					numTreesInForest = parameterValue;
				}else if(parameterName == "minParent"){
					minParent = parameterValue;
				}else if(parameterName == "maxDepth"){
					maxDepth = parameterValue;
				}else if(parameterName == "numClasses"){
					numClasses = parameterValue;
				}else if(parameterName == "mtry"){
					mtry = parameterValue;
				}else if(parameterName == "mtryMult"){
					mtryMult = (double)parameterValue;
				}else if(parameterName == "fractionOfFeaturesToTest"){
					fractionOfFeaturesToTest = (double)parameterValue;
				}else if(parameterName == "columnWithY"){
					columnWithY = parameterValue;
				}else if(parameterName == "binSize"){
					binSize = parameterValue;
				}else if(parameterName == "binMin"){
					binMin = parameterValue;
				}else if(parameterName == "numCores"){
					numCores = parameterValue;
				}else if(parameterName == "seed"){
					seed = parameterValue;
				}else if(parameterName == "numTreeBins"){
					numTreeBins = parameterValue;
				}else if(parameterName == "useRowMajor"){
					useRowMajor = (bool)parameterValue;
				}else if(parameterName == "methodToUse"){
					methodToUse = parameterValue;
					if(!(methodToUse == 1 || methodToUse == 2)){
						throw std::runtime_error("methodToUse outside allowable parameters {1,2}.");
					}
				}else if(parameterName == "imageHeight"){
					imageHeight = parameterValue;
				}else if(parameterName == "imageWidth"){
					imageWidth = parameterValue;
				}else if(parameterName == "patchHeightMax"){
					patchHeightMax = parameterValue;
				}else if(parameterName == "patchHeightMin"){
					patchHeightMin = parameterValue;
				}else if(parameterName == "patchWidthMax"){
					patchWidthMax = parameterValue;
				}else if(parameterName == "patchWidthMin"){
					patchWidthMin = parameterValue;
				}else {
					throw std::runtime_error("Unknown parameter type.(int)");
				}
			}

			void printAllParameters(){
				std::cout << "numTreesInForest -> " << numTreesInForest << "\n";
				std::cout << "maxDepth -> " << maxDepth << "\n";
				std::cout << "minParent -> " << minParent << "\n";
				std::cout << "numClasses -> " << numClasses << "\n";
				std::cout << "numObservations -> " << numObservations << "\n";
				std::cout << "numFeatures -> " << numFeatures << "\n";
				std::cout << "mtry -> " << mtry << "\n";
				std::cout << "mtryMult -> " << mtryMult << "\n";
				std::cout << "fractionOfFeaturesToTest -> " << fractionOfFeaturesToTest << "\n";
				std::cout << "CSV file name -> " <<  CSVFileName << "\n";
				std::cout << "columnWithY -> " << columnWithY << "\n";
				std::cout << "Type of Forest -> " << forestType << "\n";
				std::cout << "binSize -> " << binSize << "\n";
				std::cout << "binMin -> " << binMin << "\n";
				std::cout << "numCores -> " << numCores << "\n";
				std::cout << "seed -> " << seed << "\n";
				std::cout << "numTreeBins -> " << numTreeBins << "\n";

				if(methodToUse == 2){
					std::cout << "imageHeight -> " << imageHeight << "\n";
					std::cout << "imageWidth -> " << imageWidth << "\n";
					std::cout << "patchHeightMax -> " << patchHeightMax << "\n";
					std::cout << "patchHeightMin -> " << patchHeightMin << "\n";
					std::cout << "patchWidthMax -> " << patchWidthMax << "\n";
					std::cout << "patchWidthMin -> " << patchWidthMin << "\n";
				}
			}


			void printForestType(){
				std::cout << forestType << "\n";
			}

	}; // class fpInfo
} //namespace fp
#endif //fpInfo.h
