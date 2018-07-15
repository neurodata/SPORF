
#ifndef inferenceSamples_h
#define inferenceSamples_h

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

//TODO Each observation should be cache line aligned.
class inferenceSamples
{
    public:
        int numObservations;
        int numFeatures;
				std::vector<int> observationClasses;
        //int* observationClasses = NULL;
        int* predictedClasses = NULL;
        double** samplesMatrix = NULL;

float returnPercentRight();
        void percentRight();

        inferenceSamples(const std::string& testFile);
        ~inferenceSamples();
};
#endif //inferenceSamples_h
