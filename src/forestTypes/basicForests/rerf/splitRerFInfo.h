#ifndef splitRerFInfo_h
#define splitRerFInfo_h

#include "../baseSplitInfo.h"
#include <vector>

namespace fp{

	template<typename T>
		class splitRerFInfo :public baseSplitInfo<T, std::vector<int> >
	{
		protected:

		public:

			inline void addFeatureNums(const std::vector<int>& fNum){
				baseSplitInfo<T,std::vector<int> >::featureNum = fNum;
			}

			inline std::vector<int>& returnFeatureNum(){
				return baseSplitInfo<T,std::vector<int> >::featureNum;
			}
	};


}//namespace fp
#endif //splitRerFInfo_h
