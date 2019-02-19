#ifndef padNodeStat_h
#define padNodeStat_h
#include <cstdint>
#include "padNode.h"

class padNodeStat: public padNode
{
    private:
    uint32_t timesUsed;

    public:
    padNodeStat(): timesUsed(0){}
    bool isInternalNode();
    int returnFreq();
};
#endif //padNodeStat_h
