#ifndef splitInfo_h
#define splitInfo_h

#include "../baseSplitInfo.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

namespace fp{

	template<typename T>
		class splitInfo : public baseSplitInfo<T, int>
	{
		protected:

		public:
			splitInfo(){
			baseSplitInfo<T,int>::featureNum = -1;
			}

			inline void setFeatureNum(int fNum){
				baseSplitInfo<T,int>::featureNum = fNum;
			}

			inline int returnFeatureNum(){
				return baseSplitInfo<T,int>::featureNum;
			}
	};

}//namespace fp
#endif //splitInfo_h
