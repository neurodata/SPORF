#ifndef treeBin2_h
#define treeBin2_h

#include "padForest.h"
#include "inferenceSamples.h"
#include "padNodeStat.h"
#include "padNode.h"


class treeBin2   
{
 //   private:
    public:
    padNode* bin = NULL;
    int numOfTreesInBin;
    int depth;
    int firstFreeNode = -1;
    int currProcess = -1;
    int numOfClasses = -1;
    int numOfNodes = -1;
    void repackTree(padNodeStat* tree, int workingNode);

//    public:
treeBin2(std::ifstream &in);
treeBin2(padNodeStat**& forest, int*& treeLength, int startTree, int finalTree, int headDepth, int numClasses);
~treeBin2();
bool isInternalNode(int currentNodeNum);
};


#endif //treeBin2_h
