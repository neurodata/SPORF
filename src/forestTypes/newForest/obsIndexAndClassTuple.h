#ifndef obsIndexAndClassTuple_h
#define obsIndexAndClassTuple_h


namespace fp{

	template<typename T>
		class obsIndexAndClassTuple
		{
			protected:
				int indexOfObs;
				T classOfObs;

			public:

				inline bool operator < (const obsIndexAndClassTuple<T>& otherData) const
				{
					return indexOfObs < otherData.indexOfObs;
				}

				inline int returnClassOfObs(){
					return classOfObs;
				}

				inline T returnIndexOfObs(){
					return indexOfObs;
				}

				void setObsClass(T obsClass){
					classOfObs = obsClass;
				}

				void setObsIndex(int index){
					indexOfObs = index;
				}
		};

}//namespace fp
#endif //obsIndexAndClassTuple_h
