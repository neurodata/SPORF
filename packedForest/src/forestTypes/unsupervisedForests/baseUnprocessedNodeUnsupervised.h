#ifndef baseUnprocessedNodeUnsupervised_h
#define baseUnprocessedNodeUnsupervised_h
#include "stratifiedInNodeClassIndicesUnsupervised.h"
#include <vector>

namespace fp{


	template <typename T> //
		class baseUnprocessedNodeUnsupervised{
			protected:
				int parentID;
				int depth;
				double nodeImpurity; //lower impurity is better
				bool isLeftNode; //in order to set parent node with location


				stratifiedInNodeClassIndicesUnsupervised* obsIndices;
				stratifiedInNodeClassIndicesUnsupervised* leftIndices;
				stratifiedInNodeClassIndicesUnsupervised* rightIndices;

				std::vector<T> featureHolder;
				
			public:
				baseUnprocessedNodeUnsupervised(int numObsForRoot):  parentID(0), depth(0), isLeftNode(true){
					obsIndices = new stratifiedInNodeClassIndicesUnsupervised(numObsForRoot);
				}

				baseUnprocessedNodeUnsupervised(int parentID, int dep, bool isLeft): parentID(parentID), depth(dep), isLeftNode(isLeft){}

				virtual ~baseUnprocessedNodeUnsupervised(){}
				

				inline stratifiedInNodeClassIndicesUnsupervised* returnLeftIndices(){
					return leftIndices;
				}

				inline stratifiedInNodeClassIndicesUnsupervised* returnRightIndices(){
					return rightIndices;
				}

				inline stratifiedInNodeClassIndicesUnsupervised* returnObsIndices(){
					return obsIndices;
				}

				inline int returnParentID(){
					return parentID;
				}

				inline int returnDepth(){
					return depth;
				}
				inline double returnNodeImpurity(){
					return nodeImpurity;
				}

				inline void setNodeImpurity(double nodeImp){
					nodeImpurity = nodeImp;
				}

				inline bool isNodePure(){
					return nodeImpurity == 0;
				}

				inline bool returnIsLeftNode(){
					return isLeftNode;
				}

				inline int returnInSampleSize(){
					return obsIndices->returnInSampleSize();
				}

				inline int returnOutSampleSize(){
					return obsIndices->returnOutSampleSize();
				}

				inline void setHolderSizes(){
					obsIndices->initializeBinnedSamples();
					if(obsIndices->useBin()){
						featureHolder.resize(obsIndices->returnBinnedSize());
					}else{
						featureHolder.resize(obsIndices->returnInSampleSize());
					}
				}

				inline float calculateNodeImpurity(){
					return obsIndices->returnImpurity();
				}

				inline void loadIndices(stratifiedInNodeClassIndicesUnsupervised* indices){
					obsIndices = indices;
				}


		}; //unprocessedNode.h
}//namespace fp
#endif //baseUnprocessedNodeUnsupervised_h
