/***************************************************************************
                                 cuda_timer.h
                             -------------------
                               W. Michael Brown

  Class for timing CUDA routines

 __________________________________________________________________________
    This file is part of the Math Library
 __________________________________________________________________________

    begin                : Tue Feb 3 2009
    copyright            : (C) 2009 by W. Michael Brown
    email                : wmbrown@sandia.gov
 ***************************************************************************/

/* -----------------------------------------------------------------------
   Copyright (2009) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under 
   the GNU General Public License.
   ----------------------------------------------------------------------- */

#ifndef CUDA_TIMER_H
#define CUDA_TIMER_H

#include "cuda_calls.h"

/// Class for timing CUDA events
class TimerCUDA {
 public:
  TimerCUDA() { 
  CUDA_SAFE_CALL( cudaEventCreate(&start_event) );
    CUDA_SAFE_CALL( cudaEventCreate(&stop_event) );
  }

  /// Start timing
  inline void start() { cudaEventRecord(start_event,0); }
  
  /// Stop timing and store event time
  inline void stop() {
    cudaEventRecord(stop_event,0);
    cudaEventSynchronize(stop_event);
    CUDA_SAFE_CALL( cudaEventElapsedTime(&timer,start_event,stop_event) );
  }
  
  /// Stop timing, store event time, and restart timing
  inline void next() { stop(); start(); }
  
  /// Return the last stored event time
  inline float time() { return timer; }

 private:
  cudaEvent_t start_event, stop_event;
  float timer;
};

#endif
