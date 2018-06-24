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
			testData<double, int>* inTestData;
			//			fpSingleton* forestInfo;

		public:

			fpData(){
				//forestInfo = fpSingleton::getSingleton();
			}

			~fpData(){
if(inData != NULL){
					delete inData;
					inData = NULL;
				}
if(inTestData != NULL){
					delete inTestData;
					inTestData = NULL;
				}
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


			void fpDeleteData(){
				if(inData != NULL){
					delete inData;
					inData = NULL;
				}else {
					throw std::runtime_error("Unable to delete data.  Data does not exist." );
				}
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

			inline double returnFeatureVal(const int featureNumber, const int observationNumber){
				return inData->returnFeatureValue(featureNumber, observationNumber);
			}

			inline void setDataRelatedParameters(fpInfo& settings){
				settings.setNumFeatures(this->returnNumFeatures());
				settings.setNumObservations(this->returnNumObservations());
				settings.setNumClasses(this->returnNumClasses());
			}

inline int returnNumTestObservations(){
				return inTestData->returnNumObservations();
			}
			
inline void setTestDataRelatedParameters(fpInfo& settings){
				settings.setNumObservations(this->returnNumTestObservations());
			}

			void fpLoadTestData(fpInfo& settings){
				if(loadDataFromCSV(settings)){
					inTestData = new testData<double,int>(settings.returnCSVFileName(), settings.returnColumnWithY());
				}else {
					throw std::runtime_error("Unable to read test data." );
				}
				setTestDataRelatedParameters(settings);
			}



void fpDeleteTestData(){
				if(inTestData != NULL){
					delete inTestData;
					inTestData = NULL;
				}else {
					throw std::runtime_error("Unable to delete test data.  Test data does not exist." );
				}
			}

			

			inline int returnTestLabel(int observationNumber){
				return inTestData->returnClassOfObservation(observationNumber);
			}
			inline int returnTestFeatureVal(const int featureNumber, const int observationNumber){
				return inTestData->returnFeatureValue(featureNumber, observationNumber);
			}
	}; // class fpData
} //namespace fp
#endif //fpData.h
