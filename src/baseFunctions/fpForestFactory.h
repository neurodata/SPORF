#ifndef fpForestFactory_h
#define fpForestFactory_h

#include <string>
#include "fpForestBase.h"
#include "../rfClassification/fpForestClassificationBase.h"

class forestFactory
{
	public:
		static fpForestBase* setForestType(const int forestType){
			switch(forestType){
				case 0 :
					return new fpForestClassificationBase<float>;
					//return new fpForestClassificationBase;
					break;
				default :
					throw std::runtime_error("Unimplemented forest type chosen." ); return NULL; }
		}
};
#endif //fpForestFactory_h
