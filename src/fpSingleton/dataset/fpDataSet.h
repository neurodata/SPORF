#ifndef fpDataSet_h
#define fpDataSet_h
#include <vector>
#include <iostream>
#include "fpReadCSV.h"
#include "fpDataBase.h"
#include "inputData.h"
#include <string>


namespace fp {

template <typename T, typename Q>
class inputCSVData : public inputData<T,Q>
{
	private:
		inputXData<T> X;
		inputYDataClassification<Q> Y;

	public:
		inputCSVData(const std::string& forestCSVFileName, const int &columnWithY)
		{
			csvHandle csvH(forestCSVFileName);
			if(columnWithY >= csvH.returnNumColumns()){
				throw std::runtime_error("column with class labels does not exist." );
				return;
			}

			X.initializeXData(csvH.returnNumColumns()-1, csvH.returnNumRows());
			Y.initializeYData(csvH.returnNumRows());

			for(int i=0; i<csvH.returnNumRows(); i++){
				for(int j=0; j<csvH.returnNumColumns(); j++){
					if(j < columnWithY){
						X.setXElement(j,i, csvH.returnNextElement<T>());
					}else if(j == columnWithY){
						Y.setYElement(i, csvH.returnNextElement<Q>());
					}else{
						X.setXElement(j-1,i, csvH.returnNextElement<T>());
					}
				}
			}

		}

		~inputCSVData(){
//TODO destroy X and Y
		}

		inline Q returnClassOfObservation(const int &observationNum){
			return Y.returnYElement(observationNum);
		}

		inline T returnFeatureValue(const int &featureNum, const int &observationNum){
			return X.returnElement(featureNum, observationNum);
		}

		inline void prefetchFeatureValue(const int &featureNum, const int &observationNum){
			X.prefetchElement(featureNum, observationNum);
		}

		inline int returnNumFeatures(){
			return X.returnNumFeatures();
		}

		inline int returnNumObservations(){
			return Y.returnNumObservations();
		}

		inline int returnNumClasses(){
			return Y.numClasses();
		}

		inline void checkY(){
			return Y.checkClassRepresentation();
		}

		void printDataStats(){
			std::cout << "there are " << this.returnNumFeatures() << " features.\n";
			std::cout << "there are " << this.returnNumObservations() << " observations.\n";
		}

		void printXValues(){
			for(int i = 0; i < this->returnNumFeatures(); i++){
				for(int j = 0; j < this->returnNumObservations(); j++){
					std::cout << this->returnFeatureValue(i,j) << " ";
				}
				std::cout << "\n";
			}
		}

		void printYValues(){
			for(int j = 0; j < this->returnNumObservations(); j++){
				std::cout << this->returnClassOfObservation(j) << " ";
			}
			std::cout << "\n";
		}
};

} //namespace fp
#endif //fpDataSet_h
