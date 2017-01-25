#***************************************************************************
#                               Makefile
#                          -------------------
#
#  _________________________________________________________________________
#
#  Configuration for Makefiles
#
#  _________________________________________________________________________
#
#    begin                : Thu Oct 22 2003
#    copyright            : (C) 2003 by W. Michael Brown
#    email                : wmbrown@sandia.gov
# ***************************************************************************/

#Compiler type
COMPILER = gnu_fast
#COMPILER = gnu_fast_atlas
#COMPILER = intel_openmp
#COMPILER = gnu_safe
#COMPILER = gnu_safe_atlas
#COMPILER = gnu_fast_cygwin
#COMPILER = gnu_safe_cygwin
#COMPILER = gnu4_openmp

#MPI (0 don't use MPI, 1 use MPI)
USE_MPI = 0

#CUDA (0 don't use GPU, 1 use GPU, 2 use Tesla GPU)
USE_CUDA = 0



#Locations of outside objects relative to a source directory
HOBJ_DIR = ../../obj
BIN_DIR = ../../bin/

ALL_DIR = ../all
ALL_LIB = $(HOBJ_DIR)/liball.a

MATH_DIR = ../math

CUDA_DIR = ../cuda

DRL_DIR = ../../drl

EVERY_LIB = $(ALL_LIB)

# Include directories
INC         = -I$(ALL_DIR) -I$(MATH_DIR)

# Set up CUDA parameterization
ifeq ($(USE_CUDA),1)
  CUDA      = nvcc -I/usr/local/cuda/include -DUNIX -O3 -DDEBUG
  CUDALNK   = -L/usr/local/cuda/lib -lcudart $(CUDA_LIB)
  CUDACPP   = -DCUDA -I../cuda -DDEBUG
  CUDA_LIB = $(HOBJ_DIR)/libcuda.a
endif
ifeq ($(USE_CUDA),2)
  CUDA      = nvcc -I/usr/local/cuda/include -DUNIX -O3 -DDEBUG -arch=sm_13
  CUDALNK   = -L/usr/local/cuda/lib -lcudart $(CUDA_LIB)
  CUDACPP   = -DCUDA -DCUDA_DOUBLE -I../cuda -DDEBUG
  CUDA_LIB = $(HOBJ_DIR)/libcuda.a
  USE_CUDA = 1
endif

ifeq ($(COMPILER),intel_openmp)
  CPP         = icpc 
  OPENMP      = -openmp -DYA_OMP
  AR          = xiar
  OPT         = -O2 -DYA_PROGMETER -xT -ansi-alias #-vec-report3 -ipo -no-prec-div -static
  CBLASF      = -DYA_BLAS -DYA_LAPACK -DYA_BLAS_MULT -I/opt/intel/mkl/10.0.3.020/include
  CBLAS       = -L/opt/intel/mkl/10.0.3.020/lib/em64t/ -lmkl_lapack -lmkl
#-DYA_INTELVML
endif

ifeq ($(COMPILER),gnu_fast_atlas)
  CPP         = g++
  CC          = gcc
  AR          = ar
  OPT         = -O2 -DYA_PROGMETER
  CBLASF      = -DYA_BLAS -DYA_LAPACK -DYA_BLAS_MULT
  CBLAS       = -L/usr/local/atlas/lib -llapack -lptcblas -lptf77blas -latlas -ltmglib -lnsl -lg2c /usr/lib/gcc/x86_64-redhat-linux/4.1.1/libgfortran.a -lpthread
endif

ifeq ($(COMPILER),gnu_fast)
  CPP         = g++
  CC          = gcc
  AR          = ar
  OPT         = -O2 -DYA_PROGMETER
endif

ifeq ($(COMPILER),gnu_safe_atlas)
  CPP         = g++
  CC          = gcc
  AR          = ar
  DBUG        = -g -DYA_DEBUG1 -DYA_SAFE -Wall -pedantic -Wno-unknown-pragmas #-ansi
  OPT         = -O2 -DYA_PROGMETER
  CBLASF      = -DYA_BLAS -DYA_LAPACK -DYA_BLAS_MULT
  CBLAS       = -L/usr/local/atlas/lib -llapack -lptcblas -lptf77blas -latlas -ltmglib -lnsl -lg2c /usr/lib/gcc/x86_64-redhat-linux/4.1.1/libgfortran.a -lpthread
endif

ifeq ($(COMPILER),gnu_safe)
  CPP         = g++
  CC          = gcc
  AR          = ar
  DBUG        = -g -DYA_DEBUG1 -DYA_SAFE -Wall -pedantic -Wno-unknown-pragmas #-ansi
  OPT         = -O2 -DYA_PROGMETER
endif

ifeq ($(COMPILER),gnu_fast_cygwin)
  CPP         = g++
  CC          = gcc
  AR          = ar
  OPT         = -O2 -DYA_PROGMETER
  CBLASF = -DYA_BLAS -DYA_LAPACK -DYA_BLAS_MULT
  CBLAS =  -L/usr/local/atlas/lib -llapack -lptcblas -lptf77blas -latlas -ltmglib -lg2c -lpthread
endif

ifeq ($(COMPILER),gnu_safe_cygwin)
  CPP         = g++
  CC          = gcc
  AR          = ar
  DBUG        = -g -DYA_DEBUG1 -DYA_SAFE -Wall -pedantic  -Wno-unknown-pragmas #-ansi
  OPT         = -O2 -DYA_PROGMETER
  CBLASF = -DYA_BLAS -DYA_LAPACK -DYA_BLAS_MULT
  CBLAS =  -L/usr/local/atlas/lib -llapack -lptcblas -lptf77blas -latlas -ltmglib -lg2c -lpthread
endif

ifeq ($(COMPILER),gnu4_openmp)
  CPP         = g++
  CC          = gcc
  OPENMP      = -fopenmp -DYA_OMP
  AR          = ar
  OPT         = -O3 -DYA_PROGMETER
  CBLASF      = -DYA_BLAS -DYA_LAPACK -DYA_BLAS_MULT
  CBLAS       = -llapack -lblas
endif

ifeq ($(USE_MPI),1)
  CPP    = mpicxx -DYA_MPI
endif

CFLAGS        = $(CUDACPP) $(OPT) $(OPENMP) $(CBLASF) $(DBUG) $(INC) -c
LFLAGS        = $(OPT) $(OPENMP)
LLIBS         =
