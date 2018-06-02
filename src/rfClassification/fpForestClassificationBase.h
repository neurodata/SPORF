#ifndef fpForestClassification_h
#define fpForestClassification_h
//#include "../baseFunctions/fpForestBase.h"
#include "../baseFunctions/fpUtils.h"
#include <vector>
#include <stdio.h>
#include "rfTree.h"

namespace fp {

	template <typename T>
		class fpForestClassificationBase : public fpForestBase
	{
		protected:
			std::vector<rfTree<T> > trees;

		public:
			//		fpForestClassificationBase(){

			//		}
			void printForestType(){
				std::cout << "This is a basic classification forest.\n";
			}


			void changeForestSize(int numTrees){
				trees.resize(numTrees);
			}

			void setDataRelatedParameters(fpInfo& info, fpData& dat){
				info.setNumClasses(dat.returnNumClasses());
				info.setNumFeatures(dat.returnNumFeatures());
				info.setMTRY();

			}

			void growForest(fpInfo& info, fpData& dat){
				setDataRelatedParameters(info, dat);
				changeForestSize(info.returnNumTrees());

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
