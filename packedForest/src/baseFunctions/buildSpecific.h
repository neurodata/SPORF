#ifndef buildSpecific_h
#define buildSpecific_h

//Test show globalPrefetchSize >1 is beneficial.  >32 is detrimental.
//This is architecture specific.
const int globalPrefetchSize=32;

//use mm_prefetch for non GNU compilers
#if (defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_AMD64))) || defined(__INTEL_COMPILER)
#include <xmmintrin.h>
#define PREFETCHGATHER(addr) _mm_prefetch(addr, _MM_HINT_T0)
//use built_in prefetch for GNU compilers 
#elif defined(__GNUC__)
#define PREFETCHGATHER(addr) __builtin_prefetch(addr, 0, 3)
//Don't use prefetching for unknown compilers
#else
#define PREFETCHGATHER(addr) do {} while (0)
#endif
#endif //buildSpecific_h
