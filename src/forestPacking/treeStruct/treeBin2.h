#ifndef treeBin2_h
#define treeBin2_h

#include <stdlib.h>
#include <iostream>
#include <fstream>


#include "padForest.h"
#include "padNodeStat.h"
#include "padNode.h"
#include <vector>


class treeBin2   
{
	public:
		std::vector<padNode> bin;

		int numOfTreesInBin;
		int depth;
		int firstFreeNode = -1;
		int currProcess = -1;
		int numOfClasses = -1;
		int numOfNodes = -1;

		void repackTree(std::vector<padNodeStat> tree, int workingNode);

		//    public:
		treeBin2(std::ifstream &in);
		treeBin2(std::vector<std::vector<padNodeStat> > forest, std::vector<int>& treeLength, int startTree, int finalTree, int headDepth, int numClasses);
		~treeBin2();
		bool isInternalNode(int currentNodeNum);
};


#endif //treeBin2_h
