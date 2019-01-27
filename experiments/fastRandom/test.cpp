#include "aesctr.h"
#include <iostream>
#include <exception>
#include <chrono>
#include "MWC.h"

int main(int argc, char* argv[]) {

	struct timespec start, finish;
	double elapsed;


	aes::AesCtr<uint64_t, 2> rng_(4);
	clock_gettime(CLOCK_MONOTONIC, &start);

	std::cout <<  "starting aesctr\n ";

	for(int i = 0; i < 1000000; ++i){
		rng_()%20;
	}
	std::cout << "\n";

	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	std::cout << "It ran in " << elapsed << "s, " << rng_()%20 << "\n";



	

	std::cout <<  "starting mwc2\n ";

	randomNumberRerFMWC ranNum;
	ranNum.initialize(3);
	clock_gettime(CLOCK_MONOTONIC, &start);

	for(int i = 0; i < 1000000; ++i){
		ranNum.gen(20);
	}
	std::cout << "\n";

	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	std::cout << "It ran in " << elapsed << "s, " << ranNum.gen(20) << "\n";


for(int i = 0; i < 100; ++i){
	std::cout << ranNum.gen(20) << ", ";
	}
	std::cout << "\n";


}
