#include "padNodeStat.h"

bool padNodeStat::isInternalNode(){
    ++timesUsed;
    return padNode::isInternalNode();
}

int padNodeStat::returnFreq(){
    return timesUsed;
}
