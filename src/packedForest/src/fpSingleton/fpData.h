#ifndef fpData_h
#define fpData_h

#include "fpDataSet.h"
#include "fpInfo.h"
#include <string>

using DATA_TYPE_X = double;
using DATA_TYPE_Y = double;

namespace fp {

	/**
	 * fpData holds both the training and test data.  This data gets loaded
	 * from fpDataset.
	 *
	 * TODO: This is not a good way to do this.  There is nothing different
	 * from training data and test data.  This is wrong.  test data should 
	 * be stored by observation whereas training data is stored by feature.
	 */

	class fpData{

		protected:
			inputData<DATA_TYPE_X, DATA_TYPE_Y>* inData;
			      testData<DATA_TYPE_X, DATA_TYPE_Y>* inTestData;

		public:

			fpData(){
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
					inData = new inputData<DATA_TYPE_X, DATA_TYPE_Y>(settings.returnCSVFileName(), settings.returnColumnWithY());
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

			inline void prefetchFeatureVal(const int featureNumber, const int observationNumber){
				inData->prefetchFeatureValue(featureNumber, observationNumber);
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
					inTestData = new testData<DATA_TYPE_X, DATA_TYPE_Y>(settings.returnCSVFileName(), settings.returnColumnWithY());
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
			inline double returnTestFeatureVal(const int featureNumber, const int observationNumber){
				return inTestData->returnFeatureValue(featureNumber, observationNumber);
			}
	}; // class fpData
} //namespace fp
#endif //fpData.h
