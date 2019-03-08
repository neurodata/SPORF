#ifndef inputData_h
#define inputData_h

#include <iostream>
#include <iomanip>


namespace fp {

	template <typename T, typename Q>
		class inputData
		{
			private:
				bool isTrainingData = true;

			public:

				virtual ~inputData(){};
				virtual Q returnClassOfObservation(const int &observationNum){
					if(!isTrainingData){
						std::cout << "This is not training data and so does not contain class labels.\n";
					}else{
						std::cout << "Not implemented but should be.\n";
					}
					return -1;
				}
				virtual T returnFeatureValue(const int &featureNum, const int &observationNum) = 0;
				virtual void prefetchFeatureValue(const int &featureNum, const int &observationNum) = 0;
				virtual int returnNumFeatures() = 0;
				virtual int returnNumObservations() = 0;
				virtual int returnNumClasses(){
					if(!isTrainingData){
						std::cout << "This is not training data and so does not contain class labels.\n";
					}else{
						std::cout << "Not implemented but should be.\n";
					}
					return -1;
				}
				virtual void checkY(){
					if(!isTrainingData){
						std::cout << "This is not training data and so does not contain class labels.\n";
					}else{
						std::cout << "Not implemented but should be.\n";
					}
				}

				void printDataStats(){
					std::cout << "there are " << this->returnNumFeatures() << " features.\n";
					std::cout << "there are " << this->returnNumObservations() << " observations.\n";
				}

				void printXValues(){
					std::cout << std::fixed;
						for(int j = 0; j < this->returnNumObservations(); j++){
					for(int i = 0; i < this->returnNumFeatures(); i++){
							std::cout << std::setprecision(10) << this->returnFeatureValue(i,j) << " ";
						}
						std::cout << "\n";
					}
				}

				void printYValues(){
					if(isTrainingData){
						for(int j = 0; j < this->returnNumObservations(); j++){
							std::cout << this->returnClassOfObservation(j) << " ";
						}
						std::cout << "\n";
					}else{
						std::cout << "This is not training data and so does not contain class labels.\n";
					}
				}
		};

} //namespace fp
#endif //inputData_h
