#include <iostream>
#include <RcppArmadillo.h>
#include "packedForest/src/packedForest.h"

using namespace Rcpp;

using namespace fp;

RCPP_MODULE(fpForest){
	class_<fpForest<double> >("fpForest")

		.constructor()

		.method("setParamString", &fp::fpForest<double>::setParamString)
		.method("setParamDouble", &fp::fpForest<double>::setParamDouble)
		.method("setParamInt", &fp::fpForest<double>::setParamInt)
		.method("growForest", &fp::fpForest<double>::growForest)
		.method("printParameters", &fp::fpForest<double>::printParameters)
		.method("printForestType", &fp::fpForest<double>::printForestType)
		.method("testAccuracy", &fp::fpForest<double>::testAccuracy)
		.method("predict", &fp::fpForest<double>::predict)
		;

}
