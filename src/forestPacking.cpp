#include <RcppArmadillo.h>
#include <improv8.h>
#include <vector>
// [[Rcpp::depends(RcppArmadillo)]]

using namespace Rcpp;

// [[Rcpp::export]]
void packForestRCPP(){
	const std::string forestFileName = "forestPackTempFile.csv";
	const std::string traversalFileName = "traversalPackTempFile.csv";
	const std::string packedFileName = "forest.out";

	improv8 tester(forestFileName,1,traversalFileName,16, 3);
	tester.writeForest(packedFileName);
}


// [[Rcpp::export]]
Rcpp::NumericVector predictRF(const NumericMatrix mat, const int numCores){

		int numObservations = mat.nrow();
			int numFeatures = mat.ncol();

			std::vector<double> currObs(numFeatures);
			Rcpp::NumericVector predictions(numObservations);

			const std::string packedFileName = "forest.out";
			improv8 tester(packedFileName);
			for(int i = 0; i < numObservations; i++){
				for(int j = 0; j < numFeatures; j++){
					currObs[j] = mat(i,j);
				}
				predictions[i] = tester.makePrediction(currObs)+1;
			}
			return predictions;
}


