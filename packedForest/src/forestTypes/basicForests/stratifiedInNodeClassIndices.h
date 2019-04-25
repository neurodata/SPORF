#ifndef stratifiedInNodeClassIndices_h
#define stratifiedInNodeClassIndices_h

#include <iostream>
#include <random>
#include <vector>
#include <algorithm>

namespace fp{

	class stratifiedInNodeClassIndices
	{
		private:
			std::vector<std::vector<int> > inSamples;
			std::vector<int> inSamps;
			std::vector<std::vector<int> > outSamples;
			std::vector<int> binSamples;
			int inSampleSize;
			int outSampleSize;

			//TODO: the following functions would benefit from Vitter's Sequential Random Sampling
		public:
			stratifiedInNodeClassIndices(): inSamples(fpSingleton::getSingleton().returnNumClasses()), outSamples(fpSingleton::getSingleton().returnNumClasses()), inSampleSize(0), outSampleSize(0){}


			stratifiedInNodeClassIndices(const int &numObservationsInDataSet): inSamples(fpSingleton::getSingleton().returnNumClasses()), outSamples(fpSingleton::getSingleton().returnNumClasses()), inSampleSize(0), outSampleSize(0){

				createInAndOutSets(numObservationsInDataSet);

				for(auto& inSamps : inSamples){
					// NB: using inSamps as iterator.
					inSampleSize += inSamps.size();
				}

				for(auto& outSamps : outSamples){
					// NB: using outSamps as iterator.
					outSampleSize += outSamps.size();
				}

			}


			inline void createInAndOutSets(const int &numObs){
				std::vector<int> potentialSamples(numObs);

				std::random_device rd; // obtain a random number from hardware
				std::mt19937 eng(rd()); // seed the generator

				std::uniform_int_distribution<> distr(0, numObs-1);

				for(int i=0; i < numObs; ++i){
					potentialSamples[i] = i;
				}

				int numUnusedObs = numObs;
				int randomObsID;
				int tempMoveObs;
				for(int n=0; n<numObs; n++){
					randomObsID = distr(eng);
					inSamples[fpSingleton::getSingleton().returnLabel(potentialSamples[randomObsID])].push_back(potentialSamples[randomObsID]);
					inSamps.push_back(potentialSamples[randomObsID]);
					// swap if we haven't sampled this value before.
					if(randomObsID < numUnusedObs){
						--numUnusedObs;
						tempMoveObs = potentialSamples[numUnusedObs];
						potentialSamples[numUnusedObs] = potentialSamples[randomObsID];
						potentialSamples[randomObsID] = tempMoveObs;
					}
				}

				for(int n=0; n<numUnusedObs; ++n){
					outSamples[fpSingleton::getSingleton().returnLabel(potentialSamples[n])].push_back(potentialSamples[n]);
				}
			}


			inline double returnImpurity(){
				if(false){
					unsigned int sumClassTotalsSquared = 0;
					for(auto i : inSamples){
						sumClassTotalsSquared+=i.size()*i.size();
					}
					return 1-double(sumClassTotalsSquared)/(inSampleSize*inSampleSize);
				}else{
					double impSum = 0;
					double classPercent;
					for(auto i : inSamples){
						classPercent = double(i.size())/double(inSampleSize);

						impSum += double(i.size())*(1.0-classPercent);
					}
					return impSum;
				}
			}

			inline void printIndices(){
				std::cout << "samples in bag\n";
				for(unsigned int n = 0; n < inSamples.size(); ++n){
					for(auto & i : inSamples[n]){
						std::cout << i << "\n";
					}
				}

				std::cout << "samples OOB\n";
				for(unsigned int n = 0; n < outSamples.size(); ++n){
					for(auto & i : outSamples[n]){
						std::cout << i << "\n";
					}
				}
			}


			inline int returnInSampleSize(){
				return inSampleSize;
			}


			inline int returnOutSampleSize(){
				return outSampleSize;
			}


			inline int returnInSample(const int numSample){
				return inSamps[numSample];
				//The commented out below reduces memory size but is slow.
				/*
					 int totalViewed = 0;
					 for(unsigned int i = 0; i < inSamples.size(); ++i){
					 if(numSample < (totalViewed+int(inSamples[i].size()))){
					 if((numSample-totalViewed)<0 || (numSample-totalViewed)>=int(inSamples[i].size())){
					 std::cout << numSample-totalViewed << " , " << inSamples[i].size() << "\n";
					 exit(1);
					 }
					 int retNum = inSamples[i][numSample-totalViewed];
					 return retNum ;
					 }
					 totalViewed += inSamples[i].size();
					 }
					 std::cout << "it happened now\n";
					 exit(1);
					 return -1;
					 */
			}

			inline int returnOutSamplesInClass(int classNum){
				return outSamples[classNum].size();
			}

			inline int returnOutSample(const int numSample){
				int totalViewed = 0;
				for(unsigned int i = 0; i < outSamples.size(); ++i){
					if(numSample < totalViewed+int(outSamples[i].size())){
						return outSamples[i][numSample-totalViewed];
					}
					totalViewed += outSamples[i].size();
				}
				return -1;
			}

			inline std::vector<int> returnOutSample(){
				std::vector<int> outIndices;
				for (auto& i : outSamples){
					for (auto& j : i) {
						outIndices.push_back(j);
					}
				}
				return outIndices;
			}


			inline int returnBinSize(){
				return fpSingleton::getSingleton().returnBinSize();
			}

inline int returnBinMin(){
				return fpSingleton::getSingleton().returnBinMin();
			}

			inline bool useBin(){
				return fpSingleton::getSingleton().returnUseBinning() && (inSampleSize > returnBinMin());
			}

			inline void initializeBinnedSamples(){
				if(useBin()){
					int numInClass;
					std::random_device random_device;
					std::mt19937 engine{random_device()};
					for(unsigned int i = 0; i < inSamples.size(); ++i){
						numInClass = int((returnBinSize()*inSamples[i].size())/inSampleSize);
						for(int n = 0; n < numInClass; ++n){
							std::uniform_int_distribution<int> dist(0, inSamples[i].size() - 1);
							binSamples.push_back(inSamples[i][dist(engine)]);
						}
					}
				}
			}


			inline int returnBinnedSize(){
				return binSamples.size();
			}


			inline int returnBinnedInSample(const int numSample){
				return	binSamples[numSample];
			}


			inline void addIndexToOutSamples(int index){
				++outSampleSize;
				outSamples[fpSingleton::getSingleton().returnLabel(index)].push_back(index);
			}

			inline void addIndexToInSamples(int index){
				++inSampleSize;
				
				inSamples[fpSingleton::getSingleton().returnLabel(index)].push_back(index);
				inSamps.push_back(index);
			}
	};//class stratifiedInNodeClassIndices

}//namespace fp
#endif //stratifiedInNodeClassIndices_h

