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

			inline void setFunctionPointers(){
;//fpSingleton::getSingleton().setFunctionPointers();
			}

inline void initializeForestType(){
forest = forestFactory::setForestType(fpSingleton::getSingleton().returnForestType());
			}

			void growForest(){
				loadData();
				initializeForestType();
				forest->growForest();
				forest->printForestType();
			}
	}; // class fpForest
} //namespace fp
#endif //fpForest.h
