/***************************************************************************
                                  cuda_knn.h
                             -------------------
                               W. Michael Brown

  K-nearest neighbors routines for CUDA

 __________________________________________________________________________
    This file is part of the Math Library
 __________________________________________________________________________

    begin                : Wed Jan 28 2009
    copyright            : (C) 2009 by W. Michael Brown
    email                : wmbrown@sandia.gov
 ***************************************************************************/

/* -----------------------------------------------------------------------
   Copyright (2009) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under 
   the GNU General Public License.
   ----------------------------------------------------------------------- */

#ifndef CUDA_KNN_H
#define CUDA_KNN_H

typedef int cu_sizet;

/// K-Nearest Neighbors using GPU
/** in - D x N matrix of datapoints
  *    - if each col is a datapoint, host matrix must be D x N row-major
  *    - if each row is a datapoint, host matrix must be N x D column-major
  * k  - number of nearest neighbors to compute
  * neighbors - sorted indices of k-nearest neighbors
  *           - memory must be allocated before call
  *           - N x k column major
  * dists     - sorted distances to k-nearest neighbors
  *           - memory must be allocated before call
  *           - N x k column major
  * D         - dimensionality of the datapoints
  * N         - number of datapoints **/
template<class numbr>
void cuda_kneighbors(const numbr *in, const int k, cu_sizet *neighbors,
                     numbr *dists, const cu_sizet D, const cu_sizet N);

#endif
