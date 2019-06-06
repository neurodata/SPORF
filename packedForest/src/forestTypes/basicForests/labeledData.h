#ifndef labeledData_h
#define labeledData_h

#include <assert.h>
#include <iomanip>
#include <cmath>
#include "../../fpSingleton/fpSingleton.h"


namespace fp{

	template<typename T>
		class alignas(16)labeledData
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

				inline T midVal (const labeledData<T>& otherData)
				{
					assert(dataElement != otherData.dataElement);
					assert(dataElement != (dataElement+otherData.dataElement)/2.0);
					assert(otherData.dataElement != (dataElement+otherData.dataElement)/2.0);
					return (dataElement + otherData.dataElement)/2.0;
				}

				inline bool checkInequality(const labeledData<double>& otherData){
					return std::abs(dataElement - otherData.dataElement) > std::numeric_limits<double>::epsilon() * 4 * std::abs(dataElement + otherData.dataElement);
				}

				inline bool checkInequality(const labeledData<float>& otherData){
					return std::abs(dataElement - otherData.dataElement) > std::numeric_limits<float>::epsilon() * 4 * std::abs(dataElement + otherData.dataElement);
				}

				inline bool checkInequality(const labeledData<int>& otherData){
					return dataElement != otherData.dataElement;
				}

				inline void setPair(const T dElement, const int dLab){
					dataElement = dElement;
					dataLabel = dLab;
				}
		};

}//namespace fp
#endif //labeledData_h
