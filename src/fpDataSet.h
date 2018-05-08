#ifndef fpDataSet_h
#define fpDataSet_h
#include <vector>
#include "fpReadCSV.h"
#include <string>

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
		rankedInput( csvHandle<T>& csvH){

			numFeatures = csvH.returnNumColumns();
			numObservations = csvH.returnNumRows();
			rankedInputData.resize(numFeatures);
			for(int i = 0; i < numFeatures; i++){
				rankedInputData[i].resize(numObservations);
			}

			for(int j = 0; j < numObservations; j++){
				for(int i = 0; i < numFeatures; i++)
				{
					rankedInputData[i][j].setFeatureValue(csvH.returnNextElement());	
				}
			}
		}

		rankedInput( const std::string& forestCSVFileName){

			csvHandle<T> csvH(forestCSVFileName);
			numFeatures = csvH.returnNumColumns();
			numObservations = csvH.returnNumRows();
			rankedInputData.resize(numFeatures);
			for(int i = 0; i < numFeatures; i++){
				rankedInputData[i].resize(numObservations);
			}

			for(int j = 0; j < numObservations; j++){
				for(int i = 0; i < numFeatures; i++)
				{
					rankedInputData[i][j].setFeatureValue(csvH.returnNextElement());	
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
