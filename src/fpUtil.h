#ifndef fpUtil_h
#define fpUtil_h
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>

class csvHandle
{
	private:
		std::ifstream streamHandle;
		int numberOfRows;
		int numberOfColumns;

	public:
		~csvHandle(){
			streamHandle.close();
		}

		csvHandle(const std::string& forestCSVFileName){

			streamHandle.open(forestCSVFileName);
			numberOfRows = 0;

			while(streamHandle){
				std::string row;
				if(!getline(streamHandle, row))
				{
					break;
				}
				++numberOfRows;

				std::istringstream stringStream(row);

				std::string value;
				int tempNumberOfColumns = 0;
				while(stringStream)
				{
					if(!getline(stringStream,value,',')){
						break;
					}
				++tempNumberOfColumns;
				}
				if(!numberOfColumns){
numberOfColumns = tempNumberOfColumns; 
				}
				if(numberOfColumns != tempNumberOfColumns){
					throw std::runtime_error("uneven row lengths in csv file." );
				}
			}
		}

		void printCSVStats(){
			std::cout << "there are " << numberOfRows << " rows.\n";
			std::cout << "there are " << numberOfColumns << " columns.\n";
		}

		int returnNumRows(){return numberOfRows;}
		int returnNumColumns(){return numberOfColumns;}

};


#endif //fpUtil_h
