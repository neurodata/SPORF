#ifndef fpReadCSV_h
#define fpReadCSV_h
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>


namespace fp {

	/**
	 * fpReadCSV makes two passes over a csv file.  This could be bad if
	 * the csv file is large.  The first pass makes sure the csv file
	 * is structured correctly and collects basic info about the csv:
	 * number of rows and columns.  The file handle is then returned to
	 * the beginning of the file and returns individual elements in order.
	 */

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

			csvHandle(const std::string& forestCSVFileName) : numberOfRows(0), numberOfColumns(0){

				streamHandle.open(forestCSVFileName);
				numberOfRows = 0;

				if(!streamHandle.good()){
					throw std::runtime_error("Unable to open file." );
					return;
				}

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
						return;
					}
				}
				streamHandle.clear(); // clear fail and eof bits
				streamHandle.seekg(0, std::ios::beg); // return to beginning of stream
			}

			void printCSVStats(){
				std::cout << "there are " << numberOfRows << " rows.\n";
				std::cout << "there are " << numberOfColumns << " columns.\n";
			}

			int returnNumRows(){return numberOfRows;}
			int returnNumColumns(){return numberOfColumns;}

			template <class T>
				inline T returnNextElement(){
					T temp;
					streamHandle >> temp;
					streamHandle.ignore(1, ',');
					return temp;
				}
	};

} //namespace fp
#endif //fpReadCSV_h
