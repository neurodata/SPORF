#ifndef fpURF_h
#define fpURF_h

#include "../../../baseFunctions/fpForestBase.h"
#include <vector>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <stdio.h>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include "urfTree.h"
#include <sys/time.h>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/Core>
using namespace Eigen;

namespace fp {

	template <typename T>
		class fpURFBase : public fpForestBase<T>
	{
		protected:
			std::vector<urfTree<T> > trees;
			std::map<int, std::map<int, int> > simMat;
			std::map<std::pair<int, int>, double> pairMat;	
			typedef Eigen::SparseMatrix<int> spMat;
			typedef Eigen::Triplet<int> TripType;
			std::vector<TripType> tripletList;
			SpMat eigenMat;
		public:

			~fpURFBase(){}

			fpDisplayProgress printProgress;
			inline void printForestType(){
				std::cout << "This is a urf forest.\n";
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

			inline void createSparseMat(){
				//Not in use now. TODO: Remove entirely?
				auto numObs = fpSingleton::getSingleton().returnNumObservations();
				SpMat eigenSimMat(numObs, numObs);
				for (auto it=pairMat.begin(); it!=pairMat.end(); ++it){
					int i = (it->first).first;
					int j = (it->first).second;
					int v_ij = it->second;
					eigenSimMat.coeffRef(i, j) = v_ij;
				}
				eigenSimMat.makeCompressed();
				this->eigenMat = eigenSimMat ;
			}			

			
			inline void printSparseMat(){
				//Not in use now. TODO: Remove entirely?
				for (int k = 0; k < eigenMat.outerSize(); ++k){
    					for (Eigen::SparseMatrix<double>::InnerIterator it(eigenMat, k); it; ++it){
        					std::cout << it.row() <<"\t";
        					std::cout << it.col() << "\t";
        					std::cout << it.value() << "\n";
    					}
				}
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

			inline int predictClass(std::vector<T>& observation){
				std::cout<<"Not defined for unsupervised random forests. \n";
				return 0;
			}

			inline int predictClass(const T *observation)
			{
				std::cout << "Not defined for unsupervised random forests. \n";
				return 0;
			}
			inline std::vector<int> predictClassPost(std::vector<T> &observation)
			{
				std::cout << "Not defined for unsupervised random forests. \n";
				return {};
			}

			inline float reportOOB()
			{
				return 0;
			}
			inline float testForest()
			{
				return 0;
			}
	};

}// namespace fp
#endif
