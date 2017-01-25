This is the Dr. L (Dimensionality Reduction Library) software package,
version 

Copyright (2009) Sandia Corporation.  Under the terms of Contract
DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
certain rights in this software.  This software is distributed under
the GNU General Public License.

----------------------------------------------------------------------

Dr. L. is a C++ library for dimensionality reduction. In the context 
of this library, dimensionality reduction is considered to consist of:

   1. Estimation of the intrinsic dimensionality using sampled data
   2. Finding maps that reduce the dimensionality of data (forward 
      map) or increase the dimensionality of data (reverse map).
   3. Mapping arbitrary coordinates to high and low dimensionalities 

The library is intended to provide a consistent interface to 
multiple dimensionality reduction algorithms with an efficient 
C++ interface that runs efficiently on multicore architectures. A 
few routines have been optimized with an option for GPU acceleration 
or distributed computation.

An executable (cml_drl) is also supplied that can be built to allow for 
command-line access to the library routines. 

The primary author of the code is Mike Brown, who can be emailed
at wmbrown@sandia.gov. 

This distribution includes the following files and directories:

README         this file
LICENSE        the GNU General Public License (GPL)
drl            directory containing libraries headers to include with your code
api            pdf and html API for Dr. L.
example        example application using Dr. L.
build	       Visual Studio files for building cml_drl executable
bin            directory containing cml_drl executable after building
manpages       man pages for cml_drl executable
cpp            source code for cml_drl executable
obj            location of object files for cml_drl builds

----------------------------------------------------------------------

To get started, see http://www.cs.sandia.gov/~wmbrown/drl


