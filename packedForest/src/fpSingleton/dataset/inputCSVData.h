#ifndef inputCSVData_h
#define inputCSVData_h
#include <vector>
#include <iostream>
#include "fpReadCSV.h"
#include "fpDataBase.h"
#include "inputData.h"
#include <string>

//#define HOLDSIZE 128

namespace fp {

	template <typename T, typename Q>
		class inputCSVData : public inputData<T,Q>
	{
		private:
			inputXData<T> X;
			inputYDataClassification<Q> Y;
			//std::vector<std::vector<T> > dataTempStore;

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

				/*
					 dataTempStore.resize(csvH.returnNumColumns()-1);
					 for(auto& i : dataTempStore){
					 i.resize(HOLDSIZE);
					 }
					 int pRow = 0;
					 for(; pRow < csvH.returnNumRows()-HOLDSIZE; pRow+=HOLDSIZE){
					 for(int i = 0; i<HOLDSIZE; i++){
					 for(int j=0; j<csvH.returnNumColumns(); j++){
					 if(j < columnWithY){
					 dataTempStore[j][i] = csvH.returnNextElement<T>();
					 }else if(j == columnWithY){
					 Y.setYElement(pRow+i, csvH.returnNextElement<Q>());
					 }else{
					 dataTempStore[j-1][i] = csvH.returnNextElement<T>();
					 }
					 }
					 }
					 for(int j=0; j<csvH.returnNumColumns()-1; j++){
					 for(int k = 0; k < HOLDSIZE; ++k){
					 X.setXElement(j,k+pRow, dataTempStore[j][k]);
					 }
					 }
					 }

					 for(int i = pRow; i<csvH.returnNumRows(); i++){
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
					 */

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
				std::cout << "there are " << this->returnNumFeatures() << " features.\n";
				std::cout << "there are " << this->returnNumObservations() << " observations.\n";
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
#endif //inputCSVData_h
