#ifndef fpForestFactory_h
#define fpForestFactory_h

#include <string>
//#include <memory>
#include "fpForestBase.h"
#include "../forestTypes/rerf/fpRerFBase.h"
#include "../forestTypes/rfClassification/fpForestClassificationBase.h"

namespace fp{

	template <typename T>
	class forestFactory
	{
		public:
			static std::unique_ptr<fpForestBase<T> > setForestType(const std::string& parameterName){
				if(parameterName == "rfBase"){
					return std::unique_ptr<fpForestBase<T> >{new fpForestClassificationBase<T>};
				}else if(parameterName == "rerf"){
					return std::unique_ptr<fpForestBase<T> >{new fpRerFBase<T>};
				}else{
					throw std::runtime_error("Unimplemented forest type chosen." );
					return NULL;
				}
			}
	};
}//namespace fp
#endif //fpForestFactory_h
