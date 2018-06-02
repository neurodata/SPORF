#ifndef fpData_h
#define fpData_h

//#include "fpUtils.h"
#include <string>
//#include <memory>

namespace fp {

	class fpData{

		protected:
			// data input
			std::string forestCSVFileName;
			int columnWithY;
			inputData<double, int>* inData;

		public:

			fpData():columnWithY(-1){}
			

void setParameter(const std::string& parameterName, const std::string& parameterValue){
		if(parameterName == "CSVFileName"){
			forestCSVFileName = parameterValue;
		}else{
			throw std::runtime_error("Unknown parameter type.(string)");
		}
	}


	void setParameter(const std::string& parameterName, const double parameterValue){
if(parameterName == "columnWithY"){
			columnWithY = (int)parameterValue;
		}else {
			throw std::runtime_error("Unknown parameter type.(double)");
		}
	}


	void setParameter(const std::string& parameterName, const int parameterValue){
		if(parameterName == "columnWithY"){
			columnWithY = parameterValue;
		}else {
			throw std::runtime_error("Unknown parameter type.(int)");
		}
	}

		//	void setDataRelatedParameters(inputData<double, int>* data);
void printAllParameters(){
		std::cout << "CSV file name -> " <<  forestCSVFileName << "\n";
		std::cout << "columnWithY -> " << columnWithY << "\n";
	}

void fpLoadData(){
		if(!forestCSVFileName.empty() || columnWithY != -1){
			inData = new inputData<double,int>(forestCSVFileName, columnWithY);
		}else {
			throw std::runtime_error("Unable to read data." );
		}
//		setDataRelatedParameters(fpData);
	}

int returnNumClasses(){
return inData->returnNumClasses();
}

int returnNumFeatures(){
return inData->returnNumFeatures();
}

	}; // class fpData
} //namespace fp
#endif //fpData.h
