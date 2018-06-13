#include "treeBin.h"


void treeBin::repackTree(padNodeStat* tree, int workingNode){

    int nodeNumberInTree = bin[workingNode].returnRightNode();

    if(tree[nodeNumberInTree].isInternalNode()){

        int leftFreq = tree[tree[nodeNumberInTree].returnLeftNode()].returnFreq();
        int rightFreq = tree[tree[nodeNumberInTree].returnRightNode()].returnFreq();
        int leftNodeNum;
        int rightNodeNum;

        if(rightFreq > leftFreq){
            rightNodeNum = ++firstFreeNode;
            bin[rightNodeNum].setNode(tree[tree[nodeNumberInTree].returnRightNode()].returnCutValue(),
                    tree[tree[nodeNumberInTree].returnRightNode()].returnFeature(),
                    0,
                    tree[nodeNumberInTree].returnRightNode());

            repackTree(tree, rightNodeNum);

            leftNodeNum = ++firstFreeNode;
            bin[leftNodeNum].setNode(tree[tree[nodeNumberInTree].returnLeftNode()].returnCutValue(),
                    tree[tree[nodeNumberInTree].returnLeftNode()].returnFeature(),
                    0,
                    tree[nodeNumberInTree].returnLeftNode());

            repackTree(tree, leftNodeNum);
        }else{

            leftNodeNum = ++firstFreeNode;
            bin[leftNodeNum].setNode(tree[tree[nodeNumberInTree].returnLeftNode()].returnCutValue(),
                    tree[tree[nodeNumberInTree].returnLeftNode()].returnFeature(),
                    0,
                    tree[nodeNumberInTree].returnLeftNode());

            repackTree(tree, leftNodeNum);

            rightNodeNum = ++firstFreeNode;
            bin[rightNodeNum].setNode(tree[tree[nodeNumberInTree].returnRightNode()].returnCutValue(),
                    tree[tree[nodeNumberInTree].returnRightNode()].returnFeature(),
                    0,
                    tree[nodeNumberInTree].returnRightNode());

            repackTree(tree, rightNodeNum);
        }

        bin[workingNode].setLeftValue(leftNodeNum);
        bin[workingNode].setRightValue(rightNodeNum);

    }else{
        bin[workingNode] = tree[nodeNumberInTree];
            }
}



treeBin::treeBin(padNodeStat**& forest, int*& treeLength, int startTree, int finalTree, int headDepth){
    numOfTreesInBin = finalTree-startTree;
    depth = headDepth;
    int binSize = 0;

    for(int i = startTree; i < finalTree; i++){
        binSize += treeLength[i];
    }
    bin = new padNode [binSize];

    for(int j = startTree; j < finalTree; j++){
        bin[++firstFreeNode] = forest[j][0];
        //left value is the originating tree
        bin[firstFreeNode].setLeftValue(j);
        //right value is position in originating tree
        bin[firstFreeNode].setRightValue(0);
    }

    int workingTree;
    int workingPosition;
    int lastNodeInLevel;
    for(int j = 0; j < headDepth; j++){
        lastNodeInLevel = firstFreeNode;
        while(currProcess < lastNodeInLevel){
            ++currProcess;
            workingTree =bin[currProcess].returnLeftNode();
            workingPosition =bin[currProcess].returnRightNode();

            if(forest[workingTree][workingPosition].isInternalNode()){
                bin[currProcess].setLeftValue(++firstFreeNode);
                bin[firstFreeNode] = forest[workingTree][forest[workingTree][workingPosition].returnLeftNode()];
                bin[firstFreeNode].setLeftValue(workingTree);
                bin[firstFreeNode].setRightValue(forest[workingTree][workingPosition].returnLeftNode());

                bin[currProcess].setRightValue(++firstFreeNode);
                bin[firstFreeNode] = forest[workingTree][forest[workingTree][workingPosition].returnRightNode()];
                bin[firstFreeNode].setLeftValue(workingTree);
                bin[firstFreeNode].setRightValue(forest[workingTree][workingPosition].returnRightNode());
            }else{

                bin[currProcess] = forest[workingTree][workingPosition];
            }
        }
    }

    int finalTopLevelNode = firstFreeNode; 
    int currTreeNum;
    for(int j = ++currProcess ;j <= finalTopLevelNode  ;j++){
        currTreeNum = bin[j].returnLeftNode();
        repackTree(forest[currTreeNum], j);
    }

}

treeBin::~treeBin(){
    delete[] bin;    
}

