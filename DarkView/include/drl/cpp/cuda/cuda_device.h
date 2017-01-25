/***************************************************************************
                               cuda_device.h
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

/* -----------------------------------------------------------------------
   Copyright (2009) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under 
   the GNU General Public License.
   ----------------------------------------------------------------------- */

#include <string>

using namespace std;
 
/// Choose the CUDA device with highest compute capability and number of cores
/** Returns false if no devices found **/
bool choose_cuda_device(int &num);

/// Set the CUDA device to the specified device number
void set_cuda_device(int num);

/// Get the current CUDA device name
string get_cuda_device_name();
