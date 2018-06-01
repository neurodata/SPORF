#ifndef fpForestClassification_h
#define fpForestClassification_h
#include "../baseFunctions/fpForestBase.h"
#include <vector>
#include <stdio.h>
#include "rfTree.h"


template <typename T>
class fpForestClassificationBase : public fpForestBase
//class fpForestClassificationBase
{
	protected:
		std::vector<rfTree<T> > trees;
//		std::vector<T > trees;
				
	public:
		void printForestType(){
			std::cout << "This is a basic classification forest.\n";
		}

		
		void growForest(){
		//	inputData<double,int> data(forestCSVFileName, columnWithY);
		//	data.printXValues();
		//	data.printYValues();
		//	setDataRelatedParameters(data);
		}
};
#endif //fpForestClassification_h
