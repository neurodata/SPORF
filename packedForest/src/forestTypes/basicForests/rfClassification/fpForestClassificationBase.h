#ifndef fpForestClassification_h
#define fpForestClassification_h

#include "../../../baseFunctions/fpForestBase.h"
#include <vector>
#include <stdio.h>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <omp.h>
#include "rfTree.h"
#include <map>

namespace fp {

	template <typename T>
		class fpForestClassificationBase : public fpForestBase<T>
	{
		protected:
			std::vector<rfTree<T> > trees;
			int numCorrect = 0;
			int numOOB = 0;
			std::map<std::pair<int, int>, double> pairMat;
		public:

			fpDisplayProgress printProgress;

			//			using fpForestBase<T>::fpForestBase;
			~fpForestClassificationBase(){}

			inline void printForestType(){
				std::cout << "This is a basic classification forest.\n";
			}

			inline void changeForestSize(){
				trees.resize(fpSingleton::getSingleton().returnNumTrees());
			}

			inline void growTrees(){

#pragma omp parallel for num_threads(fpSingleton::getSingleton().returnNumThreads())
				for(unsigned int i = 0; i < trees.size(); ++i){
					printProgress.displayProgress(i);
					trees[i].growTree();
				}
				std::cout << "\n"<< std::flush;
			}

			inline void checkParameters(){
				//TODO: check parameters to make sure they make sense for this forest type.
				;
			}

			inline std::map<std::string, float> calcTreeStats(){
				// rewrite as a struct in a top level header file.
				int maxDepth=0;
				int totalLeafNodes=0;
				int totalLeafDepth=0;

				int tempMaxDepth;
				for(int i = 0; i < fpSingleton::getSingleton().returnNumTrees(); ++i){
					tempMaxDepth = trees[i].returnMaxDepth();
					maxDepth = ((maxDepth < tempMaxDepth) ? tempMaxDepth : maxDepth);

					totalLeafNodes += trees[i].returnNumLeafNodes();
					totalLeafDepth += trees[i].returnLeafDepthSum();
				}

				std::map<std::string, float> treeStats;
				treeStats["maxDepth"] = (float) maxDepth;
				treeStats["totalLeafDepth"] = (float) totalLeafDepth;
				treeStats["totalLeafNodes"] = (float) totalLeafNodes;
				treeStats["OOBaccuracy"] = reportOOB();
				return treeStats;
			}

			inline void treeStats(){
				std::map<std::string, float> treeStats = calcTreeStats();

				std::cout << "max depth: " << treeStats["maxDepth"] << "\n";
				std::cout << "avg depth: " << float(treeStats["totalLeafDepth"])/float(treeStats["totalLeafNodes"]) << "\n";
				std::cout << "num leaf nodes: " << treeStats["totalLeafNodes"] << "\n";
				std::cout << "OOB Accuracy: " << treeStats["OOBaccuracy"] << "\n";
			}

			void printTree0(){
				trees[0].printTree();
			}

			void growForest(){
				//	checkParameters();
				changeForestSize();
				growTrees();
				treeStats();
			}

			inline int predictClass(int observationNumber){
				std::vector<int> classTally(fpSingleton::getSingleton().returnNumClasses(),0);
				for(int i = 0; i < fpSingleton::getSingleton().returnNumTrees(); ++i){
					++classTally[trees[i].predictObservation(observationNumber)];
				}

				int bestClass = 0;
				for(int j = 1; j < fpSingleton::getSingleton().returnNumClasses(); ++j){
					if(classTally[bestClass] < classTally[j]){
						bestClass = j;
					}
				}
				return bestClass;
			}

inline std::map<std::pair<int, int>, double> returnPairMat(){
                                        return pairMat;
                                }

inline int predictClass(const T* observation){
	/*
				std::vector<int> classTally(fpSingleton::getSingleton().returnNumClasses(),0);
				for(int i = 0; i < fpSingleton::getSingleton().returnNumTrees(); ++i){
					++classTally[trees[i].predictObservation(observation)];
				}

				int bestClass = 0;
				for(int j = 1; j < fpSingleton::getSingleton().returnNumClasses(); ++j){
					if(classTally[bestClass] < classTally[j]){
						bestClass = j;
					}
				}
				return bestClass;
				*/
	return 0;
			}


			inline int predictClass(std::vector<T>& observation){
				std::vector<int> classTally(fpSingleton::getSingleton().returnNumClasses(),0);
				for(int i = 0; i < fpSingleton::getSingleton().returnNumTrees(); ++i){
					++classTally[trees[i].predictObservation(observation)];
				}

				int bestClass = 0;
				for(int j = 1; j < fpSingleton::getSingleton().returnNumClasses(); ++j){
					if(classTally[bestClass] < classTally[j]){
						bestClass = j;
					}
				}
				return bestClass;
			}


			inline std::vector<int> predictClassPost(std::vector<T>& observation){
				std::vector<int> classTally(fpSingleton::getSingleton().returnNumClasses(),0);
				for(int i = 0; i < fpSingleton::getSingleton().returnNumTrees(); ++i){
					++classTally[trees[i].predictObservation(observation)];
				}
				return classTally;
			}


			inline float testForest(){
				int numTried = 0;
				int numWrong = 0;

				for (int i = 0; i <fpSingleton::getSingleton().returnNumObservations();i++){
					++numTried;
					int predClass = predictClass(i);

					if(predClass != fpSingleton::getSingleton().returnTestLabel(i)){
						++numWrong;
					}
				}
				std::cout << "\nnumWrong= " << numWrong << "\n";

				return (float)numWrong/(float)numTried;
			}

			inline float reportOOB(){
				int numCorrect = 0;
				int numOOB = 0;
				float oobAccuracy = 0;

				std::map<int, int> oobBestClass;
				// A vector of vectors (numObs X numClasses) for storing
				// the class tallies.
			    std::vector<std::vector<int>> oobClassVotes(fpSingleton::getSingleton().returnNumObservations(), std::vector<int>(fpSingleton::getSingleton().returnNumClasses(), 0));

				// Iterate over trees to get oob points and add up their
				// class votes. returnOOBvotes is an n x 2 vector with
				// <(oobID, treeVote)>.
				for (auto& ti : trees){
					for (auto j : ti.returnOOBvotes()){
						// increment the vote for class j[1] for
						// oob_point j[0].
						oobClassVotes[j[0]][j[1]] += 1;
					}
				}

				// Iterate over "rows" and calculate the bestClass.
				for (unsigned int i = 0; i < oobClassVotes.size(); i++){
					if(std::any_of(oobClassVotes[i].begin(), oobClassVotes[i].end(), [](int i) {return i>0;})){
						// This will pick the lower class value if there
						// is a tie. (same as in the predict methods).
						// TODO: flip a coin for tie-breaking.
						oobBestClass[i] = std::distance(oobClassVotes[i].begin(), std::max_element(oobClassVotes[i].begin(), oobClassVotes[i].end()));
						numOOB++;
						// using distance is a way to get the index of
						// the max value in the vector.
						// http://www.cplusplus.com/forum/beginner/212806/#msg994102
					}
				}


				for (auto& i : oobBestClass){
					// Tally the number of correct predictions.
					// i.first is the observation index.
					// i.second is the predicted class.
					if(fpSingleton::getSingleton().returnLabel(i.first) == i.second){
						numCorrect++;
					}
				}

				this->numCorrect = numCorrect;
				this->numOOB = numOOB;

				oobAccuracy = (float) numCorrect / (float) numOOB;

				return oobAccuracy;
			}

			/*
			 * testing functions -- not to be used in production
			 */

			inline std::vector<std::vector<T> > testOneTreeOOB(){
				std::vector<std::vector<T> > dataValues;

				std::vector<int> oobIndices;
				std::map<int, int> oobBestClass;
				// A vector of vectors (numObs X numClasses) for storing
				// the class tallies.
			    std::vector<std::vector<int>> oobClassVotes(fpSingleton::getSingleton().returnNumObservations(), std::vector<int>(fpSingleton::getSingleton().returnNumClasses(), 0));

				// Iterate over trees to get oob points and add up their
				// class votes.
				for (unsigned int i = 0; i < trees.size(); i++){
					for (auto j : trees[i].returnOOBvotes()){
						oobClassVotes[j[0]][j[1]] += 1;
					}
				}

				// Iterate over "rows" and calculate the bestClass.
				for (unsigned int i = 0; i < oobClassVotes.size(); i++){
					if(!std::all_of(oobClassVotes[i].begin(), oobClassVotes[i].end(), [](int i) {return i==0;})){
						// This will pick the lower class value if there
						// is a tie. (same as in the predict methods).
						// TODO: flip a coin for tie-breaking.
						oobIndices.push_back(i);
					}
				}


				for(auto& i : oobIndices){
					std::vector<T> tmp;
					for(int j = 0; j < fpSingleton::getSingleton().returnNumFeatures(); j++){
						tmp.push_back(fpSingleton::getSingleton().returnFeatureVal(j, i));
					}
					dataValues.push_back(tmp);
				}
				return dataValues;
			}

			inline std::vector<int> testOneTreeOOBind(){

				std::vector<int> oobIndices;
				std::map<int, int> oobBestClass;
				// A vector of vectors (numObs X numClasses) for storing
				// the class tallies.
			    std::vector<std::vector<int>> oobClassVotes(fpSingleton::getSingleton().returnNumObservations(), std::vector<int>(fpSingleton::getSingleton().returnNumClasses(), 0));

				// Iterate over trees to get oob points and add up their
				// class votes.
				for (unsigned int i = 0; i < trees.size(); i++){
					for (auto j : trees[i].returnOOBvotes()){
						oobClassVotes[j[0]][j[1]] += 1;
					}
				}

				// Iterate over "rows" and calculate the bestClass.
				for (unsigned int i = 0; i < oobClassVotes.size(); i++){
					if(!std::all_of(oobClassVotes[i].begin(), oobClassVotes[i].end(), [](int i) {return i==0;})){
						// This will pick the lower class value if there
						// is a tie. (same as in the predict methods).
						// TODO: flip a coin for tie-breaking.
						oobIndices.push_back(i);
					}
				}

				return oobIndices;
			}


			inline std::map<std::string, int> testReturnNumCorrectAndNumOOB(){
				std::map<std::string, int> retVal;
				retVal["numCorrect"] = numCorrect;
				retVal["numOOB"] = numOOB;
				return retVal;
			}
	};

}// namespace fp
#endif //fpForestClassification_h
