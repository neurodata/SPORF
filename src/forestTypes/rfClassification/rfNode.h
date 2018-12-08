#ifndef padNode_h
#define padNode_h

#include "../../baseFunctions/fpBaseNode.h"
#include <stdio.h>

template <typename T>
class alignas(32) rfNode : public fpBaseNode<T>
{
	protected:
		int feature;

	public:
		rfNode(){}

		inline void setFeatureValue(int fVal){
			feature = fVal;
		}

		inline int returnFeatureNumber(){
			return feature;
		}
inline int nextNode(T featureValue){
			return (featureValue < fpBaseNode<T>::cutValue) ? fpBaseNode<T>::left : fpBaseNode<T>::right;
		}
};
#endif //padNode_h
