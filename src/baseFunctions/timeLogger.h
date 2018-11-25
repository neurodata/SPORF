#ifndef fptimeLogger_h
#define fptimeLogger_h

#include <chrono>
#include <iostream>

namespace fp{

	using namespace std::chrono;
	class timeLogger
	{
		//	protected:
		public:
			steady_clock::time_point startGrow;
			static microseconds totalGrowTime;

			steady_clock::time_point startSort;
			static microseconds totalSortTime;

			steady_clock::time_point startGini;
			static microseconds totalGiniTime;

			steady_clock::time_point startFindSplit;
			static microseconds totalFindSplitTime;

		public:
			inline void startGrowTimer(){
				startGrow = steady_clock::now();
			}

			inline void stopGrowTimer(){
				totalGrowTime = totalGrowTime +std::chrono::duration_cast<std::chrono::microseconds>(steady_clock::now()-startGrow);
			}

			inline void startGiniTimer(){
				startGini = steady_clock::now();
			}

			inline void stopGiniTimer(){
				totalGiniTime = totalGiniTime +std::chrono::duration_cast<std::chrono::microseconds>(steady_clock::now()-startGini);
			}

			inline void startSortTimer(){
				startSort = steady_clock::now();
			}

			inline void stopSortTimer(){
				totalSortTime = totalSortTime +std::chrono::duration_cast<std::chrono::microseconds>(steady_clock::now()-startSort);
			}

			inline void startFindSplitTimer(){
				startFindSplit = steady_clock::now();
			}

			inline void stopFindSplitTimer(){
				totalFindSplitTime = totalFindSplitTime +std::chrono::duration_cast<std::chrono::microseconds>(steady_clock::now()-startFindSplit);
			}

			inline void printGrowTime(){
				std::cout << std::fixed << "it took " << totalGrowTime.count() << " us to grow\n";
				std::cout << std::fixed << "it took " << totalSortTime.count() << " us to sort\n";
				std::cout << std::fixed << "it took " << totalGiniTime.count() << " us to gini\n";
				std::cout << std::fixed << "it took " << totalFindSplitTime.count() << " us to find split\n";
			}
	};//time logger

	microseconds timeLogger::totalGrowTime=std::chrono::duration_values<microseconds>::zero();
	microseconds timeLogger::totalSortTime=std::chrono::duration_values<microseconds>::zero();
	microseconds timeLogger::totalGiniTime=std::chrono::duration_values<microseconds>::zero();
	microseconds timeLogger::totalFindSplitTime=std::chrono::duration_values<microseconds>::zero();

}//namespace fp
#endif //fpSplit_h
