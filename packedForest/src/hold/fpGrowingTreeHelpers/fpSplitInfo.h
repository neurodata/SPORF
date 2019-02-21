#ifndef fpSplitInfo_h
#define fpSplitInfo_h


namespace fp{

	template<typename T>
		class fpSplitInfo
		{
			protected:
				double impurity;
				double leftImpurity;
				double rightImpurity;
				int featureNum;
				T splitValue;

			public:
				fpSplitInfo(): impurity(std::numeric_limits<double>::max()), leftImpurity(-1), rightImpurity(-1), featureNum(-1), splitValue(0){}

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

				inline void setFeatureNum(int fNum){
					featureNum = fNum;
				}

				inline int returnFeatureNum(){
					return featureNum;
				}
		};


}//namespace fp
#endif //fpSplitInfo_h
