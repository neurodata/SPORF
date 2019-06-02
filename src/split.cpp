# include <RcppArmadillo.h>
// [[Rcpp::depends(RcppArmadillo)]]

using namespace Rcpp;

// [[Rcpp::export]]
List findSplit(const NumericVector x, const IntegerVector y, const int & ndSize, const double & I,
	double maxdI, IntegerVector bv, NumericVector bs, int nb, int mb, const int nzidx, arma::vec cc) {

	double xl, xr, dI;
	int yl, yr, cons, potsplit, bsct;
	bool multiy;
	IntegerVector bsidx(ndSize);

	arma::vec ccl(cc.n_elem, arma::fill::zeros);
	arma::vec ccr = cc;
	arma::vec cpl, cpr, potccl, potccr;

	bsct = 0;
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
					    bsidx[0] = potsplit;
							bv[0] = nzidx;
							nb = 1;
							maxdI = dI;
							bsct = 1;
						} else if (dI == maxdI && dI > 0 && nb < mb) {
							bsidx[bsct] = potsplit;
							bv[nb] = nzidx;
							bsct += 1;
							nb += 1;
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
							bsidx[0] = potsplit;
							bv[0] = nzidx;
							nb = 1;
							maxdI = dI;
							bsct = 1;
						} else if (dI == maxdI && dI > 0 && nb < mb) {
							bsidx[bsct] = potsplit;
							bv[nb] = nzidx;
							bsct += 1;
							nb += 1;
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
						bsidx[0] = i + 1;
						bv[0] = nzidx;
						nb = 1;
						maxdI = dI;
						bsct = 1;
			    } else if (dI == maxdI && dI > 0 && nb < mb) {
						bsidx[bsct] = i + 1;
						bv[nb] = nzidx;
						bsct += 1;
						nb += 1;
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
					bsidx[0] = i + 1;
					bv[0] = nzidx;
					nb = 1;
					maxdI = dI;
					bsct = 1;
				} else if (dI == maxdI && dI > 0 && nb < mb) {
					bsidx[bsct] = i + 1;
					bv[nb] = nzidx;
					bsct += 1;
					nb += 1;
				}
				yl = yr;
	    }
	    multiy = false;
	    xl = xr;
		}
	}

	if (bsct != 0) {
		for (int i = 0; i < bsct; ++i) {
			bs[nb-bsct+i] = (x[bsidx[i]-1] + x[bsidx[i]])/2;
		}
	}
	return List::create(_["MaxDeltaI"] = maxdI, _["BestVar"] = bv, _["BestSplit"] = bs, _["NumBest"] = nb);
}
