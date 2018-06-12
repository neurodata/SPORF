
#ifndef inferenceSamples_h
#define inferenceSamples_h

#include <stdlib.h>
#include <iostream>
#include <fstream>

//TODO Each observation should be cache line aligned.
class inferenceSamples
{
    public:
        int numObservations;
        int numFeatures;
        int* observationClasses = NULL;
        int* predictedClasses = NULL;
        double** samplesMatrix = NULL;

        void percentRight();

        inferenceSamples(const std::string& testFile);
        ~inferenceSamples();
};
#endif //inferenceSamples_h
