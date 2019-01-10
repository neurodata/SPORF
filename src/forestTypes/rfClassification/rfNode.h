#ifndef padNode_h
#define padNode_h

#include "../../baseFunctions/fpBaseNode.h"
#include <stdio.h>

template <typename T>
class alignas(32) rfNode : public fpBaseNode<T, int>
{
	protected:
		//int feature;

	public:
		rfNode(){}

		/*
		inline void setFeatureValue(int fVal){
			feature = fVal;
		}

		inline int returnFeatureNumber(){
			return feature;
		}

		*/

		
		/*
    inline int nextNode(T featureValue){
			return (featureValue < fpBaseNode<T,int>::cutValue) ? fpBaseNode<T,int>::left : fpBaseNode<T,int>::right;
		}
		*/

    inline int nextNode(std::vector<T>& observation){
			return (observation[fpBaseNode<T, int>::feature] < fpBaseNode<T, int>::cutValue) ? fpBaseNode<T, int>::left : fpBaseNode<T, int>::right;
		}

};
#endif //padNode_h
