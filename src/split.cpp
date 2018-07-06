# include <RcppArmadillo.h>

using namespace Rcpp;

// [[Rcpp::export]]
List findSplit(const NumericVector x, const IntegerVector y, const int & ndSize, const double & I,
		double maxdI, int bv, double bs, const int nzidx, arma::vec cc) {
	double xl, xr, dI;
	int yl, yr, cons, bsidx, potsplit;
	bool multiy;

	arma::vec ccl(cc.n_elem, arma::fill::zeros);
	arma::vec ccr = cc;
	arma::vec cpl, cpr, potccl, potccr;

	bsidx = 0;
	cons = 0;
	xl = x[0];
	yl = y[0] - 1;
	potsplit = 0;
	multiy = false;

	// iterate over split locations from left to right
	for (int i = 0; i < ndSize - 1; ++i) {
		xr = x[i+1];
		yr = y[i+1] - 1;
		if (xl == xr) {
			cons += 1;
			if (yl == yr) {
				continue;
			} else {
				if (~multiy) {
					multiy = true;
					if (potsplit != 0) {
						cpl = potccl/potsplit;
						cpr = potccr/(ndSize - potsplit);
						dI = I - dot(ccl,(1 - cpl)) - dot(ccr,(1 - cpr));
						if (dI > maxdI) {
							// save current best split information
							maxdI = dI;
							bsidx = potsplit;
							bv = nzidx;
						}
						potsplit = 0;
					}
				}
			}
			ccl[yl] += cons;
			ccr[yl] -= cons;
			cons = 0;
			yl = yr;
		} else if ((xl + xr)/2 == xr) {
			cons += 1;
			if (yl == yr) {
				continue;
			} else {
				if (~multiy) {
					multiy = true;
					if (potsplit != 0) {
						cpl = potccl/potsplit;
						cpr = potccr/(ndSize - potsplit);
						dI = I - dot(ccl,(1 - cpl)) - dot(ccr,(1 - cpr));
						if (dI > maxdI) {
							// save current best split information
							maxdI = dI;
							bsidx = potsplit;
							bv = nzidx;
						}
						potsplit = 0;
					}
				}
			}
			ccl[yl] += cons;
			ccr[yl] -= cons;
			cons = 0;
			xl = xr;
			yl = yr;
		} else {
			cons += 1;
			ccl[yl] += cons;
			ccr[yl] -= cons;
			cons = 0;
			if (yl == yr) {
				if (multiy) {
					cpl = ccl/(i + 1);
					cpr = ccr/(ndSize - (i + 1));
					dI = I - dot(ccl,(1 - cpl)) - dot(ccr,(1 - cpr));
					if (dI > maxdI) {
						// save current best split information
						maxdI = dI;
						bsidx = i + 1;
						bv = nzidx;
					}
				} else {
					potsplit = i + 1;
					potccl = ccl;
					potccr = ccr;
				}
			} else {
				cpl = ccl/(i + 1);
				cpr = ccr/(ndSize - (i + 1));
				dI = I - dot(ccl,(1 - cpl)) - dot(ccr,(1 - cpr));
				if (dI > maxdI) {
					// save current best split information
					maxdI = dI;
					bsidx = i + 1;
					bv = nzidx;
				}
				yl = yr;
			}
			multiy = false;
			xl = xr;
		}
	}

	if (bsidx != 0) {
		bs = (x[bsidx - 1] + x[bsidx])/2;
	}
	return List::create(_["MaxDeltaI"] = maxdI, _["BestVar"] = bv, _["BestSplit"] = bs);
}
