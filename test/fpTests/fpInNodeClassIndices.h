#include <string>
#include "../../src/baseFunctions/inNodeClassIndices.h"

using namespace fp;


TEST(checkInNodeClassIndices, checkRootNodeIndexCreation )
{
	int numOb = 1000;
	inNodeClassIndices test(numOb);

	EXPECT_EQ(numOb, test.returnInSampleSize());
	EXPECT_GT(numOb, test.returnOutSampleSize());

	bool allNumbersExist;
	int numsTested = 0;
	for(int m=0; m < test.returnInSampleSize(); m++){
		++numsTested;
		allNumbersExist = false;
		for(int n=0; n < test.returnOutSampleSize(); n++){
			//check out samples
			if(m == test.returnOutSample(n)){
				allNumbersExist = true;
				break;
			}
			if(m < test.returnOutSample(n)){
				break;
			}
		}

		if(allNumbersExist){
			continue;
		}

			//check in samples
		for(int n=0; n < test.returnInSampleSize(); n++){
			if(m == test.returnInSample(n)){
				allNumbersExist = true;
				break;
			}
			if(m < test.returnInSample(n)){
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
