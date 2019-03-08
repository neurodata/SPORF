#ifndef inputMatrixDataColMajor_h
#define inputMatrixDataColMajor_h

#include "inputMatrixData.h"
#include <xmmintrin.h>

namespace fp
{

template <typename T, typename Q>
class inputMatrixDataColMajor : public inputMatrixData<T, Q>
{
  public:
	inputMatrixDataColMajor(const T *Xmat, const Q *Yvec, int numObs, int numFeatures) : inputMatrixData<T, Q>(Xmat, Yvec, numObs, numFeatures)
	{
	}

	inline T returnFeatureValue(const int &featureNum, const int &observationNum)
	{
		return this->inputXData[this->numObs * featureNum + observationNum];
	}

	inline void prefetchFeatureValue(const int &featureNum, const int &observationNum)
	{
		//_mm_prefetch(&this->inputXData[this->numObs * featureNum + observationNum], _MM_HINT_T0);
		__builtin_prefetch(&this->inputXData[this->numObs * featureNum + observationNum], 0, 1);
	}

inline void prefetchFeatureValue(const int &featureNum, const int &observationNum, const int &hintNum)
	{
		//_mm_prefetch(&this->inputXData[this->numObs * featureNum + observationNum], hintNum);
//		__builtin_prefetch(&this->inputXData[this->numObs * featureNum + observationNum], 0, hintNum);
	}
};

} // namespace fp
#endif // inputMatrixDataColMajor_h
