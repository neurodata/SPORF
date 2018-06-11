#ifndef fpForestClassification_h
#define fpForestClassification_h
//#include "../baseFunctions/fpForestBase.h"
#include "../baseFunctions/fpUtils.h"
#include <vector>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
//#include "rfTree.h"

namespace fp {

	template <typename T>
		class fpForestClassificationBase : public fpForestBase
	{
		protected:
//			std::vector<rfTree<T> > trees;

		public:
					fpForestClassificationBase(){
						std::srand(unsigned(std::time(0)));
					}

			void printForestType(){
				std::cout << "This is a basic classification forest.\n";
			}


			void changeForestSize(){
		;	//	trees.resize(numTrees);
			}

			void setDataRelatedParameters(fpInfo& info, fpData& dat){
				info.setNumClasses(dat.returnNumClasses());
				info.setNumFeatures(dat.returnNumFeatures());
				info.setMTRY();

			}

			void growForest(){
			//	setDataRelatedParameters(info, dat);
				changeForestSize();
/*
				for(rfTree<T> i : trees){
					i.buildTree(info, dat);
				}
				*/
			}

			//		void growForest(const fpForest& forestInfo){
			//			changeForestSize(forestInfo.returnNumTrees());

			//			}


			//		virtual void growForest(fpForest* fpFor){
			//changeForestSize(fpFor->returnNumTrees());
			//		}
	};

}// namespace fp
#endif //fpForestClassification_h
