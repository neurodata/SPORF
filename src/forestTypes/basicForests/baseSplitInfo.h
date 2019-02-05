#ifndef baseSplitInfo_h
#define baseSplitInfo_h

#include "../../baseFunctions/timeLogger.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

namespace fp{

	template<typename T, typename F>
		class baseSplitInfo
		{
			protected:
				double impurity;
				double leftImpurity;
				double rightImpurity;
				F featureNum;
				T splitValue;

			public:
				baseSplitInfo(): impurity(std::numeric_limits<double>::max()), leftImpurity(-1), rightImpurity(-1),  splitValue(0){}

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

				inline void setLeftImpurity(double gVal){
					leftImpurity = gVal;
				}
				inline double returnLeftImpurity(){
					return leftImpurity;
				}

				inline void setRightImpurity(double gVal){
					rightImpurity = gVal;
				}

				inline double returnRightImpurity(){
					return rightImpurity;
				}

				inline bool perfectSplitFound(){
					return impurity == 0;
				}

		};

}//namespace fp
#endif //baseSplitInfo_h
