#ifndef nodeIterators_h
#define nodeIterators_h

#include <vector>
#include "obsIndexAndClassVec.h"

namespace fp{

	class nodeIterators
	{
		protected:
			std::vector<std::vector<int>::iterator > iteratorHolderBegin;
			std::vector<std::vector<int>::iterator > iteratorHolderEnd;

		public:
			nodeIterators(int numClasses){
				iteratorHolderBegin.resize(numClasses);
				iteratorHolderEnd.resize(numClasses);
			}

			inline void insertBeginIterator(const std::vector<int>::iterator& beginIter, int iterClass){
				iteratorHolderBegin[iterClass] = beginIter;
			}

			inline const std::vector<int>::iterator& returnBeginIterator(int iterClass){
				return iteratorHolderBegin[iterClass];
			}

			inline void insertEndIterator(const std::vector<int>::iterator& endIter, int iterClass){
				iteratorHolderEnd[iterClass] = endIter;
			}

			inline const std::vector<int>::iterator& returnEndIterator(int iterClass){
				return iteratorHolderEnd[iterClass];
			}

			inline void setInitialIterators(obsIndexAndClassVec& indexHolder){
				for(int i = 0; i < (int)iteratorHolderBegin.size(); ++i){
					iteratorHolderBegin[i] = indexHolder.returnClassVector(i).begin();
					iteratorHolderEnd[i] = indexHolder.returnClassVector(i).end();
				}
			}

			inline void setVecOfClassSizes(std::vector<int>& classSizes){
				for(int i = 0; i < (int)iteratorHolderEnd.size(); ++i){
					classSizes.push_back(iteratorHolderEnd[i]-iteratorHolderBegin[i]);
				}
			}

	};

}//namespace fp
#endif //nodeIterators_h
