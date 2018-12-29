#ifndef zipClassAndValue_h
#define zipClassAndValue_h


namespace fp{

	template<typename T, typename Q>
		class zipClassAndValue
		{
			protected:
				T classOfObs;
				Q featureVal;

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
					return (featureVal + otherData.featureVal)/2.0;
				}


				inline bool checkInequality(const zipClassAndValue<T,Q>& otherData){
					return featureVal != otherData.featureVal;
				}

				void setPair(T obsClass, Q val){
					classOfObs = obsClass;
					featureVal = val;
				}
		};

}//namespace fp
#endif //zipClassAndValue_h
