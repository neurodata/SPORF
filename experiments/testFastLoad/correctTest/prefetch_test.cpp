#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>

const int SIZE=1024*1024*1;
const int STEP_CNT=1024*1024*10;

unsigned int next(unsigned int current){
	return (current*10001+328)%SIZE;
}


template<int algorithm>
struct Worker{
	std::vector<int> mem;

	double result;
	int oracle_offset;

	void operator()(){
		unsigned int prefetch_index=0;
		for(int i=0;i<oracle_offset;i++)
			prefetch_index=next(prefetch_index);

		unsigned int index=0;
		for(int i=0;i<STEP_CNT;i++){
			//prefetch memory block used in a future iteration
			if(prefetch){
				__builtin_prefetch(mem.data()+prefetch_index,0,1);    
			}
			//actual work:
			result+=mem[index];

			//prepare next iteration
			prefetch_index=next(prefetch_index);
			index=next(mem[index]);
		}
	}

	Worker(std::vector<int> &mem_):
		mem(mem_), result(0.0), oracle_offset(0)
	{}
};

template <typename Worker>
double timeit(Worker &worker){
	auto begin = std::chrono::high_resolution_clock::now();
	worker();
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count()/1e9;
}


int main() {
	//set up the data in special way!
	std::vector<int> keys(SIZE);
	for (int i=0;i<SIZE;i++){
		keys[i] = i;
	}

	Worker<false> without_prefetch(keys);
	Worker<true> with_prefetch(keys);

	std::cout<<"#preloops\ttime no prefetch\ttime prefetch\tfactor\n";
	std::cout<<std::setprecision(17);

	for(int i=0;i<20;i++){
		//let oracle see i steps in the future:
		without_prefetch.oracle_offset=i;
		with_prefetch.oracle_offset=i;

		//calculate:
		double time_with_prefetch=timeit(with_prefetch);
		double time_no_prefetch=timeit(without_prefetch);

		std::cout<<i<<"\t"
			<<time_no_prefetch<<"\t"
			<<time_with_prefetch<<"\t"
			<<(time_no_prefetch/time_with_prefetch)<<"\n";
	}

}
