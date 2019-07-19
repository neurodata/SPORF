#ifndef splitURerFInfo_h
#define splitURerFInfo_h

#include "../../basicForests/baseSplitInfo.h"
#include <vector>

namespace fp{

	template<typename T>
		class splitURerFInfo :public baseSplitInfo<T, std::vector<int> >
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
