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
			std::vector<std::vector<int> > outSamples;
			std::vector<int> binSamples;
			int inSampleSize;
			int outSampleSize;

			int binSize = 512;
			int useBinning = false;

			//TODO: the following functions would benefit from Vitter's Sequential Random Sampling
		public:
			stratifiedInNodeClassIndices(){
				inSamples.resize(fpSingleton::getSingleton().returnNumClasses());
				outSamples.resize(fpSingleton::getSingleton().returnNumClasses());

				inSampleSize =0;
				outSampleSize =0;
			}

			stratifiedInNodeClassIndices(const int &numObservationsInDataSet){

				std::vector<int> potentialSamples;
				potentialSamples.resize(numObservationsInDataSet);

				inSamples.resize(fpSingleton::getSingleton().returnNumClasses());
				outSamples.resize(fpSingleton::getSingleton().returnNumClasses());

				std::random_device rd; // obtain a random number from hardware
				std::mt19937 eng(rd()); // seed the generator

				std::uniform_int_distribution<> distr(0, numObservationsInDataSet-1);

				for(int i=0; i < numObservationsInDataSet; ++i){
					potentialSamples[i] = i;
				}

				int numUnusedObs = numObservationsInDataSet;
				int randomObsID;
				int tempMoveObs;
				for(int n=0; n<numObservationsInDataSet; n++){
					randomObsID = distr(eng);
					inSamples[fpSingleton::getSingleton().returnLabel(potentialSamples[randomObsID])].push_back(potentialSamples[randomObsID]);
					if(randomObsID < numUnusedObs){
						--numUnusedObs;
						tempMoveObs = potentialSamples[numUnusedObs];
						potentialSamples[numUnusedObs] = potentialSamples[randomObsID];
						potentialSamples[randomObsID] = tempMoveObs;
					}
				}

				for(int n=0; n<numUnusedObs; ++n){
					outSamples[fpSingleton::getSingleton().returnLabel(potentialSamples[randomObsID])].push_back(potentialSamples[n]);
				}

				inSampleSize =0;
				outSampleSize =0;

				for(unsigned int i = 0; i < inSamples.size(); ++i){
					inSampleSize += inSamples[i].size();
				}
				for(unsigned int i = 0; i < outSamples.size(); ++i){
					outSampleSize += outSamples[i].size();
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

			void printIndices(){
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
				int totalViewed = 0;
//				std::cout << "ifn1\n";
				for(unsigned int i = 0; i < inSamples.size(); ++i){
					if(numSample < (totalViewed+int(inSamples[i].size()))){
						if((numSample-totalViewed)<0 || (numSample-totalViewed)>=int(inSamples[i].size())){
							std::cout << numSample-totalViewed << " , " << inSamples[i].size() << "\n";
							exit(1);
						}
						int retNum = inSamples[i][numSample-totalViewed];
//						std::cout << "ifn12\n";
						return retNum ;
					}
					totalViewed += inSamples[i].size();
				}
				std::cout << "it happened now\n";
				exit(1);
				return -1;
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


			inline bool useBin(){
				return (inSampleSize > 5*binSize) && useBinning;
			}


			inline void initializeBinnedSamples(){
				if(useBin()){
					int numInClass;
					std::random_device random_device;
					std::mt19937 engine{random_device()};
					for(unsigned int i = 0; i < inSamples.size(); ++i){
						numInClass = int((binSize*inSamples[i].size())/inSampleSize);
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
				/*
					 if(index < 0){
					 std::cout << "here1";
					 exit(1);
					 }
					 if(index >= fpSingleton::getSingleton().returnNumObservations()){
					 std::cout << "here3";
					 exit(1);
					 }
					 if(int(outSamples.size()) <= fpSingleton::getSingleton().returnLabel(index)){
					 std::cout << "here31";
					 exit(1);
					 }
					 if(0 > fpSingleton::getSingleton().returnLabel(index)){
					 std::cout << "here32";
					 exit(1);
					 }
					 */
				++outSampleSize;
				outSamples[fpSingleton::getSingleton().returnLabel(index)].push_back(index);
			}

			inline void addIndexToInSamples(int index){
				++inSampleSize;
				if(fpSingleton::getSingleton().returnLabel(index)<0 || fpSingleton::getSingleton().returnLabel(index) >= int(inSamples.size())){
					std::cout << "asldkjf\n";
					exit(1);
				}
				inSamples[fpSingleton::getSingleton().returnLabel(index)].push_back(index);
			}
	};//class stratifiedInNodeClassIndices

}//namespace fp
#endif //stratifiedInNodeClassIndices_h

