#ifndef fpForest_h
#define fpForest_h

#include "fpUtils.h"
#include <string>
#include <memory>

namespace fp {

	class fpForest{

		protected:
			// data input
			fpInfo fpForestInfo;
      std::unique_ptr<fpForestBase> forest;
			fpData data;

		public:

void setParameter(const std::string& parameterName, const std::string& parameterValue){
		if(parameterName == "forestType"){
		forest = forestFactory::setForestType(parameterValue);	
		}else{
	data.setParameter(parameterName, parameterValue);
		}
	}
			

			void setParameter(const std::string& parameterName, const double parameterValue){
				if(parameterName =="columnWithY"){
data.setParameter(parameterName, parameterValue);
				}else{
	fpForestInfo.setParameter(parameterName, parameterValue);
			}
			}

			void setParameter(const std::string& parameterName, const int parameterValue){
if(parameterName =="columnWithY"){
data.setParameter(parameterName, parameterValue);
				}else{
	fpForestInfo.setParameter(parameterName, parameterValue);
			}
			}

			void printParameters(){
fpForestInfo.printAllParameters();
data.printAllParameters();
			}

			void printType(){
forest->printForestType();
			}

			void loadData(){
data.fpLoadData();
			}

void growForest(){
data.fpLoadData();
forest->growForest(fpForestInfo,data);
			}
	}; // class fpForest
} //namespace fp
#endif //fpForest.h
