#ifndef zipperIterators_h
#define zipperIterators_h

#include <vector>
#include "zipClassAndValue.h"

namespace fp{

	template<typename T, typename Q>
		class zipperIterators
		{
			protected:
				typename std::vector<zipClassAndValue<T,Q> >::iterator zipBegin;
				typename std::vector<zipClassAndValue<T,Q> >::iterator zipEnd;

			public:
				zipperIterators(){ }

				inline void setZipBegin(typename std::vector<zipClassAndValue<T,Q> >::iterator beginIter){
					zipBegin = beginIter;
				}

				inline void setZipEnd(typename std::vector<zipClassAndValue<T,Q> >::iterator endIter){
					zipEnd = endIter;
				}

				inline typename std::vector<zipClassAndValue<T,Q> >::iterator returnZipBegin(){
					return zipBegin;
				}

				inline typename std::vector<zipClassAndValue<T,Q> >::iterator returnZipEnd(){
					return zipEnd;
				}

				inline void setZipIteratorsRoot(typename std::vector<zipClassAndValue<T,Q> >& zipper){
					zipBegin = zipper.begin();
					zipEnd = zipper.end();
				}

				inline void setZipIterators(zipperIterators<int,Q>& zipperIters, int numObjects, bool isLeftNode){
					if(isLeftNode){
						zipBegin = zipperIters.zipBegin;
						zipEnd = zipBegin+numObjects;
					}else{
						zipEnd = zipperIters.zipEnd;
						zipBegin = zipEnd-numObjects;
					}
				}

		};

}//namespace fp
#endif //zipperIterators_h
