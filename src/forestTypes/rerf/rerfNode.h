#ifndef rerfNode_h
#define rerfNode_h

#include "../../baseFunctions/fpBaseNode.h"
#include <stdio.h>
#include <vector>

template <typename T>
class rerfNode : public fpBaseNode<T, std::vector<int> >
{
	public:
		rerfNode(){}

		inline void addFeatureValue(int fVal){
			fpBaseNode<T,std::vector<int> >::feature.push_back(fVal);
		}

		/*
inline int nextNode(T featureVal){
			return (featureVal < fpBaseNode<T,std::vector<int> >::cutValue) ? fpBaseNode<T,std::vector<int> >::left : fpBaseNode<T,std::vector<int> >::right;
		}
*/

		inline int nextNode(std::vector<T>& observation){
			T featureVal = 0;
			for(auto featureNumber : fpBaseNode<T,std::vector<int> >::feature){
				featureVal += observation[featureNumber];
			}
			return (featureVal < fpBaseNode<T,std::vector<int> >::cutValue) ? fpBaseNode<T,std::vector<int> >::left : fpBaseNode<T,std::vector<int> >::right;
		}
    
};
#endif //padNode_h
