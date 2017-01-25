/***************************************************************************
                             cuda_knn_texture.h
                             -------------------
                               W. Michael Brown

  Tricks for templating textures for k-nearest neighbors

 __________________________________________________________________________
    This file is part of the Math Library
 __________________________________________________________________________

    begin                : Tue Feb 10 2009
    copyright            : (C) 2009 by W. Michael Brown
    email                : wmbrown@sandia.gov
 ***************************************************************************/

/* -----------------------------------------------------------------------
   Copyright (2009) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under 
   the GNU General Public License.
   ----------------------------------------------------------------------- */

#include "cuda_calls.h"

#ifndef CUDA_KNN_TEXTURE_H
#define CUDA_KNN_TEXTURE_H

// Texture for accessing constant input matrix
static texture<float, 2, cudaReadModeElementType> knn_float_tex;
static texture<int2, 2, cudaReadModeElementType> knn_double_tex;

// Get a channel for float array
template <class numbr>
inline void cuda_knn_get_channel(cudaChannelFormatDesc &channel) {
  channel = cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindFloat);
}

// Get a channel for double array
template <>
inline void cuda_knn_get_channel<double>(cudaChannelFormatDesc &channel) {
  channel = cudaCreateChannelDesc(32, 32, 0, 0, cudaChannelFormatKindSigned);
}

// Bind a float array to a texture
template <class numbr>
inline void knn_bind_texture(const cudaArray* array,
                             const cudaChannelFormatDesc *channel) {
  knn_float_tex.addressMode[0] = cudaAddressModeClamp;
  knn_float_tex.addressMode[1] = cudaAddressModeClamp;
  knn_float_tex.filterMode = cudaFilterModePoint;
  knn_float_tex.normalized = false;
  CUDA_SAFE_CALL(cudaBindTextureToArray(&knn_float_tex,array,channel));
}

#ifdef CUDA_DOUBLE
// Bind a double array to a texture
template <>
inline void knn_bind_texture<double>(const cudaArray* array,
                                     const cudaChannelFormatDesc *channel) {
  knn_double_tex.addressMode[0] = cudaAddressModeClamp;
  knn_double_tex.addressMode[1] = cudaAddressModeClamp;
  knn_double_tex.filterMode = cudaFilterModePoint;
  knn_double_tex.normalized = false;
  CUDA_SAFE_CALL(cudaBindTextureToArray(&knn_double_tex,array,channel));
}
#endif

// Unbind a float array from a texture
template <class numbr>
inline void knn_unbind_texture() {
  cudaUnbindTexture(knn_float_tex);
}

// Unbind a double array from a texture
template <>
inline void knn_unbind_texture<double>() {
  cudaUnbindTexture(knn_double_tex);
}

// Access a 2D element from float textures
template <class numbr>
static __inline__ __device__ numbr cuda_knn_tex2D(const int i, const int j) {
  return tex2D(knn_float_tex,i,j);
}

#ifdef CUDA_DOUBLE
// Access a 2D element from double textures
template <>
static __inline__ __device__ double cuda_knn_tex2D<double>(const int i, 
                                                           const int j) {
  int2 t=tex2D(knn_double_tex,i,j);
  return __hiloint2double(t.y, t.x);
}
#endif

#endif
