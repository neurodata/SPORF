#ifndef padForest_h
#define padForest_h

#include "padNode.h"

class padForest
{
    public:
padForest() : debugModeOn(false), showAllResults(false){}
    void virtual printForest();
    void virtual makePredictions();
    void toggleDebugMode();
    void toggleShowAllResults();

    protected:
    int numTreesInForest;
    int totalNumberOfNodes;
    int numOfClasses;
    bool debugModeOn;
    bool showAllResults;

    };
#endif //padForest.h
