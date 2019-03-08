#ifndef fpDisplayProgress_h
#define fpDisplayProgress_h


#include <chrono>
#include <ctime>

namespace fp {

	//TODO these two classes can be merged into one.
	class fpDisplayProgressStaticStore{

		private:
			std::chrono::time_point<steady_clock> startTime;
			std::chrono::time_point<steady_clock> stopTime;
			std::chrono::seconds diffSeconds;

		public:
			fpDisplayProgressStaticStore(){
				startTime = std::chrono::steady_clock::now();
				std::cout << "starting tree 1" << std::flush;
			}

			inline void print(int i){
			std::chrono::seconds updateTime(10);
				stopTime = std::chrono::steady_clock::now();
				diffSeconds =	std::chrono::duration_cast<std::chrono::seconds>(stopTime - startTime);
				if(diffSeconds > updateTime){
					std::cout << "..." << i << std::flush;
					startTime = std::chrono::steady_clock::now();
				}
			}
	};


	class fpDisplayProgress{
		public:
			fpDisplayProgressStaticStore staticPrint;
		inline void displayProgress(int treeNum) { 
			staticPrint.print(treeNum); } 
	};

} //namespace fp
#endif //fpDisplayProgress.h
