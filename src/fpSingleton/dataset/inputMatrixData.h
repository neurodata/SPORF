#ifndef inputMatrixData_h
#define inputMatrixData_h
#include <vector>
#include <iostream>
#include "fpReadCSV.h"
#include "fpDataBase.h"
#include "inputData.h"
#include <string>


namespace fp {

template <typename T, typename Q>
class inputMatrixData : public inputData<T,Q>
{
	private:
const std::vector<std::vector<T> >* inputXData;
const std::vector<Q>* inputYData;
	public:
		inputMatrixData( std::vector<std::vector<T> > Xmat, std::vector<Q> Yvec){
	inputXData = Xmat;
	inputYData = Yvec;
		}

		~inputMatrixData(){
//TODO destroy X and Y
		}

		inline Q returnClassOfObservation(const int &observationNum){
			return inputYData[observationNum];
		}

		inline T returnFeatureValue(const int &featureNum, const int &observationNum){
			return inputXData[featureNum, observationNum];
		}

		inline void prefetchFeatureValue(const int &featureNum, const int &observationNum){
			inputXData[featureNum][observationNum];
		}

		inline int returnNumFeatures(){
			return inputXData.size();
		}

		inline int returnNumObservations(){
			return inputYData.size();
		}

		inline int returnNumClasses(){
			return 2;
		}

		inline void checkY(){
			;
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
#endif //inputMatrixData_h
