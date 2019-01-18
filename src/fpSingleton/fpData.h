#ifndef fpData_h
#define fpData_h

#include "dataset/fpDataSet.h"
#include "dataset/inputMatrixData.h"
#include "fpInfo.h"
#include <string>
#include <vector>

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

			inline void printXValues(){
inData->printXValues();
			}

			void fpLoadData(fpInfo& settings){
				if(settings.loadDataFromCSV()){
					inData = new inputCSVData<DATA_TYPE_X, DATA_TYPE_Y>(settings.returnCSVFileName(), settings.returnColumnWithY());
				}else if(false){
					inData = new inputCSVData<DATA_TYPE_X, DATA_TYPE_Y>(settings.returnCSVFileName(), settings.returnColumnWithY());
					//inData = new inputMatrixData<DATA_TYPE_X, DATA_TYPE_Y>();//TODO add parameters
					//inData = new inputMatrixData<DATA_TYPE_X, DATA_TYPE_Y>(std::vector<std::vector<DATA_TYPE_X> > X, std::vector<DATA_TYPE_Y> Y);
				}else{
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

			inline DATA_TYPE_X returnFeatureVal(const int featureNumber, const int observationNumber){
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
				if(settings.loadDataFromCSV()){
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
			inline DATA_TYPE_X returnTestFeatureVal(const int featureNumber, const int observationNumber){
				return inTestData->returnFeatureValue(featureNumber, observationNumber);
			}
	}; // class fpData
} //namespace fp
#endif //fpData.h
