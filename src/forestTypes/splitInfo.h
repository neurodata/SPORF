#ifndef splitInfo_h
#define splitInfo_h

#include "../baseFunctions/timeLogger.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

namespace fp{

	template<typename T, typename F>
		class splitInfo
		{
			protected:
				double impurity;
				double leftImpurity;
				double rightImpurity;
				F feature;
				T splitValue;

			public:
				splitInfo(): impurity(std::numeric_limits<double>::max()), leftImpurity(-1), rightImpurity(-1),  splitValue(0){}

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

				inline void setLeftImpurity(float gVal){
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

				inline void setFeature(F& fNum){
					feature = fNum;
				}

				inline F& returnFeature(){
					return &feature;
				}

		};

}//namespace fp
#endif //splitInfo_h
