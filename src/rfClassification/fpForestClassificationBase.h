#ifndef fpForestClassification_h
#define fpForestClassification_h
//#include "../baseFunctions/fpForestBase.h"
#include "../baseFunctions/fpUtils.h"
#include <vector>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include "rfTree.h"

namespace fp {

	template <typename T>
		class fpForestClassificationBase : public fpForestBase
	{
		protected:
			std::vector<rfTree<T> > trees;

		public:
			fpForestClassificationBase(){
				std::srand(unsigned(std::time(0)));
			}

			void printForestType(){
				std::cout << "This is a basic classification forest.\n";
			}

			void changeForestSize(){
				trees.resize(fpSingleton::getSingleton().returnNumTrees());
			}

			void growTrees(){
				for(auto &tree : trees){
					tree.growTree();
					std::cout << "OOB: " <<	tree.returnOOB() << "\n";
				}
			}

			void growForest(){
				changeForestSize();
				growTrees();
			}
	};

}// namespace fp
#endif //fpForestClassification_h
