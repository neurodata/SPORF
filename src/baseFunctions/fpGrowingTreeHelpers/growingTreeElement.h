#ifndef fpGrowingTreeElement_h
#define fpGrowingTreeElement_h


namespace fp{

	template<typename T>
		class growingTreeElement
		{
			protected:
				int dataIndex;
				T dataElement;

			public:

				inline bool operator < (const labeledData<T>& otherData) const
				{
					return dataElement < otherData.dataElement;
				}

				inline int returnDataIndex(){
					return dataIndex;
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

				inline void setIndex(int dLab){
					dataIndex = dLab;
				}

				inline void setDataElement(T dElement){
					dataElement = dElement;
				}
		};

}//namespace fp
#endif //fpGrowingTreeElement_h
