#ifndef fpData_h
#define fpData_h

#include "fpDataSet.h"
#include "fpInfo.h"
#include <string>
//#include <memory>

namespace fp {

	class fpData{

		protected:
			// data input
		//	std::string forestCSVFileName;
		//	int columnWithY;
			inputData<double, int>* inData;
//			fpSingleton* forestInfo;

		public:

			fpData(){
			//forestInfo = fpSingleton::getSingleton();
			}

			~fpData(){
		delete inData;	
			}

inline bool loadDataFromCSV(fpInfo& settings){
return settings.loadDataFromCSV();
}
			void fpLoadData(fpInfo& settings){
				if(loadDataFromCSV(settings)){
					inData = new inputData<double,int>(settings.returnCSVFileName(), settings.returnColumnWithY());
				}else {
					throw std::runtime_error("Unable to read data." );
				}
						setDataRelatedParameters(settings);
			}

			inline int returnNumClasses(){
				return inData->returnNumClasses();
			}

			inline int returnNumFeatures(){
				return inData->returnNumFeatures();
			}

			inline int returnNumObservations(){
				return inData->returnNumObservations();
			}

			inline int returnLabel(int observationNumber){
return inData->returnClassOfObservation(observationNumber);
			}

			inline int returnFeatureVal(const int featureNumber, const int observationNumber){
return inData->returnFeatureValue(featureNumber, observationNumber);
			}

			inline void setDataRelatedParameters(fpInfo& settings){
settings.setNumFeatures(this->returnNumFeatures());
settings.setNumObservations(this->returnNumObservations());
settings.setNumClasses(this->returnNumClasses());
			}

	}; // class fpData
} //namespace fp
#endif //fpData.h
