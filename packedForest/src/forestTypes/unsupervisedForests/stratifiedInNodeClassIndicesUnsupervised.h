#ifndef stratifiedInNodeClassIndicesUnsupervised_h
#define stratifiedInNodeClassIndicesUnsupervised_h

#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <limits>

namespace fp{

	class stratifiedInNodeClassIndicesUnsupervised
	{
		private:
			std::vector<std::vector<int> > inSamples;
			std::vector<int> inSamps;
			std::vector<int> outSamps;
			std::vector<std::vector<int> > outSamples;
			std::vector<int> binSamples;
			int inSampleSize;
			int outSampleSize;
			double impurity;

			//TODO: the following functions would benefit from Vitter's Sequential Random Sampling
		public:
			stratifiedInNodeClassIndicesUnsupervised(): inSampleSize(0), outSampleSize(0){}


			stratifiedInNodeClassIndicesUnsupervised(const int &numObservationsInDataSet): inSampleSize(0), outSampleSize(0){
				impurity = 10; //initialize to an arbitrary non zero value
				createInAndOutSetsBagging(numObservationsInDataSet, 0);
				inSampleSize = inSamps.size();
				outSampleSize = outSamps.size();
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
					if(randomObsID < numUnusedObs){
						--numUnusedObs;
						tempMoveObs = potentialSamples[numUnusedObs];
						potentialSamples[numUnusedObs] = potentialSamples[randomObsID];
						potentialSamples[randomObsID] = tempMoveObs;
					}
				}

				for(int n=0; n<numUnusedObs; ++n){
					outSamples[fpSingleton::getSingleton().returnLabel(potentialSamples[randomObsID])].push_back(potentialSamples[n]);
					outSamps.push_back(potentialSamples[n]);
					}
				
			}

			inline void createInAndOutSetsBagging(const int &numObs, float bagging){
                                //TODO: We might want to refactor this when we move this over to the binned version.
				std::vector<int> random_indices(numObs);
				std::vector<int> random_indices2;
				std::vector<int> random_indices3;

				for(int i=0; i < numObs; ++i){
					random_indices[i] = i;
				}
				std::random_shuffle(random_indices.begin(), random_indices.end());
				int indx = (int) ((1-bagging)*(float)numObs);
				int counter = 0;
				for(auto i : random_indices)
				{
					if(counter < indx)
						random_indices2.push_back(i);
					else
						random_indices3.push_back(i);
					counter++;
				}
				
				for(auto randomObsID : random_indices2)
                                        inSamps.push_back(randomObsID);
				for(auto randomObsID2 : random_indices3)
					outSamps.push_back(randomObsID2);
			}

			inline void setNodeImpurity(double nodeImp){
				if(nodeImp < std::numeric_limits<double>::epsilon())
					nodeImp = 0;
				impurity = nodeImp;
			}
			inline double returnImpurity(){
				return impurity;
			}

			inline void printIndices(){
				std::cout << "samples in bag\n";
				std::cout<<inSamples.size()<<"\n";
				for( unsigned int n = 0; n < inSamps.size(); ++n){
					std::cout<<inSamps[n]<<" ";
					for(auto & i : inSamples[n]){
						std::cout << i << "\n";
					}
				}
				std::cout<<"\n";

				std::cout << "samples OOB\n";
				for(unsigned int n = 0; n < outSamples.size(); ++n){
					for(auto & i : outSamples[n]){
						std::cout << i << "\n";
					}
				}
			}

			inline std::vector<int> returnInSampsVec(){
				return inSamps;
			} 

			inline std::vector<int> returnOutSampsVec(){
				return outSamps;
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

			inline int returnOutSample(const int numSample){
				return outSamps[numSample];
			}

			inline int returnBinSize(){
				return fpSingleton::getSingleton().returnBinSize();
			}

			inline bool useBin(){
				return fpSingleton::getSingleton().returnUseBinning() && (inSampleSize > returnBinSize());
			}

			inline void initializeBinnedSamples(){
				/*if(useBin()){
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
				}*/
			}


			inline int returnBinnedSize(){
				return binSamples.size();
			}


			inline int returnBinnedInSample(const int numSample){
				return	binSamples[numSample];
			}


			inline void addIndexToOutSamples(int index){
				++outSampleSize;
				outSamps.push_back(index);
			}

			inline void addIndexToInSamples(int index){
				++inSampleSize;
				inSamps.push_back(index);
			}
	};//class stratifiedInNodeClassIndices

}//namespace fp
#endif //stratifiedInNodeClassIndices_h

