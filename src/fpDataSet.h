#ifndef fpDataSet_h
#define fpDataSet_h
#include <vector>
#include "fpReadCSV.h"
#include <string>

template <typename T>
class inputYDataClassification
{
	private:
		std::vector < T > YData;

	public:
		void initializeYData(int numObs){
			YData.resize(numObs);
		}

		inline void setYElement(int elementNumber, int value){
			YData[elementNumber] = value;
		}

		inline T returnYElement(int elementNumber){
			return YData[elementNumber];
		}

		inline int returnNumObservations(){
			return YData.size();
		}

		//bool
};

template <typename T>
class inputXData
{
	private:
		std::vector < std::vector<T> > XData;

	public:
		void initializeXData( int numFeatures, int numObservations){
			XData.resize(numFeatures);
			for (int i=0; i<numFeatures; i++){
				XData[i].resize(numObservations);
			}
		}

		inline T returnElement( int feature, int observation){
			return XData[feature][observation];
		}

		inline void setXElement( int feature, int observation, T value){
			XData[feature][observation] = value;
		}

		inline int returnNumFeatures(){
			return XData.size();
		}

		inline int returnNumObservations(){
			return XData[0].size();
		}
};

template <typename T, typename Q>
class inputData
{
	private:
		inputXData<T> X;
		inputYDataClassification<Q> Y;

	public:
		inputData(const std::string& forestCSVFileName, int columnWithY)
		{

//			csvHandle<T> csvH(forestCSVFileName);
			csvHandle csvH(forestCSVFileName);
			if(columnWithY >= csvH.returnNumColumns()){
				throw std::runtime_error("column with class labels does not exist." );
				return;
			}

			X.initializeXData(csvH.returnNumColumns()-1, csvH.returnNumRows());
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

		inline Q returnClassOfObservation(int observationNum){
			return Y.returnYElement(observationNum);
		}

		inline T returnFeatureValue(int featureNum, int observationNum){
			return X.returnElement(featureNum, observationNum);
		}

		inline int returnNumFeatures(){
			return X.returnNumFeatures();
		}

		inline int returnNumObservations(){
			return Y.returnNumObservations();
		}

		void printDataStats(){
			std::cout << "there are " << this.returnNumFeatures() << " features.\n";
			std::cout << "there are " << this.returnNumObservations() << " observations.\n";
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








template <typename T>	
class rankedElement
{
	private:
		int rankInFeature;
		T featureValue;

	public:
		inline void setElementValues(int rankValue, T featureVal)
		{
			rankInFeature = rankValue;
			featureValue = featureVal;
		}

		inline int returnRank()
		{
			return rankInFeature;
		}

		inline void setFeatureValue(T newFeatureValue)
		{
			featureValue = newFeatureValue;
		}

		inline T returnFeatureValue()
		{
			return featureValue;
		}
};




template <typename T>	
class rankedInput
{
	private:
		std::vector< std::vector<rankedElement<T> > > rankedInputData;
		int numFeatures;
		int numObservations;

	public:
		rankedInput( csvHandle& csvH){

			numFeatures = csvH.returnNumColumns();
			numObservations = csvH.returnNumRows();
			rankedInputData.resize(numFeatures);
			for(int i = 0; i < numFeatures; i++){
				rankedInputData[i].resize(numObservations);
			}

			for(int j = 0; j < numObservations; j++){
				for(int i = 0; i < numFeatures; i++)
				{
					rankedInputData[i][j].setFeatureValue(csvH.returnNextElement<T>());	
				}
			}
		}

		rankedInput( const std::string& forestCSVFileName){

			csvHandle csvH(forestCSVFileName);
			numFeatures = csvH.returnNumColumns();
			numObservations = csvH.returnNumRows();
			rankedInputData.resize(numFeatures);
			for(int i = 0; i < numFeatures; i++){
				rankedInputData[i].resize(numObservations);
			}

			for(int j = 0; j < numObservations; j++){
				for(int i = 0; i < numFeatures; i++)
				{
					rankedInputData[i][j].setFeatureValue(csvH.returnNextElement<T>());	
				}
			}
		}

		void printCSVStats(){
			std::cout << "there are " << numFeatures << " features.\n";
			std::cout << "there are " << numObservations << " observations.\n";
		}

		inline int returnNumFeatures(){return numFeatures;}
		inline int returnNumObservations(){return numObservations;}

		void printTableOfValues(){
			for(int i = 0; i < numFeatures; i++){
				for(int j = 0; j < numObservations; j++){
					std::cout << rankedInputData[i][j].returnFeatureValue() << " ";
				}
				std::cout << "\n";
			}
		}

		inline T returnFeatureValue(int featureNum, int ObservationNum){
			return rankedInputData[featureNum][ObservationNum].returnFeatureValue(); 
		}
};

#endif //fpDataSet_h
