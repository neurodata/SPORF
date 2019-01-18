#ifndef labeledData_h
#define labeledData_h

#include <assert.h>
#include <iomanip>

namespace fp{

	template<typename T>
		class labeledData
		{
			protected:
				T dataElement;
				int dataLabel;

			public:

				inline bool operator < (const labeledData<T>& otherData)
				{
					return dataElement < otherData.dataElement;
				}

				inline int returnDataLabel(){
					return dataLabel;
				}

				inline T returnDataElement(){
					return dataElement;
				}

				inline T midVal (const labeledData<T>& otherData)
				{
					assert(dataElement != otherData.dataElement);
					assert(dataElement != (dataElement+otherData.dataElement)/2.0);
					assert(otherData.dataElement != (dataElement/otherData.dataElement)/2.0);
					return (dataElement + otherData.dataElement)/2.0;
				}

				inline bool checkInequality(const labeledData<T>& otherData){
					if( dataElement != otherData.dataElement ){
						if( dataElement == (dataElement+otherData.dataElement)/2.0 ){
							std::cout << std::setprecision(10) << dataElement << ", " << otherData.dataElement << "\n";
						}

					}
					return dataElement != otherData.dataElement;
				}

				inline void setPair(const T dElement, const int dLab){
					dataElement = dElement;
					dataLabel = dLab;
				}
		};

}//namespace fp
#endif //labeledData_h
