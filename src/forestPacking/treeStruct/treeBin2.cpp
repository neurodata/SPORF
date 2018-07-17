#include "treeBin2.h"
#include <Rcpp.h>


void treeBin2::repackTree(std::vector<padNodeStat> tree, int workingNode){
//void treeBin2::repackTree(padNodeStat* tree, int workingNode){

	int nodeNumberInTree = bin[workingNode].returnRightNode();

	if(tree[nodeNumberInTree].isInternalNode()){

		int leftFreq = tree[tree[nodeNumberInTree].returnLeftNode()].returnFreq();
		int rightFreq = tree[tree[nodeNumberInTree].returnRightNode()].returnFreq();
		int leftNodeNum;
		int rightNodeNum;

		if(rightFreq > leftFreq){
			if(tree[tree[nodeNumberInTree].returnRightNode()].isInternalNode()){
				rightNodeNum = ++firstFreeNode;

				bin[rightNodeNum].setNode(tree[tree[nodeNumberInTree].returnRightNode()].returnCutValue(),
						tree[tree[nodeNumberInTree].returnRightNode()].returnFeature(),
						0,
						tree[nodeNumberInTree].returnRightNode());

				repackTree(tree, rightNodeNum);
			}else{
				rightNodeNum = tree[tree[nodeNumberInTree].returnRightNode()].returnClass() + numOfNodes;
			}

			if(tree[tree[nodeNumberInTree].returnLeftNode()].isInternalNode()){
				leftNodeNum = ++firstFreeNode;
				bin[leftNodeNum].setNode(tree[tree[nodeNumberInTree].returnLeftNode()].returnCutValue(),
						tree[tree[nodeNumberInTree].returnLeftNode()].returnFeature(),
						0,
						tree[nodeNumberInTree].returnLeftNode());

				repackTree(tree, leftNodeNum);
			}else{
				leftNodeNum = tree[tree[nodeNumberInTree].returnLeftNode()].returnClass() + numOfNodes;
			}
		}else{

			if(tree[tree[nodeNumberInTree].returnLeftNode()].isInternalNode()){
				leftNodeNum = ++firstFreeNode;
				bin[leftNodeNum].setNode(tree[tree[nodeNumberInTree].returnLeftNode()].returnCutValue(),
						tree[tree[nodeNumberInTree].returnLeftNode()].returnFeature(),
						0,
						tree[nodeNumberInTree].returnLeftNode());

				repackTree(tree, leftNodeNum);
			}else{
				leftNodeNum = tree[tree[nodeNumberInTree].returnLeftNode()].returnClass() + numOfNodes;
			}


			if(tree[tree[nodeNumberInTree].returnRightNode()].isInternalNode()){
				rightNodeNum = ++firstFreeNode;
				bin[rightNodeNum].setNode(tree[tree[nodeNumberInTree].returnRightNode()].returnCutValue(),
						tree[tree[nodeNumberInTree].returnRightNode()].returnFeature(),
						0,
						tree[nodeNumberInTree].returnRightNode());

				repackTree(tree, rightNodeNum);
			}else{
				rightNodeNum = tree[tree[nodeNumberInTree].returnRightNode()].returnClass() + numOfNodes;
			}
		}

		bin[workingNode].setLeftValue(leftNodeNum);
		bin[workingNode].setRightValue(rightNodeNum);

	}else{
		Rprintf("There should not be leaf nodes here subfunction. (TreeBin2.cpp)\n");
	}
}

treeBin2::treeBin2(std::ifstream &in){

	int tempNumOfTreesInBin;
	int tempDepth;
	int tempNumOfClasses;
	int tempNumOfNodes;
	in.read((char*)&tempNumOfTreesInBin, sizeof(int));
	numOfTreesInBin = tempNumOfTreesInBin;
	in.read((char*)&tempDepth, sizeof(int));
	depth = tempDepth;
	in.read((char*)&tempNumOfClasses, sizeof(int));
	numOfClasses = tempNumOfClasses;
	in.read((char*)&tempNumOfNodes, sizeof(int));
	numOfNodes = tempNumOfNodes;

	//needs to go
	uint32_t left;
	uint32_t feature;
	double cutValue;
	uint32_t right;

	bin.resize(numOfNodes+numOfClasses);
	for(int i = 0; i < (numOfNodes+numOfClasses); i++){
		in.read((char*)&left, sizeof(uint32_t));
		in.read((char*)&feature, sizeof(uint32_t));
		in.read((char*)&cutValue, sizeof(double));
		in.read((char*)&right, sizeof(uint32_t));
		bin[i].setNode(cutValue,feature, left, right);
	}
	
}



treeBin2::treeBin2(std::vector<std::vector<padNodeStat> > forest, std::vector<int>& treeLength, int startTree, int finalTree, int headDepth, int numClasses){
	numOfTreesInBin = finalTree-startTree;
	depth = headDepth;
	int binSize = 0;
	numOfClasses = numClasses;

	for(int i = startTree; i < finalTree; i++){
		binSize += treeLength[i];
	}
	numOfNodes = (binSize-numOfTreesInBin)/2;
	bin.resize(numOfNodes+numOfClasses);

	for(int j = 0; j < numOfClasses; j++){
		bin[j+numOfNodes].setClass(j);
	}

	for(int j = startTree; j < finalTree; j++){
		bin[++firstFreeNode] = forest[j][0];
		//left value is the originating tree
		bin[firstFreeNode].setLeftValue(j);
		//right value is position in originating tree
		bin[firstFreeNode].setRightValue(0);
	}

	int workingTree;
	int workingPosition;
	int workingClass;
	int lastNodeInLevel;
	for(int j = 0; j < headDepth; j++){
		lastNodeInLevel = firstFreeNode;
		while(currProcess < lastNodeInLevel){
			++currProcess;
			workingTree =bin[currProcess].returnLeftNode();
			workingPosition =bin[currProcess].returnRightNode();

			if(forest[workingTree][workingPosition].isInternalNode()){
				//Check if left node is leaf
				if(forest[workingTree][forest[workingTree][workingPosition].returnLeftNode()].isInternalNode()){
					bin[currProcess].setLeftValue(++firstFreeNode);
					bin[firstFreeNode] = forest[workingTree][forest[workingTree][workingPosition].returnLeftNode()];
					bin[firstFreeNode].setLeftValue(workingTree);
					bin[firstFreeNode].setRightValue(forest[workingTree][workingPosition].returnLeftNode());
				}else{
					workingClass = forest[workingTree][forest[workingTree][workingPosition].returnLeftNode()].returnClass();
					bin[currProcess].setLeftValue(workingClass+numOfNodes);
				}

				if(forest[workingTree][forest[workingTree][workingPosition].returnRightNode()].isInternalNode()){
					bin[currProcess].setRightValue(++firstFreeNode);
					bin[firstFreeNode] = forest[workingTree][forest[workingTree][workingPosition].returnRightNode()];
					bin[firstFreeNode].setLeftValue(workingTree);
					bin[firstFreeNode].setRightValue(forest[workingTree][workingPosition].returnRightNode());
				}else{
					workingClass = forest[workingTree][forest[workingTree][workingPosition].returnRightNode()].returnClass();
					bin[currProcess].setRightValue(workingClass+numOfNodes);
				}
			}else{
				Rprintf("There should not be leaf nodes here. (TreeBin2.cpp)\n");

			}
		}
	}

	int finalTopLevelNode = firstFreeNode; 
	int currTreeNum;
	for(int j = ++currProcess ;j <= finalTopLevelNode  ;j++){
		currTreeNum = bin[j].returnLeftNode();
		repackTree(forest[currTreeNum], j);
	}

	if( firstFreeNode != numOfNodes-1){
		Rprintf("Not all nodes were filled: Num of Nodes- %d, Num used- %d\n",numOfNodes, firstFreeNode);
	}
}


treeBin2::~treeBin2(){
	;
}


bool treeBin2::isInternalNode(int currentNodeNum){
	return numOfNodes > currentNodeNum;
}
