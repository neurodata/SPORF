#ifndef fpGrowingTree_h
#define fpGrowingTree_h

#include <vector>
#include <iostream>
#include <random>
//#include "growingTreeElement.h"

namespace fp{

	template <typename T>
		class fpGrowingTree
		{
			protected:
				static std::vector <int> potentialSamples(fpSingleton::getSingleton().returnNumObservations());
//				std::vector <growingTreeElement<T> >  nodeIndexHolder; 
				std::vector <int> OOBHolder;

			public:
				fpGrowingTree(){
					std::vector<int> potentialSamples;


					nodeIndexHolder.resize(fpSingleton::getSingleton().returnNumObservations());	
					//TODO make the next line optional.  Only if user wants OOB.
					if(true){
						OOBHolder.resize(fpSingleton::getSingleton().returnNumObservations()/2);	
					}

				}

				inline initializeRootOfTree(int growingTreeNumber){


				}
		};

//	for(int i=0; i < fpSingleton::getSingleton().returnNumObservations(); ++i){
//		potentialSamples[i] = i;
//	}

}
#endif //fpGrowingTree_h
