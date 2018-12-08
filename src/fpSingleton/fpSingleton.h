#ifndef fpSingleton_h
#define fpSingleton_h

#include "fpData.h"
#include "fpInfo.h"
#include <string>

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
			static fpSingleton& getSingleton(){
				static fpSingleton* infoSetting(new fpSingleton);
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

			inline void loadData(){
				data.fpLoadData(fpForestInfo);
			}

			inline void loadTestData(){
				data.fpLoadTestData(fpForestInfo);
			}

			inline void deleteData(){
				data.fpDeleteData();
			}

			inline void deleteTestData(){
				data.fpDeleteTestData();
			}


			inline void setNumFeatures(int numF){
				fpForestInfo.setNumFeatures(numF);
			}

			inline void setNumObservations(int numO){
				fpForestInfo.setNumObservations(numO);
			}

			inline void setNumClasses(int numC){
				fpForestInfo.setNumClasses(numC);
			}

			inline int returnNumClasses(){
				return fpForestInfo.returnNumClasses();
			}

			inline int returnMtry(){
				return fpForestInfo.returnMtry();
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

			inline int returnMinParent(){
				return fpForestInfo.returnMinParent();
			}

			inline int returnBinSize(){
				return fpForestInfo.returnBinSize();
			}

			inline bool returnUseBinning(){
				return fpForestInfo.returnUseBinning();
			}


			inline void setDataDependentParameters(){
				fpForestInfo.setMTRY();
			}


		private: //These are singleton specific methods
			fpSingleton(){}
			~fpSingleton();
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
