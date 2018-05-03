#include "fpUtil.h"
#include <iostream>

int main(int argc, char* argv[]) {
	const std::string csvFileName = "res/testCSV.csv";
	csvHandle testHandle(csvFileName);
	testHandle.printCSVStats();
	std::cout << testHandle.returnNumColumns() << "\n";
}
