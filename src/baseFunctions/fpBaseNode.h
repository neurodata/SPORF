#ifndef fpBaseNode_h
#define fpBaseNode_h

#include <stdio.h>

template <typename T, typename F>
class fpBaseNode
{
	protected:
		int left;
		T cutValue;
		int right;
		int depth;
		F feature;

	public:
		fpBaseNode():left(0), right(0), depth(0){}

		inline bool isInternalNode(){
			return left;
		}

		inline int returnDepth(){
			return depth;
		}

		inline void setDepth(int dep){
			depth = dep;
		}

		inline T returnCutValue(){
			return cutValue;
		}
		inline void setCutValue(T cVal){
			cutValue = cVal;
		}

		inline int returnLeftNodeID(){
			return left;	
		}

		inline int returnRightNodeID(){
			return right;
		}

		inline int returnClass(){
			return right;	
		}

		inline void setClass(int classNum){
			right = classNum;
			left = 0;
		}

		inline void setLeftValue(int LVal){
			left = LVal;	
		}

		inline void setRightValue(int RVal){
			right = RVal;
		}

		inline bool goLeft(T featureValue){
			return featureValue < cutValue;
		}

		inline void setFeatureValue(F fVal){
feature = fVal;
		}

		inline F returnFeatureNumber(){
			return feature;
		}


inline int nextNode(T featureVal){
			return (featureVal < cutValue) ? left : right;
		}


		/*
inline int nextNode(std::vector<T>& observation, std::vector<int>& featureVector){
			T featureVal = 0;
			for(auto featureNumber : featureVector){
				featureVal += observation[featureNumber];
			}
			return (featureVal < fpBaseNode<T,std::vector<int> >::cutValue) ? fpBaseNode<T,std::vector<int> >::left : fpBaseNode<T,std::vector<int> >::right;
		}
		*/

		void virtual printNode(){
			if(isInternalNode()){
				std::cout << "internal ";
			}else{
				std::cout << "leaf ";
			}
			std::cout << "cutValue " << cutValue << ", left " << left << ", right " << right << ", depth " << depth << "\n";
		}

};
#endif //fpBaseNode_h
