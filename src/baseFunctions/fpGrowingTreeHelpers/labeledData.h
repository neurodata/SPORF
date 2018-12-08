#ifndef labeledData_h
#define labeledData_h


namespace fp{

	template<typename T>
		class labeledData
		{
			protected:
				T dataElement;
				int dataLabel;

			public:

				inline bool operator < (const labeledData<T>& otherData) const
				{
					return dataElement < otherData.dataElement;
				}

				inline int returnDataLabel(){
					return dataLabel;
				}

				inline T returnDataElement(){
					return dataElement;
				}

				inline T midVal (const labeledData<T>& otherData) const
				{
					return (dataElement + otherData.dataElement)/2.0;
				}

				inline bool checkInequality(const labeledData<T>& otherData){
					return dataElement != otherData.dataElement;
				}

				void setPair(T dElement, int dLab){
					dataElement = dElement;
					dataLabel = dLab;
				}
		};

}//namespace fp
#endif //labeledData_h
