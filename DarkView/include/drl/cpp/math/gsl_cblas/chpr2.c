#include "gsl_math.h"
#include "gsl_cblas.h"
#include "cblas.h"

void
cblas_chpr2 (const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
             const int N, const void *alpha, const void *X, const int incX,
             const void *Y, const int incY, void *Ap)
{
#define BASE float
#include "source_hpr2.h"
#undef BASE
}