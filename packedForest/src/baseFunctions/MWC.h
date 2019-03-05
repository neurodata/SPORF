#ifndef MWC_h
#define MWC_h


#include <random>

class randomNumberRerFMWC{
	private:
		unsigned int z, w;

	public:
		randomNumberRerFMWC(): z(362436069), w(521288629){
		}

		inline void initialize(int seed){
			z = 362436069 + seed;
			w = 521288629 + seed;
			if(z == 0 || w == 0){
				z = 362436069;
				w = 521288629;
			}
		}

		inline void initialize(){
			std::random_device rd;
			do{
				z += rd();
				w += rd();
			}while(z==0 || w == 0);
		}

		inline int gen(){
			z = 36969*(z&65535)+(z>>16);
			w = 18000*(w&65535)+(w>>16);
			return (z<<16)+w;
		}

		inline int gen(int range){
			z = 36969*(z&65535)+(z>>16);
			w = 18000*(w&65535)+(w>>16);
			return ((z<<16)+w)%range;
		}

};

#endif //MWC_h
