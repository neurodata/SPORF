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
                                bool sortbysec(const std::pair<int,int> &a, const std::pair<int,int> &b)
                                {
                                        return (a.second > b.second);
                                }
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

                        inline void printSparseMat(){
                                for (int k = 0; k < eigenMat.outerSize(); ++k){
                                        for (Eigen::SparseMatrix<double>::InnerIterator it(eigenMat, k); it; ++it){
                                                std::cout << it.row() <<"\t";
                                                std::cout << it.col() << "\t";
                                                std::cout << it.value() << "\n";
                                        }
                                }
                        }

                        inline void createSparseMat(){
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

			inline void normalizeByNTrees(){
				double treesize = (double)trees.size();
				for (auto it=pairMat.begin(); it!=pairMat.end(); ++it) {
					it->second = it->second / treesize;
				}
				
			}
			inline void growTrees(){
#pragma omp parallel for num_threads(fpSingleton::getSingleton().returnNumThreads())
				for(int i = 0; i < (int)trees.size(); ++i){
					trees[i].growTree();
					trees[i].updateSimMat(simMat, pairMat);
				}
				//normalizeByNTrees();
			}


			inline void checkParameters(){
				//TODO: check parameters to make sure they make sense for this forest type.
				;
			}

			
                        inline double computePrecision(int k) {
                                // show content:
				double prec = 0;
				int count = 0;
				int numCorrect = 0;
				int labelNeighbor = 0;
                                for (auto it=simMat.begin(); it!=simMat.end(); ++it)
                                {
					std::map<int, int> neighbors;	
					neighbors = it->second;
					count+=1;
					std::vector<std::pair<int, int> > v;
    					copy(neighbors.begin(), neighbors.end(), back_inserter(v));
   					sort(v.begin(), v.end(), sortbysec);
					auto labelCurr = fpSingleton::getSingleton().returnLabel(it->first);
					numCorrect = 0	;
					k = std::min(k, int(v.size() - 1));
					if (k<=0)
						k=1;	
					int i = 0;
					int counter = 0;
					while(counter<k){
						if(it->first == v[i].first)
						{
							i++;
							continue;
						}
						
                                        	labelNeighbor = fpSingleton::getSingleton().returnLabel(v[i].first);
                                        	if (labelCurr == labelNeighbor)
                                                	numCorrect++;
						i++;
						counter++;
                                	}
					prec+= numCorrect/(double)k;
				}
                                return prec/(double)(count);
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
				//std::cout<<"precision @10: "<<computePrecision(10)<<"\n";
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
				//TODO: Generate Error message
				return 0;
			}

			inline int predictClass(const T* observation){
				//TODO: Generate Error message
        			return 0;
                        }

			inline float reportOOB(){
				return 0;
			}
                        inline std::vector<int> predictClassPost(std::vector<T>& observation){
                                return {};
                        }

			inline float testForest(){
				//TODO: Generate Error message
				return 0;
			}

	};

}// namespace fp
#endif //fpForestClassification_h
