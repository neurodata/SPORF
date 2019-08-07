#ifndef fpForestFactory_h
#define fpForestFactory_h

#include <string>
#include "fpForestBase.h"
#include "weightedFeature.h"
#include "../forestTypes/basicForests/rerf/fpRerFBase.h"
#include "../forestTypes/unsupervisedForests/urf/fpURFBase.h"
#include "../forestTypes/unsupervisedForests/urerf/fpURerFBase.h"
#include "../forestTypes/basicForests/rfClassification/fpForestClassificationBase.h"
#include "../forestTypes/binnedTree/binnedBase.h"

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
					}else if(parameterName == "urf"){
						return std::unique_ptr<fpForestBase<T> >{new fpURFBase<T>};
					}else if(parameterName == "urerf"){
						return std::unique_ptr<fpForestBase<T> >{new fpURerFBase<T>};
					}else if(parameterName == "binnedBase"){
						return std::unique_ptr<fpForestBase<T> >{new binnedBase<T,int>};
					}else if(parameterName == "binnedBaseRerF"){
						return std::unique_ptr<fpForestBase<T> >{new binnedBase<T,std::vector<int> >};
					}else if(parameterName == "binnedBaseTern"){
						return std::unique_ptr<fpForestBase<T> >{new binnedBase<T,weightedFeature >};
					}else{
						throw std::runtime_error("Unimplemented forest type chosen." );
						return NULL;
					}
				}
		};
}//namespace fp
#endif //fpForestFactory_h
