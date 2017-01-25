#include "gsl_math.h"
#include "gsl_cblas.h"
#include "cblas.h"

void
cblas_cgerc (const enum CBLAS_ORDER order, const int M, const int N,
             const void *alpha, const void *X, const int incX, const void *Y,
             const int incY, void *A, const int lda)
{
#define BASE float
#include "source_gerc.h"
#undef BASE
}