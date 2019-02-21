#include <string>
#include <vector>
#include <algorithm>
#include "../../src/baseFunctions/stratifiedInNodeClassIndices.h"
#include "../../src/baseFunctions/fpUtils.h"

using namespace fp;

TEST(checkStratified, checkRootNodeIndexCreation )
{
			int numOb = 1000;
	stratifiedInNodeClassIndices test(numOb);

	
	EXPECT_EQ(numOb, test.returnInSampleSize());
	EXPECT_GT(numOb, test.returnOutSampleSize());

	//order vectors to check existence easier
	std::vector<int> inSamps = test.returnInSamples();
	std::vector<int> outSamps = test.returnOutSamples();
	std::sort(inSamps.begin(),inSamps.end());
	std::sort(outSamps.begin(),outSamps.end());

	bool allNumbersExist;
	int numsTested = 0;
	for(int m=0; m < test.returnInSampleSize(); m++){
		++numsTested;
		allNumbersExist = false;
		for(int n=0; n < test.returnOutSampleSize(); n++){
			//check out samples
			if(m == outSamps[n]){
				allNumbersExist = true;
				break;
			}
			if(m < outSamps[n]){
				break;
			}
		}

		if(allNumbersExist){
			continue;
		}

			//check in samples
		for(int n=0; n < test.returnInSampleSize(); n++){
			if(m ==inSamps[n]){
				allNumbersExist = true;
				break;
			}
			if(m < inSamps[n]){
				break;
			}
		}

		if(!allNumbersExist){
			break;
		}
	}

	EXPECT_TRUE(allNumbersExist);
	EXPECT_EQ(numOb, numsTested);
}
