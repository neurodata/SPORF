#ifndef fpForestFactory_h
#define fpForestFactory_h

#include <string>
#include <memory>
#include "fpForestBase.h"
#include "../rfClassification/fpForestClassificationBase.h"

namespace fp{

	class forestFactory
	{
		public:
			static std::unique_ptr<fpForestBase> setForestType(const std::string& parameterName){
			//	std::uniqe_ptr<fpForestBase> pToForest;
				if(parameterName == "rfBase"){
					return std::unique_ptr<fpForestBase>{new fpForestClassificationBase<float>};
					//pToForest = std::unique_ptr<fpForestBase>{new fpForestClassificationBase<float>};
				}else{
					throw std::runtime_error("Unimplemented forest type chosen." );
					return NULL;
				}
		//		return pToForest;
			}
	};
}//namespace fp
#endif //fpForestFactory_h
