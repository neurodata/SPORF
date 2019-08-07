#ifndef fpRerF_h
#define fpRerf_h

#include "../../../baseFunctions/fpForestBase.h"
#include <vector>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <stdio.h>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include "urerfTree.h"
#include <sys/time.h>

namespace fp {
	template <typename T>
		class fpURerFBase : public fpForestBase<T>
	{
		protected:
			std::vector<urerfTree<T> > trees;
			std::map<int, std::map<int, int> > simMat;
                        std::map<std::pair<int, int>, double> pairMat;
                        typedef Eigen::SparseMatrix<int> spMat;
                        typedef Eigen::Triplet<int> TripType;
                        std::vector<TripType> tripletList;
                        SpMat eigenMat;
		public:

			~fpURerFBase(){}

			fpDisplayProgress printProgress;
			inline void printForestType(){
				std::cout << "This is a urerf forest.\n";
			}

			inline void changeForestSize(){
				trees.resize(fpSingleton::getSingleton().returnNumTrees());
			}

			inline void initSimMat(){
				auto numObs = fpSingleton::getSingleton().returnNumObservations();
				for(auto i = 0; i < numObs; ++i) {
					std::map<int, int> init_map;
					simMat[i] = init_map;
				}
			}

                        inline void createSparseMat(){
				//Not in use now. TODO: Remove entirely?
				auto numObs = fpSingleton::getSingleton().returnNumObservations();
				SpMat eigenSimMat(numObs, numObs);
				for (auto it=pairMat.begin(); it!=pairMat.end(); ++it) {
					int i = (it->first).first;
					int j = (it->first).second;
					int v_ij = it->second;
					eigenSimMat.coeffRef(i, j) = v_ij;
				}
				eigenSimMat.makeCompressed();
				this->eigenMat = eigenSimMat;
			}

			inline void growTrees(){
#pragma omp parallel for num_threads(fpSingleton::getSingleton().returnNumThreads())
				for(int i = 0; i < (int)trees.size(); ++i){
					trees[i].growTree();
					trees[i].updateSimMat(simMat, pairMat);
					trees[i].updateSimMatOut(simMat, pairMat);
				}
			}


			inline void checkParameters(){
				//TODO: check parameters to make sure they make sense for this forest type.
				;
			}

			inline void treeStats(){
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
			}


			inline std::map<int, std::map<int, int> > returnSimMat() {
				return simMat;
			}

			inline std::map<std::pair<int, int>, double> returnPairMat(){
                                        return pairMat;
                                }

			void printTree0(){
				trees[0].printTree();
			}

			void growForest(){
				changeForestSize();
				growTrees();
				treeStats();
			}

			inline int predictClass(std::vector<T> &observation)
			{
				std::cout << "Not implemented for unsupervised forests\n";
				return 0;
			}

			inline int predictClass(const T *observation)
			{
				std::cout << "Not implemented for unsupervised forests\n";
				return 0;
			}

			inline float reportOOB()
			{
				return 0;
			}
			
			inline std::vector<int> predictClassPost(std::vector<T> &observation)
			{
				std::cout << "Not implemented for unsupervised forests\n";
				return {};
			}

			inline float testForest()
			{
				return 0;
			}
	};

}// namespace fp
#endif
