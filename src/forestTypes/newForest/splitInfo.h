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
				double impurity;

				F featureNum;
				T splitValue;

			public:
				bestSplitInfo(): impurity(std::numeric_limits<double>::max()){}

				inline void setSplitValue(T sVal){
					splitValue = sVal;
				}

				inline T returnSplitValue(){
					return splitValue;
				}

				inline void setImpurity(double gVal){
					impurity = gVal;
				}

				inline double returnImpurity(){
					return impurity;
				}


				inline void setFeature(F fNum){
					featureNum = fNum;
				}

				inline F returnFeatureNum(){
					return featureNum;
				}

				
		};

}//namespace fp
#endif //bestSplitInfo_h
