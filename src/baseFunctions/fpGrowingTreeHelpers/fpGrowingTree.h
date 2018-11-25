#ifndef fpGrowingTree_h
#define fpGrowingTree_h
#include "rfNode.h"
#include <vector>
#include <random>
#include "growingTreeElement.h"

namespace fp{

	template <typename T>
		class fpGrowingTree
		{
			protected:
std::vector <growingTreeElement<T> >  nodeIndexHolder; 
std::vector <int> OOBHolder;

			public:
				fpGrowingTree(){
			nodeIndexHolder.resize(fpSingleton::getSingleton().returnNumObservations());	
			OOBHolder.resize(fpSingleton::getSingleton().returnNumObservations()/2);	
				}

				inline initializeRootOfTree(int growingTreeNumber){


				}
		};

}
#endif //fpGrowingTree_h
