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

#include <iostream>
#include "cuda_calls.h"
#include "cuda_knn.h"
#include "cuda_timer.h"
#include "cuda_device.h"
#include "cuda_knn_texture.h"

using namespace std;


template void cuda_kneighbors(const float *in, const cu_sizet k, 
                              cu_sizet *neighbors, float *dists, 
              							  const cu_sizet rows, const cu_sizet cols);

#ifdef CUDA_DOUBLE
template void cuda_kneighbors(const double *in, const cu_sizet k, 
                              cu_sizet *neighbors, double *dists, 
							                const cu_sizet rows, const cu_sizet cols);
#endif

// Numer of threads for 1D blocks
#define BLOCK_1D 64
// Sqrt(number of threads) for 2D blocks 
#define BLOCK_2D 16

// 1D block adjustment for float and double precision
template <class numbr> inline int cuda_knn_block1d() { return 1; }
template <> inline int cuda_knn_block1d<double>() { return 4; }


/***************************************************************************
 * Compute the symmetric distance matrix for the input data
 * 
 * dist - N x N matrix of distances to be computed (diagonal is infinity)
 * d - Dimensionality of the input data
 * dist_pitch - Size of a row in the matrix in memory
 ***************************************************************************/
template<class numbr>
__global__ void kernel_knn(numbr *dist, const int d, const size_t dist_pitch) {
  // ti and tj index the threads within the block
  int ti=threadIdx.x;
  int tj=threadIdx.y;
  // vdim is the number of datapoint dimensions that can fit in shared memory
  int vdim=blockDim.y;
  
  // i and j index the two datapoints being compared
  int i=ti+blockIdx.x*blockDim.x;
  int j=tj+blockIdx.y*vdim;
  
  // Each block stores datapoints indexed by all threads in the block
  __shared__ numbr in_blocki[BLOCK_2D][BLOCK_2D+1];
  __shared__ numbr in_blockj[BLOCK_2D][BLOCK_2D+1];
  
  // If i==j set to inf, else, to 0
  numbr distance;
  if (i==j)
    distance=cuda_inf<numbr>();
  else
    distance=cuda_zero<numbr>();
	
  for (int l=0; l<d; l+=vdim) {
    // Each thread loads two elements of shared input matrices
    in_blocki[ti][tj]=cuda_knn_tex2D<numbr>(i,l+tj);
    in_blockj[tj][ti]=cuda_knn_tex2D<numbr>(j,l+ti);
	// Loop over the number of dimensions that fit in shared memory
	int loop_end=vdim;
	if (l+loop_end>d)
	  loop_end=d-l;
    __syncthreads();
	// Compute distance between i and j
    for (int k=0; k<loop_end; ++k) {
      numbr d2=in_blocki[ti][k]-in_blockj[tj][k];
	  distance+=d2*d2;
	}
	// Sync so that we don't load new 'vdim' set of dimensions too soon
    __syncthreads();
  }

  // Store distance in global memory
  dist[INT_MUL(j,dist_pitch)+i]=distance;
}

/***************************************************************************
 * Sort to find k closest distances and corresponding datapoint indices
 * O(n*k)
 * 
 * sort - k+1 x N matrix containing k-closest distances in sorted order
 * nbor - k+1 x N matrix containing indices of k-closest neighbors
 * dist - N x N matrix of distances between datapoints
 * k - number of neighbors
 * n - number of datapoints (N)
 * sort_pitch, dist_pitch, nbor_pitch - row sizes of each supplied matrix
 ***************************************************************************/
template<class numbr>
__global__ void kernel_knnsort(numbr *sort, int *nbor, const numbr *dist, 
                               const int k, const int n, const size_t sort_pitch,
							   const size_t dist_pitch, const size_t nbor_pitch){
  // i indexes the datapoint whose neighbors are being sorted
  int i=threadIdx.x+blockIdx.x*blockDim.x;

  // Minimum distance of Inf to start with
  int loop_end=INT_MUL(k+1,sort_pitch);
  for (int z=i; z<loop_end; z+=sort_pitch)
	sort[z]=cuda_inf<numbr>();
  
  // Insertion sort O(n*k)
  const numbr *d_row=dist+i;
  for (int z=0; z<n; ++z) {
    numbr *c=sort+INT_MUL(k-1,sort_pitch)+i;
	int *d=nbor+INT_MUL(k-1,nbor_pitch)+i;
    for (int y=0; y<k && *d_row<*c; ++y) {
	  *(c+sort_pitch)=*c;
  	  *(d+nbor_pitch)=*d;
	  c-=sort_pitch;
	  d-=nbor_pitch;
	}
	*(c+sort_pitch)=*d_row;
	*(d+nbor_pitch)=z;
	d_row+=dist_pitch;
  }
  
  for (int z=i; z<loop_end; z+=sort_pitch)
    sort[z]=sqrt(sort[z]);
}

template<class numbr>
void cuda_kneighbors(const numbr *in, const cu_sizet k, cu_sizet *neighbors,
                     numbr *dists, const cu_sizet rows, const cu_sizet cols) {
  cout << endl << "Calculating k-neighbors on GPU: " << get_cuda_device_name() 
       << endl;
  
  // Timing stuff
  #ifdef CUDA_TIME
  TimerCUDA timer;
  timer.start();
  #endif
  
  // Determine the block size and grid size
  const int BX=BLOCK_2D;
  const int BY=BLOCK_2D;
  // Adjust 1D grid size if we are using double precision
  const int B1D=BLOCK_1D/cuda_knn_block1d<numbr>();
  int GX=static_cast<int>(ceil(static_cast<double>(cols)/
                               static_cast<double>(BX)));
  int GY=static_cast<int>(ceil(static_cast<double>(cols)/
                               static_cast<double>(BY)));
  int GX2=static_cast<int>(ceil(static_cast<double>(cols)/
                                static_cast<double>(B1D)));

  // Calculate matrix dimensions that give even division of work
  // - Matrices are allocated with extra room for load-balancing
  cu_sizet v_width=max(GX*BX,GX2*B1D);
  cu_sizet v_height=max(GY*BY,GX2*B1D);
  cu_sizet v_dims=rows;
  if (rows%BY!=0)
    v_dims+=(BY-rows%BY);

  // Allocate memory on device for input matrix and distance matrix
  numbr *dev_dist;
  cudaArray* dev_in;
  size_t dist_pitch;

  // Create read-only texture for input matrix and copy data to device
  cudaChannelFormatDesc channel;
  cuda_knn_get_channel<numbr>(channel);
  CUDA_SAFE_CALL(cudaMallocArray(&dev_in, &channel, v_width, v_dims));
  for (int i=0; i<rows; i++)
    CUDA_SAFE_CALL(cudaMemcpyToArray(dev_in, 0, i, in+i*cols, cols*sizeof(numbr),
	                                 cudaMemcpyHostToDevice));
  knn_bind_texture<numbr>(dev_in,&channel);

  // Reserve memory for pairwise distance matrix on device
  CUDA_SAFE_CALL(cudaMallocPitch((void **)&dev_dist,&dist_pitch,
                                 v_width*sizeof(numbr),v_height));

  #ifdef CUDA_TIME
  timer.next();
  printf("malloc and memcpy:\t%f\n", timer.time());
  #endif

  // Calculate pairwise distance matrix
  dim3 dimBlock(BX,BY);
  dim3 dimGrid(GX,GY);
  kernel_knn<numbr><<<dimGrid,dimBlock>>>(dev_dist,rows,
                                          dist_pitch/sizeof(numbr));
  #ifdef CUDA_TIME
  timer.next();
  printf("knn:\t\t\t%f\n", timer.time());
  #endif

  // Free memory from input matrix and reserve memory for sorted distances
  // and neighbor indices
  knn_unbind_texture<numbr>();
  cudaFreeArray(dev_in);
  numbr *dev_sort;
  size_t sort_pitch, nbor_pitch;
  int *dev_nbor;
  CUDA_SAFE_CALL(cudaMallocPitch((void **)&dev_nbor,&nbor_pitch,
                                 v_width*sizeof(int),k+1));
  CUDA_SAFE_CALL(cudaMallocPitch((void **)&dev_sort,&sort_pitch,
                                 v_width*sizeof(numbr),k+1));

  #ifdef CUDA_TIME
  timer.next();
  printf("malloc:\t\t\t%f\n", timer.time());
  #endif

  kernel_knnsort<numbr><<<GX2,B1D>>>(dev_sort,dev_nbor,dev_dist,k,cols,
                                     sort_pitch/sizeof(numbr),
									 dist_pitch/sizeof(numbr),
									 nbor_pitch/sizeof(int));
  #ifdef CUDA_TIME
  timer.next();
  printf("sort:\t\t\t%f\n", timer.time());
  #endif

  // Copy distances and neighbors back to host
  CUDA_SAFE_CALL(cudaMemcpy2D(dists,cols*sizeof(numbr),dev_sort,sort_pitch,
                              cols*sizeof(numbr),k,cudaMemcpyDeviceToHost));
  CUDA_SAFE_CALL(cudaMemcpy2D(neighbors,cols*sizeof(int),dev_nbor,nbor_pitch,
                              cols*sizeof(int),k,cudaMemcpyDeviceToHost));

  // Free allocated memory
  cudaFree(dev_nbor);
  cudaFree(dev_dist);
  #ifdef CUDA_TIME
  timer.stop();
  printf("memcpy and free:\t%f\n", timer.time());
  #endif
}
