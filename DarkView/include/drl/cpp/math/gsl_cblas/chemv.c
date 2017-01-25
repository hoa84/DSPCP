#include "gsl_math.h"
#include "gsl_cblas.h"
#include "cblas.h"

void
cblas_chemv (const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
             const int N, const void *alpha, const void *A, const int lda,
             const void *X, const int incX, const void *beta, void *Y,
             const int incY)
{
#define BASE float
#include "source_hemv.h"
#undef BASE
}
