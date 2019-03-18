#ifndef splitURFInfo_h
#define splitURFInfo_h

#include "../../basicForests/baseSplitInfo.h"
#include <vector>

namespace fp{

	template<typename T>
		class splitURFInfo :public baseSplitInfo<T, int>
	{
		protected:

		public:
			splitURFInfo(){
				baseSplitInfo<T,int>::featureNum = -1;
			}
			inline void setFeatureNums(int fNum){
				baseSplitInfo<T,int>::featureNum = fNum;
			}

			inline int returnFeatureNum(){
				return baseSplitInfo<T,int>::featureNum;
			}
	};


}//namespace fp
#endif //splitRFInfo_h
