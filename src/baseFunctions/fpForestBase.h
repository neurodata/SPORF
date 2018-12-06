#ifndef fpForestBase_h
#define fpForestBase_h

namespace fp{

	template <typename T>
		class fpForestBase
		{

			public:
				virtual void printForestType() = 0;
				virtual void growForest() = 0;
				virtual float testForest() = 0;
				virtual int predictClass(std::vector<T>& observation) = 0;
		};

}//namespace fp
#endif //fpForestBase.h
