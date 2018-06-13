#ifndef treeBin_h
#define treeBin_h

#include "padForest.h"
#include "inferenceSamples.h"
#include "padNodeStat.h"
#include "padNode.h"


class treeBin   
{
 //   private:
    public:
    padNode* bin = NULL;
    int numOfTreesInBin;
    int depth;
    int firstFreeNode = -1;
    int currProcess = -1;
    void repackTree(padNodeStat* tree, int workingNode);

//    public:
treeBin(padNodeStat**& forest, int*& treeLength, int startTree, int finalTree, int headDepth);
~treeBin();
};


#endif //treeBin_h
