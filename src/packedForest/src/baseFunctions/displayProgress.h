#ifndef fpDisplayProgress_h
#define fpDisplayProgress_h


#include <chrono>
#include <ctime>

namespace fp {

	class fpDisplayProgressStaticStore{

		private:
			std::chrono::time_point<system_clock> startTime;
			std::chrono::time_point<system_clock> stopTime;
			std::chrono::seconds diffSeconds;

		public:
			fpDisplayProgressStaticStore(){
				startTime = std::chrono::system_clock::now();
				std::cout << "starting tree 1" << std::flush;
			}

			inline void print(int i){
			std::chrono::seconds updateTime(10);
				stopTime = std::chrono::high_resolution_clock::now();
				diffSeconds =	std::chrono::duration_cast<std::chrono::seconds>(stopTime - startTime);
				if(diffSeconds > updateTime){
					std::cout << "..." << i << std::flush;
					startTime = std::chrono::high_resolution_clock::now();
				}
			}
	};


	class fpDisplayProgress{
		public:
		inline void displayProgress(int treeNum) { 
			static fpDisplayProgressStaticStore staticPrint;
			staticPrint.print(treeNum); } 
	};

} //namespace fp
#endif //fpDisplayProgress.h
