#ifndef fpSingleton_h
#define fpSingleton_h

#include "fpData.h"
#include "fpInfo.h"
#include <string>
#include <memory>

namespace fp {

	/**
	 * fpSingleton combines the data (fpData) and the forest parameters (fpInfo).
	 * As a singleton, fpSingleton is available to all functions.
	 */

	class fpSingleton{

		private:
			static fpSingleton* infoSetting;
			fpInfo fpForestInfo;
			fpData data;


		public:
			inline int returnNumTreeBins(){
				return fpForestInfo.returnNumTreeBins();
			}

			inline void setNumTreeBins(int numTB){
				fpForestInfo.setNumTreeBins(numTB);
			}

			static fpSingleton& getSingleton(){
				static std::unique_ptr<fpSingleton> infoSetting(new fpSingleton);
				return *infoSetting;
			}

			inline void setParameter(const std::string& parameterName, const std::string& parameterValue){
				fpForestInfo.setParameter(parameterName, parameterValue);
			}

			inline void setParameter(const std::string& parameterName, const double parameterValue){
				fpForestInfo.setParameter(parameterName, parameterValue);
			}

			inline void setParameter(const std::string& parameterName, const int parameterValue){
				fpForestInfo.setParameter(parameterName, parameterValue);
			}

			inline void printAllParameters(){
				fpForestInfo.printAllParameters();
			}

			inline void printForestType(){
				fpForestInfo.printForestType();
			}

			inline void loadData(const double* Xmat, const int* Yvec, int numObs, int numFeatures){
				data.fpLoadData(Xmat,Yvec,numObs,numFeatures, fpForestInfo);
			}

			inline void loadData(){
				data.fpLoadData(fpForestInfo);
			}

			inline void loadTestData(){
				data.fpLoadTestData(fpForestInfo);
			}

			inline void deleteData(){
				data.fpDeleteData();
			}

			inline void resetSingleton(){
				fpForestInfo.resetInfo();
			}

			inline void deleteTestData(){
				data.fpDeleteTestData();
			}

			inline void printXValues(){
				data.printXValues();
			}

			inline void setNumFeatures(int numF){
				fpForestInfo.setNumFeatures(numF);
			}

			////////////////////////////////////////
			//Random Number Generator
			///////////////////////////////////////
			inline void initializeRandomNumberGenerator(){
				fpForestInfo.initRandom();
			}

			inline int genRandom(int range){
				return fpForestInfo.genRandom(range);
			}

			///////////////////////////////////////
			//
			//////////////////////////////////////

			inline void setNumObservations(int numO){
				fpForestInfo.setNumObservations(numO);
			}

			inline void setNumClasses(int numC){
				fpForestInfo.setNumClasses(numC);
			}

			inline float returnFloatEpsilon(){
				return fpForestInfo.returnFloatEpsilon();
			}

			inline double returnDoubleEpsilon(){
				return fpForestInfo.returnDoubleEpsilon();
			}

			inline int returnNumClasses(){
				return fpForestInfo.returnNumClasses();
			}

			inline int returnMtry(){
				return fpForestInfo.returnMtry();
			}

			inline double returnMtryMult(){
				return fpForestInfo.returnMtryMult();
			}

			inline std::string& returnForestType(){
				return fpForestInfo.returnForestType();
			}

			inline int returnNumFeatures(){
				return fpForestInfo.returnNumFeatures();
			}

			inline int returnNumObservations() const{
				return fpForestInfo.returnNumObservations();
			}

			inline int returnLabel(int observationNumber){
				return data.returnLabel(observationNumber);
			}

			inline int returnTestLabel(int observationNumber){
				return data.returnTestLabel(observationNumber);
			}

			inline double returnFeatureVal(const int featureNumber, const int observationNumber){
				return data.returnFeatureVal(featureNumber, observationNumber);
			}

			inline void prefetchFeatureVal(const int featureNumber, const int observationNumber){
				data.prefetchFeatureVal(featureNumber, observationNumber);
			}

			inline double returnTestFeatureVal(const int featureNumber, const int observationNumber){
				return data.returnTestFeatureVal(featureNumber, observationNumber);
			}

			inline int returnNumTrees(){
				return fpForestInfo.returnNumTrees();
			}

			inline int returnNumThreads(){
				return fpForestInfo.returnNumThreads();
			}

			inline int returnMinParent(){
				return fpForestInfo.returnMinParent();
			}

			inline int returnMaxDepth(){
				return fpForestInfo.returnMaxDepth();
			}

			inline int returnBinSize(){
				return fpForestInfo.returnBinSize();
			}

			inline int returnBinMin(){
				return fpForestInfo.returnBinSize();
			}

			inline bool returnUseBinning(){
				return fpForestInfo.returnUseBinning();
			}


			inline void setDataDependentParameters(){
				fpForestInfo.setMTRY();
				initializeRandomNumberGenerator();
			}


		private: //These are singleton specific methods
			fpSingleton(){}
			//~fpSingleton();
			fpSingleton(const fpSingleton &old){
				if(this != &old){
					infoSetting= old.infoSetting;
				}
			}
			const fpSingleton &operator=(const fpSingleton &old){
				if(this !=&old){
					infoSetting=old.infoSetting;
				}
				return *this;
			}

	}; // class fpSingleton
	fpSingleton * fpSingleton::infoSetting = nullptr;

} //namespace fp
#endif //fpSingleton.h
