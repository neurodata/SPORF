#ifndef obsIndexAndClassVec_h
#define obsIndexAndClassVec_h

#include <vector>
#include <algorithm>

namespace fp{

	class obsIndexAndClassVec
	{
		protected:
			std::vector<std::vector<int> > indexHolder;

		public:
			obsIndexAndClassVec(int numClasses):indexHolder(numClasses){}

			inline void insertIndex(int index, int classOfObs){
				indexHolder[classOfObs].push_back(index);
			}

			inline std::vector<int>& returnClassVector(int classOfObs){
				return indexHolder[classOfObs];
			}

			inline void sortVectors(){
				for(auto& i : indexHolder){
					std::sort(i.begin(), i.end());
				}
			}

			inline void resetVectors(){
				for(auto& i : indexHolder){
					i.clear();
				}
			}


			//needs test in googletest
			inline int returnNumClasses(){
				return (int)indexHolder.size();
			}

	};

}//namespace fp
#endif //obsIndexAndClassVec_h
