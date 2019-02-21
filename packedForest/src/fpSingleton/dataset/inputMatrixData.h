#ifndef inputMatrixData_h
#define inputMatrixData_h
#include <vector>
#include <iostream>
#include "inputData.h"
#include <string>


namespace fp {

	template <typename T, typename Q>
		class inputMatrixData : public inputData<T,Q>
	{
		protected:
			const T* inputXData;
			const Q* inputYData;
			int numClasses;
			int numObs;
			int numFeatures;
		public:
			inputMatrixData( const T* Xmat, const Q* Yvec, int numObs, int numFeatures):inputXData(Xmat),inputYData(Yvec),numClasses(-1),numObs(numObs),numFeatures(numFeatures){
				countAndCheckClasses();
			}

			~inputMatrixData(){
				//TODO destroy X and Y
			}

			inline Q returnClassOfObservation(const int &observationNum){
				return inputYData[observationNum];
			}

			inline int returnNumFeatures(){
				return numFeatures;
			}

			inline int returnNumObservations(){
				return numObs;
			}

			inline int returnNumClasses(){
				return numClasses;
			}


			inline void countAndCheckClasses(){
				std::vector<short> classRoll;
				for(int i = 0; i < numObs;++i){
					if(inputYData[i] >= numClasses){
						numClasses=inputYData[i]+1;
						classRoll.resize(numClasses,0);
					}
					classRoll[inputYData[i]]=1;
				}

				for(auto i : classRoll){
					if(i != 1){
						throw std::runtime_error("Not all classes represented in input." );
					}
				}
			}


			void printDataStats(){
				std::cout << "there are " << returnNumFeatures() << " features.\n";
				std::cout << "there are " << returnNumObservations() << " observations.\n";
			}

			void printXValues(){
				for(int i = 0; i < returnNumFeatures(); i++){
					for(int j = 0; j < returnNumObservations(); j++){
						std::cout << returnFeatureValue(i,j) << " ";
					}
					std::cout << "\n";
				}
			}

			T returnFeatureValue(const int &featureNum,
								 const int &observationNum)=0;

			void printYValues(){
				for(int j = 0; j < returnNumObservations(); j++){
					std::cout << returnClassOfObservation(j) << " ";
				}
				std::cout << "\n";
			}
	};

} //namespace fp
#endif //inputMatrixData_h
