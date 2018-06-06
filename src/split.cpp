# include <RcppArmadillo.h>

using namespace Rcpp;

/**
 * Computes the split of a node. Given a vectorized version of the node assigned data values,
 * iterate through each split location to find the split that optimizes the information gain.
 * Information gain will be gini impurity for classification and split variance for regression.
 *
 * @param x         the projected data vector to split
 * @param y         the response vector
 * @param ndSize    number of data points at the current node
 * @param I         node sample objective value
 * @param maxdI     current best split information gain
 * @param bv        current best split projection variable
 * @param bs        current best split split boundary
 * @param nzidx     the projection variable being considered
 * @param cc        class counts in classification
 * @param task      the task being performed. 1 for regression, 0 for classification
 *
 *
 */

List findSplitClassification(const NumericVector x, const IntegerVector y, const int & ndSize, const double & I,
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

double mean(NumericVector x) {
    int n = x.size();
    double total = 0;

    for(int i = 0; i < n; ++i) {
        total += x[i];
    }
    return total / n;
}

double mse(NumericVector y) {
    double y_mean = mean(y);
    return sum(pow((y_mean - y),2));
}


List findSplitRegression(const NumericVector x, const NumericVector y, const int & ndSize, const double & I,
	       double maxdI, int bv, double bs, const int nzidx) {
    double dI;
    if (x[0] != x[ndSize - 1]) {
        for (int i = 0; i < ndSize - 2; ++i) {
            dI = I - sum(NumericVector::create(mse(y[Range(0,i)]), mse(y[Range(i+1,ndSize-1)])));
            if (dI > maxdI) {
                maxdI = dI;
                bv = nzidx;
                bs = (x[i] + x[i+1]) / 2;
            }
        }
    }

	return List::create(_["MaxDeltaI"] = maxdI, _["BestVar"] = bv, _["BestSplit"] = bs);
}


// [[Rcpp::export]]
List findSplit(const NumericVector x, const NumericVector y, const int & ndSize, const double & I,
	       double maxdI, int bv, double bs, const int nzidx, arma::vec cc, const int & task) {

    List ret;
    if (task == 0) {
        const IntegerVector int_y = as<IntegerVector>(y);
        ret = findSplitClassification(x, int_y, ndSize, I, maxdI, bv, bs, nzidx, cc);
    } else {
        ret = findSplitRegression(x, y, ndSize, I, maxdI, bv, bs, nzidx);
    }
    return ret;
}
