#ifndef rerfNode_h
#define rerfNode_h

#include <stdio.h>
#include <vector>

template <typename T>
class rerfNode
{
	protected:
		int left;
		int right;
		int depth;
		T cutValue;
		std::vector<int> feature;

	public:
		rerfNode():left(0), right(0), depth(0){}
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

		inline void addFeatureValue(int fVal){
			feature.push_back(fVal);
		}

		inline void setFeatureValue(std::vector<int> fVal){
			feature = fVal;
		}

		inline std::vector<int>& returnFeatureNumber(){
			return feature;
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

		inline int nextNode(T featureValue){
			return (featureValue < cutValue) ? left : right;
		}

		void virtual printNode(){
			if(isInternalNode()){
				std::cout << "internal ";
			}else{
				std::cout << "leaf ";
			}
			std::cout << "cutValue " << cutValue << ", left " << left << ", right " << right << ", depth " << depth << "\n";
			//std::cout << "cutValue " << cutValue <<", feature " << feature << ", left " << left << ", right " << right << ", depth " << depth << "\n";
		}

};
#endif //padNode_h
