#include "gsl_math.h"
#include "gsl_cblas.h"
#include "cblas.h"

void
cblas_srot (const int N, float *X, const int incX, float *Y, const int incY,
            const float c, const float s)
{
#define BASE float
#include "source_rot.h"
#undef BASE
}