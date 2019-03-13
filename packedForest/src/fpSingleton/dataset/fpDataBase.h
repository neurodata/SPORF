#ifndef fpDataBase_h
#define fpDataBase_h
#include "fpReadCSV.h"
#include "../../baseFunctions/buildSpecific.h"
#include <vector>
#include <string>


namespace fp {

	template <typename T>
		class inputYData
		{
			protected:
				std::vector < T > YData;

			public:

				virtual ~inputYData(){};

				void initializeYData(const int &numObs){
					YData.resize(numObs);
				}

				inline void setYElement(const int &elementNumber, const T &value){
					YData[elementNumber] = (int)value;
				}

				inline T returnYElement(const int &elementNumber){
					return YData[elementNumber];
				}

				inline int returnNumObservations(){
					return YData.size();
				}

		};


	template <typename T>
		class inputYDataClassification : public inputYData<T>
	{
		private:
			int maxClass;
			std::vector<short> classesUsed;

		public:
			inputYDataClassification(): maxClass(-1){}

			~inputYDataClassification(){};

			inline void setYElement(const int &elementNumber, const T &value){
				inputYData<int>::YData[elementNumber] = (int)value;
				if(value > maxClass){
					maxClass = value;
					classesUsed.resize(maxClass+1,0);
				}
				classesUsed[value]=1;
			}

			inline int numClasses(){
				return maxClass+1;
			}

			void checkClassRepresentation(){
				for(int i=0; i<maxClass; i++){
					if(classesUsed[i]==0){
						throw std::runtime_error("Not all classes represented in input." );
					}
				}
			}
	};


	template <typename T>
		class inputXData
		{
			private:
				std::vector < std::vector<T> > XData;

			public:
				virtual ~inputXData(){}
				void initializeXData( const int &numFeatures, const int &numObservations){
					XData.resize(numFeatures);
					for (int i=0; i<numFeatures; i++){
						XData[i].resize(numObservations);
					}
				}

				inline T returnElement(const int &feature, const int &observation){
					return XData[feature][observation];
				}

				inline void prefetchElement(const int &feature, const int &observation){
					PREFETCHGATHER(&XData[feature][observation]);
				}

				inline void setXElement( const int &feature, const int &observation, const T &value){
					XData[feature][observation] = value;
				}

				inline int returnNumFeatures(){
					return XData.size();
				}

				inline int returnNumObservations(){
					return XData[0].size();
				}
		};


	template <typename T>
		class testXData
		{
			private:
				std::vector < std::vector<T> > XData;

			public:
				void initializeTestXData( const int &numFeatures, const int &numObservations){
					XData.resize(numObservations);
					for (int i=0; i<numObservations; i++){
						XData[i].resize(numFeatures);
					}
				}

				inline T returnElement(const int &feature,const int &observation){
					return XData[observation][feature];
				}

				inline void prefetchElement(const int &feature,const int &observation){
					PREFETCHGATHER(XData[observation][feature]);
				}

				inline void setXElement( const int &feature, const int &observation, const T &value){
					XData[observation][feature] = value;
				}

				inline int returnNumFeatures(){
					return XData[0].size();
				}

				inline int returnNumObservations(){
					return XData.size();
				}
		};//testXData


	template <typename T, typename Q>
		class testData
		{
			private:
				testXData<T> X;
				inputYDataClassification<Q> Y;

			public:
				testData(const std::string& forestCSVFileName, const int &columnWithY)
				{

					//			csvHandle<T> csvH(forestCSVFileName);
					csvHandle csvH(forestCSVFileName);
					if(columnWithY >= csvH.returnNumColumns()){
						throw std::runtime_error("column with class labels does not exist." );
						return;
					}

					X.initializeTestXData(csvH.returnNumColumns()-1, csvH.returnNumRows());
					Y.initializeYData(csvH.returnNumRows());

					for(int i=0; i<csvH.returnNumRows(); i++){
						for(int j=0; j<csvH.returnNumColumns(); j++){
							if(j < columnWithY){
								X.setXElement(j,i, csvH.returnNextElement<T>());
							}else if(j == columnWithY){
								Y.setYElement(i, csvH.returnNextElement<Q>());
							}else{
								X.setXElement(j-1,i, csvH.returnNextElement<T>());
							}
						}
					}
				}

				inline Q returnClassOfObservation(const int &observationNum){
					return Y.returnYElement(observationNum);
				}

				inline T returnFeatureValue(const int &featureNum, const int &observationNum){
					return X.returnElement(featureNum, observationNum);
				}

				inline int returnNumFeatures(){
					return X.returnNumFeatures();
				}

				inline int returnNumObservations(){
					return Y.returnNumObservations();
				}

				inline int returnNumClasses(){
					return Y.numClasses();
				}

				void printDataStats(){
					std::cout << "there are " << this->returnNumFeatures() << " features.\n";
					std::cout << "there are " << this->returnNumObservations() << " observations.\n";
				}

				void printXValues(){
					for(int i = 0; i < this->returnNumFeatures(); i++){
						for(int j = 0; j < this->returnNumObservations(); j++){
							std::cout << this->returnFeatureValue(i,j) << " ";
						}
						std::cout << "\n";
					}
				}

				void printYValues(){
					for(int j = 0; j < this->returnNumObservations(); j++){
						std::cout << this->returnClassOfObservation(j) << " ";
					}
					std::cout << "\n";
				}
		};

} //namespace fp
#endif //fpDataBase_h
