#ifndef bestSplitInfo_h
#define bestSplitInfo_h

//#include <iostream>
//#include <vector>
//#include <algorithm>
//#include <limits>

namespace fp{

	template<typename T, typename F>
		class bestSplitInfo
		{
			protected:
				float impurity;

				T splitValue;
				F featureNum;

			public:
				bestSplitInfo(): impurity(std::numeric_limits<float>::max()){}

				inline void setSplitValue(T sVal){
					splitValue = sVal;
				}

				inline T returnSplitValue(){
					return splitValue;
				}

				inline void setImpurity(float gVal){
					impurity = gVal;
				}

				inline double returnImpurity(){
					return impurity;
				}

				inline void setFeature(F fNum){
					featureNum = fNum;
				}

				inline F& returnFeatureNum(){
					return featureNum;
				}

				inline bool perfectSplitFound(){
					return impurity == 0;
				}
		};

}//namespace fp
#endif //bestSplitInfo_h
