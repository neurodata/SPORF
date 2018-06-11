#ifndef fpSingleton_h
#define fpSingleton_h

#include "fpData.h"
#include "fpInfo.h"
#include <string>
//#include <memory>

namespace fp {

	class fpSingleton{

		private:

			static fpSingleton* infoSetting;
			fpInfo fpForestInfo;
			fpData data;

		public:
			static fpSingleton& getSingleton(){
				static fpSingleton* infoSetting(new fpSingleton);
				return *infoSetting;
			}

			inline void setParameter(const std::string& parameterName, const std::string& parameterValue){
				fpForestInfo.setParameter(parameterName, parameterValue);
			}

			inline void setParameter(const std::string& parameterName, const double parameterValue){
				fpForestInfo.setParameter(parameterName, parameterValue);
			}

			inline void setParameter(const std::string& parameterName, const int parameterValue){
				fpForestInfo.setParameter(parameterName, parameterValue);
			}

			inline void printAllParameters(){
				fpForestInfo.printAllParameters();
			}

			inline void printForestType(){
				fpForestInfo.printForestType();
			}

			inline void loadData(){
				data.fpLoadData(fpForestInfo);
			}

			inline void setNumFeatures(int numF){
				fpForestInfo.setNumFeatures(numF);
			}

			inline void setNumObservations(int numO){
				fpForestInfo.setNumObservations(numO);
			}

			inline void setNumClasses(int numC){
				fpForestInfo.setNumClasses(numC);
			}

			inline int returnNumClasses(){
				return fpForestInfo.returnNumClasses();
			}

			inline std::string& returnForestType(){
return fpForestInfo.returnForestType();
}

			inline int returnNumFeatures(){
				return fpForestInfo.returnNumFeatures();
			}

			inline int returnNumObservations(){
				return fpForestInfo.returnNumObservations();
			}

			inline int returnLabel(int observationNumber){
				return data.returnLabel(observationNumber);
			}

			inline int returnFeatureVal(const int featureNumber, const int observationNumber){
				return data.returnFeatureVal(featureNumber, observationNumber);
			}

			inline bool loadDataFromCSV(){
				return data.loadDataFromCSV(fpForestInfo);
			}

		private: //These are singleton specific methods
			fpSingleton(){}
			~fpSingleton();
			fpSingleton(const fpSingleton &old){
				if(this != &old){
					infoSetting= old.infoSetting;
				}
			}
			const fpSingleton &operator=(const fpSingleton &old){
				if(this !=&old){
					infoSetting=old.infoSetting;
				}
				return *this;
			}

	}; // class fpSingleton
	fpSingleton * fpSingleton::infoSetting = nullptr;

} //namespace fp
#endif //fpSingleton.h
