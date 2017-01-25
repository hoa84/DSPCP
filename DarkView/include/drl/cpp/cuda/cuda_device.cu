/***************************************************************************
                               cuda_device.cu
                             -------------------
                               W. Michael Brown

  Utilities for dealing with cuda devices

 __________________________________________________________________________
    This file is part of the Math Library
 __________________________________________________________________________

    begin                : Wed Jan 28 2009
    copyright            : (C) 2009 by W. Michael Brown
    email                : wmbrown@sandia.gov
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cuda_calls.h"
#include "cuda_device.h"

bool choose_cuda_device(int &num) {
  int deviceCount;
  CUDA_SAFE_CALL(cudaGetDeviceCount(&deviceCount));
  if (deviceCount == 0)
	return false;
  
  num = 0;
  int major = 0, minor = 0, mp = 0;
  
  cudaDeviceProp deviceProp;
  for (int i=0; i<deviceCount; i++) {
    CUDA_SAFE_CALL(cudaGetDeviceProperties(&deviceProp, i));
    if (i == 0 && deviceProp.major == 9999 && deviceProp.minor == 9999) {
      printf("There is no device supporting CUDA\n");
	  return false;
	} 
	
	bool better=false;
	if (deviceProp.major>=major) {
	  if (deviceProp.major>major)
	    better=true;
	  else if (deviceProp.minor>=minor) {
	    if (deviceProp.minor>minor)
		  better=true;
		else if (deviceProp.multiProcessorCount>mp)
		  better=true;
      }
    }
	
	if (better) {
	  num=i;
	  major=deviceProp.major;
	  minor=deviceProp.minor;
	  mp=deviceProp.multiProcessorCount;
	}
  }
  return true;
}

/// Set the CUDA device to the specified device number
void set_cuda_device(int num) {
  CUDA_SAFE_CALL(cudaSetDevice(num));
}

/// Get the current CUDA device name
string get_cuda_device_name() {
  int num;
  CUDA_SAFE_CALL(cudaGetDevice(&num));
  string name;
  cudaDeviceProp deviceProp;
  CUDA_SAFE_CALL(cudaGetDeviceProperties(&deviceProp, num));
  name=deviceProp.name;
  return name;
}
