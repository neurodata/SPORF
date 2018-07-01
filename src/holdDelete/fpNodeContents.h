#ifndef fpNodeContents_h
#define fpnodeContents_h
#include <iostream>
#include <vector>
#include <algorithm>


template <class T>
class nodeContents
{
	private:
		std::vector<T>::iterator firstElement;
		std::vector<T>::iterator lastElement;

	public:
		nodeCreator(std::vector<T>::iterator firstInSet, std::vector<T>::iterator lastInSet): firstElement(firstInSet), lastElement(lastInSet){}

		inline int numElementsInNode(){
return std::distance(firstElement, lastElement);
		}
};

template <class T>
class featureToTest
{
	private:
		std::vector<T> featureValues;

	public:
		featureToTest(){}

		inline int numElementsInNode(){
return std::distance(firstElement, lastElement);
		}
};


template <class T>
class tree
{
	private:
		std::vector<node> nodesInTree;

	public:


}
#endif //fpNodeContents_h
