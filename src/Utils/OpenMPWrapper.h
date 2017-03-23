#ifdef __USE_OPENMP__
#include <omp.h>
#else
typedef int omp_int_t;
inline omp_int_t omp_get_thread_num()  { return 0; }
inline omp_int_t omp_get_max_threads() { return 1; }
#endif
