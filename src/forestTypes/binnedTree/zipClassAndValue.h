#ifndef zipClassAndValue_h
#define zipClassAndValue_h

#include <assert.h>
#include <cmath>

#define FLOAT_EPSILON  .0000001
#define DOUBLE_EPSILON .0000001

namespace fp{

	template<typename T, typename Q>
		class alignas(16) zipClassAndValue
		{
			protected:
				Q featureVal;
				T classOfObs;

			public:

				inline bool operator < (const zipClassAndValue<T,Q>& otherData) const
				{
					return featureVal < otherData.featureVal;
				}

				inline T returnObsClass(){
					return classOfObs;
				}

				inline Q returnFeatureVal(){
					return featureVal;
				}

				inline Q midVal (const zipClassAndValue<T,Q>& otherData) const
				{
					assert(featureVal != otherData.featureVal);
					assert(featureVal != (featureVal +otherData.featureVal )/2.0);
					assert(otherData.featureVal != (featureVal+featureVal)/2.0);
					return (featureVal + otherData.featureVal)/2.0;
				}

				inline bool checkInequality(const zipClassAndValue<int,double>& otherData){
					return std::abs(featureVal - otherData.featureVal) > DOUBLE_EPSILON;
				}

				inline bool checkInequality(const zipClassAndValue<int,float>& otherData){
					return std::abs(featureVal - otherData.featureVal) > FLOAT_EPSILON;
				}

				inline bool checkInequality(const zipClassAndValue<int,int>& otherData){
					return featureVal != otherData.featureVal;
				}

				void setPair(T obsClass, Q val){
					classOfObs = obsClass;
					featureVal = val;
				}
		};

}//namespace fp
#endif //zipClassAndValue_h
