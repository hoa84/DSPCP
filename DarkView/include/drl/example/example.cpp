/**************************************************************************
                                example.cpp
                             -------------------

    Dr. L. Example
    -- Reduces the swiss roll from 3-dimensions to 2-dimensions using
       IsoMap.

 __________________________________________________________________________
    This file is part of the Dr. L. package
__________________________________________________________________________

    begin      : Tue May 5 2009
    authors    : W. Michael Brown
    email      : wmbrown@sandia.gov
 ***************************************************************************/

/* -----------------------------------------------------------------------
   Copyright (2009) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under 
   the GNU General Public License.
   ----------------------------------------------------------------------- */

#include <iostream>
#include "dimred/ya_dimred.h"
#include "dimred/ya_manifold_samples.h"

using namespace std;
using namespace yala;

int main(int argc, char *argv[]) {
  // Generate a warning message if this was compiled in debug mode
  ya_check_debug();
  MathRandom<MathMersenneTwister> rng;

  // Number of sample points and matrices for input and output
  int nS=400;
  double input_matrix[nS*3];
  double output_matrix[nS*2];
  
  // Create wrappers for using matrices with Dr. L.
  YA_WRAP(double) winput(input_matrix,nS,3);
  YA_WRAP(double) woutput(output_matrix,nS,2);

  // Sample the swiss roll
  int colors[nS];
  ya_manifold_swissroll(winput,colors,nS,rng);

  // Set the low dimensionality to 2
  int low_dim=2;
  
  // Use IsoMap with progress meter and timing output
  YAIsoReduce<double> redmet;
  redmet.verbose(2);
  
  // Eigen decomposition options
  EigenOptions eigopts;
  
  // Use k-nearest neighbors with k=10
  redmet.neighbor_mode(0);
  redmet.neighbors(10);
  
  // Reduce the dimensionality
  cout << "Performing IsoMap on " << nS << "  Swiss Roll Samples.\n" << flush;
  redmet.find_t(winput,woutput,low_dim,eigopts);
  
  // Create a new test set of samples from the swiss roll
  int nT=100;
  double test_matrix[nT*3];
  YA_WRAP(double) wtest(test_matrix,nT,3);
  assert(nT<nS); // So we can use same colors array as above
  ya_manifold_swissroll(wtest,colors,nT,rng);
  
  // Calculate the reconstruction error for the test set.
  cout << "Calculating Reconstruction Error for " << nT 
       << " Test Set Samples.\n" << flush;
  double re=redmet.reconstruct_error(wtest);
  cout << "Reconstruction error is: " << re << endl;
  
  return 0;
}
