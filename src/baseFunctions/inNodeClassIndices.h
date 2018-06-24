#ifndef inNodeClassIndices_h
#define inNodeClassIndices_h
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>

namespace fp{

	class inNodeClassIndices
	{
		private:
			std::vector<int> inSamples;
			std::vector<int> outSamples;

			//TODO: the following functions would benefit from Vitter's Sequential Random Sampling
		public:
			inNodeClassIndices(){}
			inNodeClassIndices(const int &numObservationsInDataSet){

				inSamples.resize(numObservationsInDataSet);

				std::random_device rd; // obtain a random number from hardware
				std::mt19937 eng(rd()); // seed the generator
				std::uniform_int_distribution<> distr(0, numObservationsInDataSet-1);

				for(int n=0; n<numObservationsInDataSet; n++){
					inSamples[n] = distr(eng);
				}

				std::sort(inSamples.begin(), inSamples.end());

				int checkNum = 0;
				int n = 0;
				while(checkNum < numObservationsInDataSet){
					//ensure stopping spot exists
					inSamples.emplace_back(numObservationsInDataSet);
					//find which values are missing
					if(checkNum < inSamples[n]){
						outSamples.push_back(checkNum);
						++checkNum;
					}else if(inSamples[n] < checkNum){
						++n;
					}else{
						++n;
						++checkNum;
					}
				//remove ensured stopping spot.	
					inSamples.pop_back();
				}

			}

			void printIndices(){
				std::cout << "samples in bag\n";
				for(auto & i : inSamples){
					std::cout << i << "\n";
				}
				std::cout << "samples OOB\n";
				for(auto & i : outSamples){
					std::cout << i << "\n";
				}
			}

			inline int returnInSampleSize(){
				return inSamples.size();
			}

			inline int returnOutSampleSize(){
				return outSamples.size();
			}

			inline int returnInSample(const int numSample){
				return inSamples[numSample];
			}

			inline int returnOutSample(const int numSample){
				return outSamples[numSample];
			}

			inline std::vector<int>& returnInSamples(){
				return inSamples;
			}

			inline std::vector<int>& returnOutSamples(){
				return outSamples;
			}

			inline void addIndexToOutSamples(int index){
				outSamples.push_back(index);
			}

			inline void addIndexToInSamples(int index){
				inSamples.push_back(index);
			}
	};//class inNodeClassIndices
}//namespace fp
#endif //inNodeClassIndices_h
