#ifndef fpDetermineOOB_h
#define fpDetermineOOB_h
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>

namespace fp{

	class OOB
	{
		private:
			int numObservations;
			int numOOB;
			std::vector<int> inSamples;
			std::vector<int> outSamples;

		public:
			OOB(const int &numObs){
				numObservations = numObs;
				numOOB = 0;
				inSamples.resize(numObservations);
				//		outSamples.resize(numObservations*.4);

				std::random_device rd; // obtain a random number from hardware
				std::mt19937 eng(rd()); // seed the generator
				std::uniform_int_distribution<> distr(0, numObservations-1);

				for(int n=0; n<numObservations; n++){
					inSamples[n] = distr(eng);
				}
				std::sort(inSamples.begin(), inSamples.end());

				int checkNum = 0;
				int n = 0;
				while(checkNum < numObservations){
					if(checkNum < inSamples[n]){
						outSamples.push_back(checkNum);
						++checkNum;
					}else if(inSamples[n] < checkNum){
						++n;
					}else{
						++n;
						++checkNum;
					}
				}
			}


			void printOOB(){
				std::cout << "samples in bag\n";
				for(int n=0; n < numObservations; n++){
					std::cout << inSamples[n] << "\n";
				}
				std::cout << "samples OOB\n";
				for (std::vector<int>::iterator it = outSamples.begin() ; it != outSamples.end(); ++it){
					std::cout << *it << "\n";
				}
			}

			int returnInSampleSize(){
				return inSamples.size();
			}

			int returnOutSampleSize(){
				return outSamples.size();
			}

			inline int returnInSample(const int &numSample){
				return inSamples[numSample];
			}

			inline int returnOutSample(const int &numSample){
				return outSamples[numSample];
			}
	};//class fpDetermineOOB

}//namespace fp
#endif //fpDetermineOOB_h
