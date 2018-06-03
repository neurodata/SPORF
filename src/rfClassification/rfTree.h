#ifndef rfTree_h
#define rfTree_h
#include "rfNode.h"
//#include <stdio.h>
#include <vector>

namespace fp{

	template <typename T>
	class nodeData{
		protected:
			std::vector<int>::iterator start;
			std::vector<int>::iterator end;

			std::vector<T> featureHolder;
			std::vector<int> labelHolder;
		public:
nodeData(std::vector<int>::iterator x, std::vector<int>::iterator y): start(x), end(y){}

 std::vector<int>::iterator returnNodeStart(){
return start;
 }

std::vector<int>::iterator returnNodeEnd(){
return end;
 }

void setLabelHolder(OOB* indices){

}

	};

template <typename T>
class rfTree
{
	protected:
		float OOBAccuracy;
		std::vector< rfNode<T> > tree;
		OOB* sampleIndices;

	public:
		rfTree() : OOBAccuracy(-1.0){}
		
		void setupOOB(fpData& dat){
sampleIndices = new OOB(dat.returnNumObservations());
		}
		
	void buildTree(fpInfo& info, fpData& dat){
		setupOOB(dat);


	}
		
	
};

}
#endif //rfTree_h
