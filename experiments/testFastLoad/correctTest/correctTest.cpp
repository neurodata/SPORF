#include "../src/zipClassAndValue.h"
#include <vector>
#include <list>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <stdint.h>
#include <assert.h>


//////////Fast random number generator Start
#define znew (z=36969*(z&65535)+(z>>16))
#define wnew (w=18000*(w&65535)+(w>>16))
#define MWC ((znew<<16)+wnew )
#define SHR3 (jsr^=(jsr<<17), jsr^=(jsr>>13), jsr^=(jsr<<5))
#define CONG (jcong=69069*jcong+1234567)
#define FIB ((b=a+b),(a=b-a))
#define KISS ((MWC^CONG)+SHR3)
#define LFIB4 (c++,t[c]=t[c]+t[UC(c+58)]+t[UC(c+119)]+t[UC(c+178)])
#define SWB (c++,bro=(x<y),t[c]=(x=t[UC(c+34)])-(y=t[UC(c+19)]+bro))
#define UNI (KISS*2.328306e-10)
#define VNI ((long) KISS)*4.656613e-10
#define UC (unsigned char) /*a cast operation*/
typedef unsigned long UL;
/* Global static variables: */
static UL z=362436069, w=521288629, jsr=123456789, jcong=380116160;
static UL a=224466889, b=7584631, t[256];
/* Use random seeds to reset z,w,jsr,jcong,a,b, and the table
 * t[256]*/
static UL x=0,y=0,bro; static unsigned char c=0;
/* Example procedure to set the table, using KISS: */
void settable(UL i1,UL i2,UL i3,UL i4,UL i5, UL i6)
{ int i; z=i1;w=i2,jsr=i3; jcong=i4; a=i5; b=i6;
	for(i=0;i<256;i=i+1) t[i]=KISS;
}
//////////Fast random number generator Stop

class iterBeginEnd{
	private:
		int start;
		int end;

	public:
		inline void setStart(int startSetter){
			start = startSetter;
		}

		inline void setEnd(int endSetter){
			end = endSetter;
		}

		inline void setBoth(int startSetter, int endSetter){
			start = startSetter;
			end = endSetter;
		}

		inline int returnStart(){
			return start;
		}

		inline void incrementStart(){
			++start;
		}

		inline bool returnAtEnd(){
			return start==end;
		}
};


class roundRobinLoad
{
	protected:
		std::list<iterBeginEnd> iteratorList;

	public:
		roundRobinLoad(): iteratorList(15){}

		inline void createIterators(int indexVecSize){
			int binSize = indexVecSize/15;
			int binStart = 0;
			int binEnd = binSize+binStart;
			auto iter = iteratorList.begin();
			for(; iter != iteratorList.end(); ++iter){
				iter->setBoth(binStart, binEnd);
				binStart = binEnd;
				binEnd = binStart +binSize;
			}
			iteratorList.emplace_back();
			iteratorList.back().setBoth(binStart, indexVecSize);
		}

		inline std::list<iterBeginEnd>& returnList(){
			return iteratorList;
		}

		inline bool isEmpty(){
			return iteratorList.empty();
		}

		inline std::list<iterBeginEnd>::iterator returnFront(){
			return iteratorList.begin();
		}

		inline std::list<iterBeginEnd>::iterator returnEnd(){
			return iteratorList.end();
		}

		inline std::list<iterBeginEnd>::iterator eraseIt(std::list<iterBeginEnd>::iterator dIt){
			return iteratorList.erase(dIt);
		}
};


int main(int argc, char* argv[]) {

	int testSize = 1000;

	std::vector<std::vector<fp::zipClassAndValue<int, UL> > > zipVec(4);
	for(auto &i : zipVec){
		i.resize(testSize);
	}


	std::vector<UL>  randomValVec;
	std::vector<int> randomOrderVec;

	randomValVec.resize(testSize);
	randomOrderVec.resize(testSize);
	for(int i = 0;i < testSize;++i){
		randomValVec[i]=(UL)i;
		randomOrderVec[i]=(int)(MWC%testSize);
	}

	//////////////////////////////
	//Test baseline
	//////////////////////////////
	for(int i = 0; i < testSize; ++i){
		zipVec[0][i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
		//	zipVec[i].setPair(i, randomValVec[featureToUse][randomOrderVec[i]]);
	}
	std::sort(zipVec[0].begin(), zipVec[0].end());

	//////////////////////////////
	//Test prefetch all
	//////////////////////////////
	for(int i = 0; i < testSize; ++i){
		__builtin_prefetch (&randomValVec[randomOrderVec[i]], 0, 0);
	}
	for(int i = 0; i < testSize; ++i){
		zipVec[1][i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
	}
	std::sort(zipVec[1].begin(), zipVec[1].end());

	//////////////////////////////
	//Test class bin
	//////////////////////////////

	roundRobinLoad rrIters;
	int loadPosition = 0;

	rrIters.createIterators((int)randomOrderVec.size());
	for(auto i : rrIters.returnList()){
		__builtin_prefetch (&randomValVec[randomOrderVec[i.returnStart()]], 0, 0);
	}

	while(!rrIters.isEmpty()){
		for(auto iter = rrIters.returnFront(); iter != rrIters.returnEnd();){
			if(iter->returnAtEnd()){
				iter = rrIters.eraseIt(iter);
				continue;
			}
			__builtin_prefetch (&randomValVec[randomOrderVec[iter->returnStart()+1]], 0, 3);
			zipVec[2][loadPosition].setPair(randomOrderVec[iter->returnStart()], randomValVec[randomOrderVec[iter->returnStart()]]);
			++loadPosition;
			iter->incrementStart();
			++iter;
		}
	}

	std::sort(zipVec[2].begin(), zipVec[2].end());


	//////////////////////////////
	//Test simple bin
	//////////////////////////////

	int numBins = 32;
	std::vector<int> position(numBins);
	int binSize = (int)randomOrderVec.size() / numBins;
	int currPos = 0;

	for(auto &i : position){
		i = currPos;
		__builtin_prefetch (&randomValVec[randomOrderVec[i]], 0, 3);
		currPos += binSize;
	}

	for(int j=0; j < binSize;++j){
		for(auto& i : position){
			__builtin_prefetch (&randomValVec[randomOrderVec[i+1]], 0, 3);
			zipVec[3][i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
			++i;
		}
	}
	for(int i = currPos; i < (int)randomOrderVec.size(); ++i){
		zipVec[3][i].setPair(randomOrderVec[i], randomValVec[randomOrderVec[i]]);
	}

	std::sort(zipVec[3].begin(), zipVec[3].end());


	//////////////////////////
	//Correctness Test
	//////////////////////////

for(int i = 0; i < testSize; ++i){
	for(int j = 1; j < (int)zipVec.size(); ++j){
	assert(zipVec[0][i].returnObsClass() == zipVec[j][i].returnObsClass());
	assert(zipVec[0][i].returnFeatureVal() == zipVec[j][i].returnFeatureVal());
	}
}


}
