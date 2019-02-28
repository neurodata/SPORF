#include <iostream>
#include <Rcpp.h> 
#include "forestPackingRConversion.h"

using namespace Rcpp;


RCPP_MODULE(forestPackingRConversion_mod){
	class_<forestPackingRConversion<double> >("forestPackingRConversion")

		.constructor()

		.method("setParameterString", &forestPackingRConversion<double>::setParameterString)
		.method("setParameterInt", &forestPackingRConversion<double>::setParameterInt)
		.method("setParameterDouble", &forestPackingRConversion<double>::setParameterDouble)
		.method("growForestGivenX", &forestPackingRConversion<double>::growForestGivenX)
		.method("growForestCSV", &forestPackingRConversion<double>::growForestCSV)
		.method("printParameters", &forestPackingRConversion<double>::printParameters)
		.method("printForestType", &forestPackingRConversion<double>::printForestType)
		.method("testAccuracy", &forestPackingRConversion<double>::testAccuracy)
		.method("predict", &forestPackingRConversion<double>::predict)
		;
}

