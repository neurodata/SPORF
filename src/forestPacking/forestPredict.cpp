#include <queue>
#include <Rcpp.h>
#include <iostream>
#include <vector>
#include <sstream>
#include "forestPredict.h"

namespace {

	inline int returnClassPrediction(std::vector<int>& classTotals, int numClasses){
		int predictMaxValue = -1;
		int predictMaxClass = -1;
		for(int m=0; m<numClasses; m++){
			if(classTotals[m] > predictMaxValue){
				predictMaxClass = m;
				predictMaxValue = classTotals[m];
			}
		}
		return predictMaxClass;
	}

	class repack 
	{
		public:
			int currentNode=-1;
			padNodeStat* tree;
			padNode* realTree;
			int repackTree(int workingNode);
			repack( padNodeStat* tTree, padNode* rTree): tree(tTree), realTree(rTree) {};
	};

	int repack::repackTree(int workingNode){

		int thisNodesLocation = ++currentNode;

		if(tree[workingNode].isInternalNode()){
			int leftFreq = tree[tree[workingNode].returnLeftNode()].returnFreq();
			int rightFreq = tree[tree[workingNode].returnRightNode()].returnFreq();
			int leftNodeNum;
			int rightNodeNum;
			if(rightFreq > leftFreq){
				rightNodeNum = repackTree(tree[workingNode].returnRightNode());
				leftNodeNum = repackTree(tree[workingNode].returnLeftNode());
			}else{
				leftNodeNum = repackTree(tree[workingNode].returnLeftNode());
				rightNodeNum = repackTree(tree[workingNode].returnRightNode());
			}

			realTree[thisNodesLocation].setNode(tree[workingNode].returnCutValue(),
					tree[workingNode].returnFeature(),
					leftNodeNum,
					rightNodeNum
					);
		}else{
			realTree[thisNodesLocation].setNode(tree[workingNode].returnCutValue(),
					tree[workingNode].returnFeature(),
					tree[workingNode].returnLeftNode(),
					tree[workingNode].returnRightNode());
		}
		return(thisNodesLocation);
	}
} //namespace

forestPredict::forestPredict(const std::string& forestFileName){
	std::ifstream infile (forestFileName, std::ofstream::binary);

	infile.read((char*)&numOfBins, sizeof(int));
	infile.read((char*)&numTreesInForest, sizeof(int));
	infile.read((char*)&totalNumberOfNodes, sizeof(int));
	infile.read((char*)&numOfClasses, sizeof(int));
	infile.read((char*)&debugModeOn, sizeof(bool));
	infile.read((char*)&showAllResults, sizeof(bool));

	forestRoots =  new treeBin2*[numOfBins]; 


	for(int i = 0 ; i < numOfBins; i++){
		forestRoots[i] = new treeBin2(infile);
	}

	int testEOF;
	if(infile >> testEOF){
		Rprintf("not end of file \n");
	}
	infile.close();
}



int forestPredict::makePrediction(const std::vector<double>& observation){

	std::vector<int> predictions(numOfClasses);
	std::vector<int> currentNode(forestRoots[0]->numOfTreesInBin);
	int numberNotInLeaf;
	int k, q;

	for( k=0; k < numOfBins;k++){

		for( q=0; q<forestRoots[k]->numOfTreesInBin; q++){
			currentNode[q] = q;
			__builtin_prefetch(&forestRoots[k]->bin[currentNode[q]], 0, 3);
		}

		do{
			numberNotInLeaf = forestRoots[k]->numOfTreesInBin;

			for( q=0; q<forestRoots[k]->numOfTreesInBin; q++){

				if(forestRoots[k]->bin[currentNode[q]].isInternalNode()){
					currentNode[q] = forestRoots[k]->bin[currentNode[q]].nextNode(observation[forestRoots[k]->bin[currentNode[q]].returnFeature()]);
					__builtin_prefetch(&forestRoots[k]->bin[currentNode[q]], 0, 3);
					continue;
				}
				--numberNotInLeaf;
			}
		}while(numberNotInLeaf > 0);

		for( q=0; q<forestRoots[k]->numOfTreesInBin; q++){
			++predictions[forestRoots[k]->bin[currentNode[q]].returnRightNode()];
		}
	}
	return returnClassPrediction(predictions, numOfClasses);
}

/*
int improv8::makePrediction(double*& observation, int numCores){

	std::vector<int> predictions(numOfClasses);
	std::vector<int> currentNode(forestRoots[0]->numOfTreesInBin);
	int numberNotInLeaf;
	int k, q;

	for( k=0; k < numOfBins;k++){

		for( q=0; q<forestRoots[k]->numOfTreesInBin; q++){
			currentNode[q] = q;
			__builtin_prefetch(&forestRoots[k]->bin[currentNode[q]], 0, 3);
		}

		do{
			numberNotInLeaf = forestRoots[k]->numOfTreesInBin;

			for( q=0; q<forestRoots[k]->numOfTreesInBin; q++){

				if(forestRoots[k]->bin[currentNode[q]].isInternalNode()){
					currentNode[q] = forestRoots[k]->bin[currentNode[q]].nextNode(observation[forestRoots[k]->bin[currentNode[q]].returnFeature()]);
					__builtin_prefetch(&forestRoots[k]->bin[currentNode[q]], 0, 3);
					continue;
				}
				--numberNotInLeaf;
			}
		}while(numberNotInLeaf > 0);

		for( q=0; q<forestRoots[k]->numOfTreesInBin; q++){
			++predictions[forestRoots[k]->bin[currentNode[q]].returnRightNode()];
		}
	}
	return returnClassPrediction(predictions, numOfClasses);
}
*/
