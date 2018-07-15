
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
				std::vector<int> predictedClasses;
				std::vector<std::vector<double> > samplesMatrix;

float returnPercentRight();
        void percentRight();

        inferenceSamples(const std::string& testFile);
        ~inferenceSamples();
};
#endif //inferenceSamples_h
