#ifndef fpForest_h
#define fpForest_h

#include "fpUtils.h"
#include "../fpSingleton/fpSingleton.h"
#include <string>
#include <memory>

namespace fp {

	class fpForest{

		protected:
			std::unique_ptr<fpForestBase> forest;

		public:

			fpForest(){}


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

			void loadData(){
				fpSingleton::getSingleton().loadData();
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
				forest = forestFactory::setForestType(fpSingleton::getSingleton().returnForestType());
			}

			inline void setDataDependentParameters(){
				fpSingleton::getSingleton().setDataDependentParameters();
			}

			void growForest(){
				loadData();
				initializeForestType();
				setDataDependentParameters();
				forest->growForest();
				deleteData();
			}

			void loadTestData(){
				fpSingleton::getSingleton().loadTestData();
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
