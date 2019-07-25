#ifndef weightedFeature_h
#define weightedFeature_h

#include <vector>

namespace fp {

	class weightedFeature
	{
		protected:
			std::vector<int> featureNum;
			std::vector<float> featureWeight;

		public:
			inline std::vector<int>& returnFeatures(){
				return featureNum;
			}

			inline std::vector<float>& returnWeights(){
				return featureWeight;
			}
	};
}
#endif //weightedFeature_h
