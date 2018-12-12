#include <iostream>
#include <RcppArmadillo.h>
#include "packedForest/src/packedForest.h"

using namespace Rcpp;

using namespace fp;

RCPP_MODULE(fpForest){
	class_<fpForest<double> >("fpForest")

		.constructor()

		.method("setParameter1", &fp::fpForest<double>::setParameter1)
		.method("setParameter2", &fp::fpForest<double>::setParameter2)
		.method("setParameter3", &fp::fpForest<double>::setParameter3)
		.method("growForest", &fp::fpForest<double>::growForest)
		.method("printParameters", &fp::fpForest<double>::printParameters)
		.method("printForestType", &fp::fpForest<double>::printForestType)
		.method("testAccuracy", &fp::fpForest<double>::testAccuracy)
		.method("predict", &fp::fpForest<double>::predict)
		;

}
