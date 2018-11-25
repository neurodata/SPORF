#ifndef fpForestBase_h
#define fpForestBase_h

namespace fp{

class fpForestBase
{

	public:
		virtual void printForestType() = 0;
		virtual void growForest() = 0;
		virtual float testForest() = 0;
};

}//namespace fp
#endif //fpForestBase.h
