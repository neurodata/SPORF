#ifndef fpForest_h
#define fpForest_h

#include "../fpSingleton/fpSingleton.h"
#include <string>
#include <memory>

#if defined(ENABLE_OPENMP)
#include <omp.h>
#else
void	omp_set_dynamic(int x){
	std::cout << "I should not get called\n";
}
void omp_set_num_threads(int x){
	std::cout << "I should not get called\n";
}
#endif

namespace fp {

	template <typename T>
		class fpForest{

			protected:
				std::unique_ptr<fpForestBase<T> > forest;
				float OOBaccuracy = 0;

				void loadData(){
					fpSingleton::getSingleton().loadData();
				}

				void loadData(const T* Xmat, const int* Yvec, int numObs, int numFeatures){
					fpSingleton::getSingleton().loadData(Xmat,Yvec,numObs,numFeatures);
				}


				void loadTestData(){
					fpSingleton::getSingleton().loadTestData();
				}

				void deleteData(){
					fpSingleton::getSingleton().deleteData();
				}

				void deleteTestData(){
					fpSingleton::getSingleton().deleteTestData();
				}


				inline void setFunctionPointers(){
					;//fpSingleton::getSingleton().setFunctionPointers();
				}

				inline void initializeForestType(){
					forest = forestFactory<T>::setForestType(fpSingleton::getSingleton().returnForestType());
				}

				inline void setDataDependentParameters(){
					fpSingleton::getSingleton().setDataDependentParameters();
				}

				inline void checkDataDependentParameters(){
					fpSingleton::getSingleton().checkDataDependentParameters();
				}

			public:

				fpForest(){}
				~fpForest(){
				fpSingleton::getSingleton().resetSingleton();
				}

				inline void setParameter(const std::string& parameterName, const std::string& parameterValue){
					fpSingleton::getSingleton().setParameter(parameterName, parameterValue);	
				}


				inline void setParameter(const std::string& parameterName, const double parameterValue){
					fpSingleton::getSingleton().setParameter(parameterName, parameterValue);	
				}


				inline void setParameter(const std::string& parameterName, const int parameterValue){
					fpSingleton::getSingleton().setParameter(parameterName, parameterValue);	
				}


				inline void printParameters(){
					fpSingleton::getSingleton().printAllParameters();
				}


				inline void printForestType(){
					fpSingleton::getSingleton().printForestType();
				}


				inline void setNumberOfThreads(){
					omp_set_dynamic(0);     // Explicitly disable dynamic teams
					omp_set_num_threads(fpSingleton::getSingleton().returnNumThreads());
					std::cout << "\n" << fpSingleton::getSingleton().returnNumThreads() << " thread was set\n";
				}


				inline void growForest(const T* Xmat, const int* Yvec, int numObs, int numFeatures){
					loadData(Xmat,Yvec,numObs,numFeatures);
					setDataDependentParameters();
					checkDataDependentParameters();
					initializeForestType();
					forest->growForest();
					updateOOB();
					
					deleteData();
				}


				inline void growForest(){
					loadData();
					setDataDependentParameters();
					checkDataDependentParameters();
					initializeForestType();
					forest->growForest();
					updateOOB();

					deleteData();
				}


				inline int predict(std::vector<T>& observation){
					return forest->predictClass(observation);
				}

        inline std::vector<int> predictPost(std::vector<T>& observation){
					return forest->predictClassPost(observation);
				}

				inline int predict(const T* observation){
					return forest->predictClass(observation);
				}

				inline void updateOOB(){
					OOBaccuracy = forest->reportOOB();
				}

				inline float reportOOB(){
					return OOBaccuracy;
				}

                inline std::map<std::pair<int, int>, double> returnPairMat(){
                                    return forest->returnPairMat();
                            }
				float testAccuracy(){
					float testError;
					loadTestData();
					testError = forest->testForest();
					deleteTestData();
					return testError;
				}
		}; // class fpForest
} //namespace fp
#endif //fpForest.h
