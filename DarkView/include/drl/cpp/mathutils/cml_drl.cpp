/**************************************************************************
                                 cml_drl.cpp
                             -------------------
        W. Michael Brown, Shawn Martin, Haixia Jia, Jean-Paul Watson
                             -------------------

    Command Line Utilities for Dimensionality Reduction Library

 __________________________________________________________________________
    This file is part of the Dr.L. library for command-line access to 
    Dr.L. library functions
__________________________________________________________________________

    begin      : Fri Oct 27 2006
    authors    : W. Michael Brown, Shawn Martin, Haixia Jia, Jean-Paul Watson
    email      : wmbrown@sandia.gov
 ***************************************************************************/

/* -----------------------------------------------------------------------
   Copyright (2009) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under 
   the GNU General Public License.
   ----------------------------------------------------------------------- */

#define AUTHORS "W. Michael Brown, Shawn Martin, Haixia Jia, Jean-Paul Watson"

#include "dimred/ya_dimred.h"
#include "dimred/ya_manifold_samples.h"
#include "base/ya_timer.h"
#include "commandline.h"
#include "base/ya_commandline.h"
#include <stdlib.h>

#ifdef YA_MPI
#include "mpi.h"
#endif

#ifdef CUDA
#include "cuda_device.h"
#endif

// Describe the program parameters
void Describe(CommandLine &cl,ostream &out);
// Parse the command line parameters
void HandleArgs(CommandLine &, int, char **, Error *, EigenOptions &);
// Determine the type of map for a previously saved reduction
string determine_map_type(const string filename, Error &error);

int main(int argc, char *argv[]) {
  #ifndef YA_MPI
  int me=0;
  int num_procs=1;
  #else
  int me, num_procs;
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  MPI_Comm_size(MPI_COMM_WORLD,&num_procs);
  YA_TimeKeeper mtk;
  mtk.mpi_wstart();
  #endif
  
  YA_TimeKeeper tk;
  tk.start();

  CommandLine cl;
  Error error;
  MathRandom<MathMersenneTwister> rng;
  ya_check_debug();

  #ifdef CUDA
  set_cuda_device(0);
  #endif

  // Parse the command line
  EigenOptions eigopts;
  HandleArgs(cl,argc,argv,&error,eigopts);

  // MPI Sanity Checks
  #ifdef YA_MPI
  if (num_procs>1 && !cl['u']) {
    if (me==0)
      error.generate_error(0,"cml_drl",
        "Can only use multiple procs for intrinsic dimensionality (-u).");
    MPI_Finalize();
    return 1;
  }
  #endif

  YA_MatD input_matrix;
  YA_MatD output_matrix;
  YA_MatI landmarks;
  ofstream out;

  // --------------------- Set the random number seed
  if (cl['b'])
    rng.seed(cl.argul('b',0));

  // --------------------- Set the accuracy for intel mkl
  #ifdef VM_INTEL_VML
  #include "mkl_vml.h"
  if (cl['g'])
    vmlSetMode( VML_LA );
  #endif
  
  // --------------------- Choose the reduction type
  enum {
    PCA,MDS,LLE,ISOMAP,AE
  };
  ya_sizet method=0;
  YADimReduce<double> *redmet=NULL;
  string method_str="PCA";
  if (cl['m'])
  method_str=cl.argstring('m',0);
  if (cl['f'])
  method_str=determine_map_type(cl.argstring('f',0),error);

  if (method_str=="PCA") {
    method=PCA;
    redmet=new YAPCAReduce<double>;
  } else if (method_str=="MDS") {
    method=MDS;
    redmet=new YAMDSReduce<double>;
  } else if (method_str=="LLE") {
    method=LLE;
    redmet=new YALLEReduce<double>;
  } else if (method_str=="ISOMAP") {
    method=ISOMAP;
    redmet=new YAIsoReduce<double>;
  } else if (method_str=="AE") {
     method=AE;
    redmet=new YAATEReduce<double>;
    reinterpret_cast<YAATEReduce<double>*>(redmet)->set_rng(&rng);
  } else
    error.generate_error(0,"cml_drl","Unsupported reduction type: "+
                         cl.argstring('m',0));

  // ------------- Choose the method for obtaining weights for neighbors
  if (cl['v'])
    redmet->neighbor_weight_mode(1);

  // ------------- Choose the statistics type
  enum {
    NONE, DISTANCE, RECONSTRUCT, ALL
  };
  ya_sizet stats=NONE;
  if (cl['s'])
    if (cl.argstring('s',0)=="NONE")
    stats=NONE;
  else if (cl.argstring('s',0)=="DISTANCE")
    stats=DISTANCE;
  else if (cl.argstring('s',0)=="RECONSTRUCT")
    stats=RECONSTRUCT;
  else if (cl.argstring('s',0)=="ALL")
    stats=ALL;
  else
    error.generate_error(0,"cml_drl","Unsupported statistic type: "+
                         cl.argstring('s',0));

  // ------------- Set up dimensionality
  ya_sizet low_dim=2;
  YA_MatI layers;
  if (method==AE && !cl['f']) {
  if (cl['d']) {
    layers=cl.argstring('d',0);
    if (layers.numel()==1)
      error.addwarning(0,4,"cml_drl",
                     "Only 1 dimensionality specified for AE reduction.");
    redmet->ae_layers(layers);
    low_dim=redmet->low_dim();
  } else
    error.generate_error(0,"cml_drl",
                         "Layers must be specified for AE with -d");
  } else
  if (cl['d'])
      low_dim=cl.argint('d',0);

  // ------------- Set up Eigen options
  if (cl['e']) 
    eigopts.method(cl.argstring('e',0));
  if (cl['c'])
    eigopts.tol(cl.argdouble('c',0));
  if (cl['i'])
    eigopts.max_iters(cl.argint('i',0));

  // ------------- Set up number of neighbors
  if (cl['k'])
    redmet->neighbors(cl.argint('k',0));
  if (cl['p'])
    redmet->epsilon(cl.argdouble('p',0));
  
  // ------------- Autoencoder parameters
  if (method==AE && cl['a']) {
    double frac=cl.argdouble('a',0);
  if (frac>1.0)
    error.generate_error(0,"cml_drl",
                         "train_test_split must be between 0 and 1.");
    reinterpret_cast<YAATEReduce<double>*>(redmet)->train_split(frac);
    reinterpret_cast<YAATEReduce<double>*>(redmet)->rbm_iters(cl.argint('a',1));
    reinterpret_cast<YAATEReduce<double>*>(redmet)->bp_iters(cl.argint('a',2));
    reinterpret_cast<YAATEReduce<double>*>(redmet)->rbm_size(cl.argint('a',3));
    reinterpret_cast<YAATEReduce<double>*>(redmet)->bp_size(cl.argint('a',4));
  }
  YA_MatD ate_test_matrix;
  if (cl['t'])
    load(cl.argstring('t',0),ate_test_matrix);
      
  // -------------------------- Load the input matrix
  int* colors=NULL;
  if (cl.argstring(' ',0)=="INT_SWISSROLL") {
     colors = new int[400];
     ya_manifold_swissroll(input_matrix,colors,400,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_SWISSROLL5") {
     colors = new int[5000];
     ya_manifold_swissroll(input_matrix,colors,5000,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_SWISSROLLN") {
     colors = new int[400];
     ya_manifold_swissroll1(input_matrix,colors,400,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_SWISSROLLN5") {
     colors = new int[5000];
     ya_manifold_swissroll1(input_matrix,colors,5000,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_THINSWISSROLL") {
     colors = new int[400];
     ya_manifold_thinswissroll(input_matrix,colors,400,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_THINSWISSROLL5") {
     colors = new int[5000];
     ya_manifold_thinswissroll(input_matrix,colors,5000,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_JAPAROLL") {
     colors = new int[400];
     ya_manifold_japaneseflag(input_matrix,colors,400,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_JAPAROLL5") {
     colors = new int[5000];
     ya_manifold_japaneseflag(input_matrix,colors,5000,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_NONDEVROLL") {
     colors = new int[400];
     ya_manifold_nondevroll(input_matrix,colors,400,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_NONDEVROLL5") {
     colors = new int[5000];
     ya_manifold_nondevroll(input_matrix,colors,5000,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_TKNOT") {
     colors = new int[400];
     ya_manifold_tknot(input_matrix,colors,400,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_TKNOT5") {
     colors = new int[5000];
     ya_manifold_tknot(input_matrix,colors,5000,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_SPHERE") {
     colors = new int[400];
     ya_manifold_sphere(input_matrix,colors,400,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_SPHERE5") {
     colors = new int[5000];
     ya_manifold_sphere(input_matrix,colors,5000,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_TORUS") {
     colors = new int[400];
     ya_manifold_torus(input_matrix,colors,400,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_TORUS5") {
     colors = new int[5000];
     ya_manifold_torus(input_matrix,colors,5000,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_BOX") {
     colors = new int[600];
     ya_manifold_box(input_matrix,colors,100,rng);
  } 
  else if (cl.argstring(' ',0)=="INT_BOX5") {
     colors = new int[6000];
     ya_manifold_box(input_matrix,colors,1000,rng);
  } else {
    if (me==0)
      error.note[9] << "Loading matrix: '" << cl.argstring(' ',0)
                    << "'...\n";
    load(cl.argstring(' ',0),input_matrix);
  }
  
  // Using internal matrix
  if (colors!=NULL) {
    if (method==MDS)
      input_matrix=YA_MatD(dist_mat(input_matrix));
    if (me==0)
      error.note[9] << "Using internal matrix: " << cl.argstring(' ',0) << endl;
  }
    
  if (cl['t'])
    if (input_matrix.cols()!=ate_test_matrix.cols())
      error.generate_error(0,"cml_drl",
        "Number of dimensions is different for training matrix and test matrix");

  // ------------- Use landmarks?
  if (cl['l']) {
    if (cl.argstring('l',0)=="FILE")
      load(cl.argstring('l',1),landmarks);
    else if (cl.argstring('l',0)=="STRING")
      landmarks=cl.argstring('l',1);
    else if (cl.argstring('l',0)=="RANDOM") {
      double frac=cl.argdouble('l',1);
      if (frac==0 || frac>=1.0)
        error.generate_error(0,"cml_drl",
                        "Fraction specified with -l RANDOM must be >0 and <1");
      landmarks=randperm(input_matrix.rows(),rng)(vmcount(ya_sizet(frac*
                                                         input_matrix.rows())));
    } else
      error.generate_error(0,"cml_drl",
                           "Invalid option to -l specified on command line.");
    if (landmarks.numel()==0)
      error.addwarning(0,9,"cml_drl","-l used, but no landmarks specified.");
    else
      error.note[9] << "Using " << landmarks.numel() << " landmark points.\n";
    if (maximum(landmarks)>=input_matrix.rows())
      error.generate_error(0,"cml_drl",
               "Landmark specified with index larger than number of points.");
    if (landmarks.numel()>0 && landmarks.numel()<=low_dim)
      error.generate_error(0,"cml_drl",
                       "Must use more landmarks than the low dimensionality.");
  }

  // ++++++++++ Get dimensions for statistics calculation
  YA_VecI dimensions;
  if (cl['x']) {
    YA_MatI temp_dim=cl.argstring('x',0);
    dimensions=reshape(temp_dim,temp_dim.numel(),1); // Make column vector
    ipsort(dimensions);
  } else
    dimensions=VM_ScalarU(low_dim);

  if (maximum(dimensions)>low_dim)
    error.generate_error(0,"cml_drl",
      "Dimensions specified with -x must be >0 and less than the low dimensionality.");

  // ----------------------- Load a previous map?
  if (cl['f']) {
    YA_MatD newin; // Use for calculating reconstruction error
    redmet->load_map(cl.argstring('f',0));
    if (input_matrix.cols()==redmet->high_dim()) {
      redmet->forward(input_matrix,output_matrix);
      if (cl['r'])
        redmet->reverse(output_matrix,newin);
    } else if (input_matrix.cols()==redmet->low_dim()) {
      redmet->reverse(input_matrix,output_matrix);
      if (cl['r'])
        redmet->forward(output_matrix,newin);
    } else {
      error.buffer() << "Dimensionality of input matrix (" 
                     << input_matrix.cols() << ") does not match high ("
                     << redmet->high_dim() << ") or low (" 
                     << redmet->low_dim() << ") dimensionalities.";
      error.addbuf(0,"cml_drl");
    }
    if (cl['r']) {
      if (cl['x']) {
        if (input_matrix.cols()!=redmet->high_dim()) {
          error.buffer() << "Multiple dimensions (-x) for reconstruction error "
                         << "can only be used with an input matrix in the high "
                         << "dimensionality.";
          error.addbuf(0,"cml_drl");
        }
        YA_RowD rmsds;
        if (method==PCA)
          reinterpret_cast<YAPCAReduce<double> *>(redmet)->
            reconstruct_error(input_matrix,output_matrix,dimensions,rmsds);
        else if (method==LLE || method==ISOMAP) {
          rmsds.setup(1,dimensions.numel());
        for (ya_sizet i=0; i<dimensions.numel(); i++) {
          YA_MatD recon_low,recon_high;
          redmet->nbors_forward(input_matrix,dimensions(i),recon_low);
          redmet->nbors_reverse(recon_low,dimensions(i),recon_high);
          rmsds(i)=rmsd(input_matrix,recon_high);
        }
      } else {
        error.buffer() << "Cannot specify multiple dimensions for "
                       << "reconstruction error calculation with autoencoder.";
        error.addbuf(0,"cml_drl");
      }
      cout << "Reconstruction_Errors: " << rmsds << endl << endl;
    } else {
      cout << "Reconstruction_Error: " << rmsd(input_matrix,newin) << endl;
    }
  }
  if (cl.argstring(' ',1)!="NO_OUTPUT")
    save(cl.argstring(' ',1),output_matrix,YA_PRETTY_IO);
  delete redmet;
  tk.end();
  cout << "\nExecution complete; total run-time=" << tk << ".\n\n";
  return 0;
  }

  // ----------------------- Sanity Checks
  if (input_matrix.rows()==0)
    error.generate_error(0,"cml_drl","Input matrix is empty!");
  if (input_matrix.cols()<low_dim || low_dim==0) {
    error.buffer() << "Cannot reduce to " << low_dim << " dimensions "
                   << " from " << input_matrix.cols() 
           << " dimensions.";
    error.addbuf(0,"cml_drl");
  }
  if (redmet->neighbors()>=input_matrix.rows() && 
    (method==LLE || method==ISOMAP)) {
    redmet->neighbors(input_matrix.rows());
    error.buffer() << "The number of neighbors exceeds the number of points. "
                   << "Resetting to " << input_matrix.rows();
    error.addbuf(0,4,"cml_drl");
  }
  if (method==AE && !cl['f'])
  if (layers(0)!=input_matrix.cols())
    error.generate_error(0,"cml_drl",
          "Initial layer for autoencoder must match input dimensionality.");

  // ----------------------- Intrinsic dimension estimation?
  if (cl['u']) {
    if (cl['d'] || (cl['x'] && dimensions.numel()==1)) {
      if (cl['x'])
        low_dim=dimensions(0);
      if (redmet->neighbor_mode() == 0)
        point_pca(input_matrix,redmet->neighbors(),VM_ScalarU(low_dim),
                  eigopts,output_matrix,2);
      else 
        point_pca_ep(input_matrix,redmet->epsilon(),VM_ScalarU(low_dim),
                     eigopts,output_matrix,2);
      if (me==0) {
        if (cl['z'])
          plot(output_matrix,"Residual Variance",'+');
        cout << "Maximum residual: " << maximum(output_matrix)
             << endl << "Minimum residual: " << minimum(output_matrix)
             << endl << "Mean residual: " 
             << total(output_matrix)/output_matrix.numel() << endl;
        if (cl.argstring(' ',1)!="NO_OUTPUT")
          save(cl.argstring(' ',1),output_matrix,YA_PRETTY_IO);
      }
    } else {
      YA_RowD vmin, vmax, vmean, vstd;
      if (!cl['x'])
        dimensions=vmcount(input_matrix.cols())+1;
      if (redmet->neighbor_mode() == 0)
        point_pca(input_matrix,redmet->neighbors(),dimensions,eigopts,
                  vmean,vmin,vmax,vstd,2);
      else 
        point_pca_ep(input_matrix,redmet->epsilon(),eigopts,vmean,vmin,vmax,
                     vstd,2);
      output_matrix=concat(vmean,";",concat(vmin,";",concat(vmax,";",vstd)));
      if (me==0) {
        if (cl['z']) {
          YA_MatD prow=concat(output_matrix,";",vstd);
          prow(3,":")=vmean-vstd;
          prow(4,":")+=vmean;
          prow(find(prow<0.0))=0.0;
          plot(vm_cast<double>::sc(vmcount(output_matrix.cols()+1)),
               transpose(concat(vmones(5),prow)),
               "Mean Residual Variance,Minimum,Maximum,-StdDev,+StdDev",'+');
        }
        if (cl.argstring(' ',1)!="NO_OUTPUT")
           save(cl.argstring(' ',1),output_matrix,YA_PRETTY_IO);
      }
    }
    tk.end();
    #ifdef YA_MPI
    MPI_Finalize();
    mtk.mpi_wend();
    #endif
    if (me==0) {
      cout << "\nExecution complete; total CPU run-time=" << tk << ".\n";
      #ifdef YA_MPI
      cout << "Wall time: " << mtk << ".\n\n"; 
      #endif
    }
    return 0;
  }

  // --------------------- Check if we only want isomap manifold stats
  if (cl['y']) {
    if (method!=ISOMAP)
    error.generate_error(0,"cml_drl","-y can only be used with IsoMap.");

    // ++++++++++ Get dimensions for statistics calculation
    YA_RowI nbors;
    YA_MatI temp_dim=cl.argstring('y',0);
    nbors=reshape(temp_dim,1,temp_dim.numel()); // Make row vector
    ipsort(nbors);
    if (maximum(nbors)>=input_matrix.rows())
      error.generate_error(0,"cml_drl",
               "Cannot specify more neighbors than data points!");
    YA_MatI stat_mat;
    YA_DynI stats;
    #ifdef YA_PROGMETER
    redmet->verbose(0);
    YA_TimeKeeper mtk;
    YA_ProgMeter pm;
    mtk.start();
    pm.start("Finding connected components:",70,nbors.numel(),false);
    #endif    
    for (ya_sizet i=0; i<nbors.numel(); i++) {
      reinterpret_cast<YAIsoReduce<double>*>(redmet)->
      component_stat(input_matrix,nbors(i),stats,landmarks);
      if (i==0)
        stat_mat=vmzeros(stats.numel(),nbors.numel());
      stat_mat(vmcount(stats.numel()),i)=stats;
      #ifdef YA_PROGMETER
      pm.iterate();
      #endif
    }
    #ifdef YA_PROGMETER
    redmet->verbose(2);
    pm.finish();
    mtk.end();
    cerr << "Done. " << mtk << ".\n";
    #endif

    if (cl['z']) {
      string legend;
      for (ya_sizet i=0; i<nbors.numel(); i++) {
        if (i!=0)
          legend+=',';
        legend+="k=";
        legend+=a::itoa(nbors(i));
      }
      if (stat_mat.rows()<20)
        plot(stat_mat,legend,'+',"Component Number","Component Size");
      else
        plot(stat_mat(vmcount(20),":"),legend,'+');
      YA_RowI sizes=sum(stat_mat>0);
      plot(nbors,sizes,"",'+',"k","Number of Components");
    }
    if (cl['o']) {
      ofstream out;
      a::fileopen(out,cl.argstring('o',1),error);
      out << "Neighbors " << size(nbors) << endl << nbors << endl
          << "Manifold_Nodes " << size(stat_mat) << endl << stat_mat << endl;
      a::fileclose(out,error);
    }
    delete redmet;
    tk.end();
    cout << "\nExecution complete; total run-time=" << tk << ".\n\n";
    return 0;
  }

  // ----------------------- Find a map and save to disk
  if (method==ISOMAP && landmarks.numel()>0)
    reinterpret_cast<YAIsoReduce<double>*>(redmet)->
                find_map(input_matrix,output_matrix,low_dim,eigopts,landmarks);
  else if (method==AE && cl['t'])
    reinterpret_cast<YAATEReduce<double>*>(redmet)->
      find(input_matrix,ate_test_matrix,output_matrix,low_dim,eigopts);
  else
    redmet->find(input_matrix,output_matrix,low_dim,eigopts);
 
  if (cl.argstring(' ',1)!="NO_OUTPUT")
    redmet->save_map(cl.argstring(' ',1));

  // ----------------------- Look at the low dimensional coords
  if (cl['o'])
    save(cl.argstring('o',0),output_matrix,YA_PRETTY_IO);
  if (cl['z'] && output_matrix.cols()>1) {
    if (colors!=NULL) {
      plot(colors, output_matrix);
      delete [] colors;
    } else
      plot(output_matrix(":",0),output_matrix(":",1),
       "First 2 Components",'o');
  }
  if (cl['w']) {
    ofstream pyout;
    a::fileopen(pyout,cl.argstring('w',0),error);
    pyout << "from pymol.cgo import *\n"
          << "from pymol import cmd\n"
          << "obj = [\n"
          << "BEGIN, POINTS," << endl;
    double alpha=1.0;
    for (ya_sizet i=0; i<output_matrix.rows(); i++) {
      pyout << "ALPHA," << alpha << ","
            << "COLOR,0,0,1,VERTEX,";
      for (ya_sizet k=0; k<3; k++)
        pyout << output_matrix(i,k) << ",";
      pyout << endl;
    }
    pyout << "END\n"
          << "]\n"
          << "cmd.load_cgo(obj,'" << "manifold" << "')\n\n";
    a::fileclose(pyout,error);
  }

  // ----------------------- Look at the statistics
  if (cl['o'])
  a::fileopen(out,cl.argstring('o',1),error);

  // ++++++++++ Eigen Values
  if (method!=AE) {
    if (cl['z'])
      plot(redmet->eigenvalues(),"Eigen Values",'+');
    if (cl['o'])
      out << "Eigen_Values " << size(redmet->eigenvalues()) << endl
          << redmet->eigenvalues() << endl;
  } else {
    if (cl['z']) {
      string rbm_layers="";
      for (ya_sizet i=1; i<layers.numel(); i++) {
        if (i!=1)
        rbm_layers+=',';
        rbm_layers+=a::itoa(layers(i));
      }
      plot(transpose(redmet->rbm_errors()),rbm_layers,'+');
      plot(transpose(redmet->bp_errors()),"bp_train,bp_test",'+',"AutoEncoder Back-Propagation Iteration","RMSD");
    }
    if (cl['o'])
      out << "RBMerror_Train_Test " << size(redmet->rbm_errors()) << endl
          << redmet->rbm_errors() << endl << "BPerror_Train_Test " 
          << size(redmet->bp_errors()) << endl << redmet->bp_errors() << endl;
    }

  // ++++++++++ Distance Residual
  if (stats==DISTANCE || (stats==ALL && method!=LLE && method!=AE)) {
    if (method==LLE || method==AE)
    error.addwarning(0,4,"cml_drl",
                     "Distance residual is meaningless for "+
                     redmet->method_id());
  YA_MatD distance_residual;
  YA_VecD corrcoeff;
  YA_VecD rsquared;
  if (method==MDS)
      dist_residual(input_matrix,output_matrix,
                    dimensions,corrcoeff,rsquared);
  else if (method==ISOMAP)
    if (landmarks.numel()>0)
      dist_residual(reinterpret_cast<YAIsoReduce<double>*>(redmet)->
                    graph_dists(),
                    output_matrix,dimensions,corrcoeff,rsquared,landmarks);
    else
      dist_residual(reinterpret_cast<YAIsoReduce<double>*>(redmet)->
                    graph_dists(),
                    output_matrix,dimensions,corrcoeff,rsquared);
  else
      dist_residual(YA_MatD(dist_mat(input_matrix)),output_matrix,
                    dimensions,corrcoeff,rsquared);
  if (cl['z'])
    plot(vm_cast<double>::sc(dimensions),concat(corrcoeff,rsquared),
       "Distance CorrCoeff,Distance R^2",'+');
  if (cl['o']) 
    out << "Dimensions_DistCorr_DistR2 " << dimensions.numel() << " 3\n"
      << concat(concat(vm_cast<double>::sc(dimensions),corrcoeff),rsquared)
          << endl;
  }

  // ++++++++++ Reconstruction error
  if (stats==RECONSTRUCT || (stats==ALL && (method==PCA || method==AE))) {
    if (method!=PCA && method!=AE)
    error.generate_error(0,"cml_drl",
      "Reconstruction error not implemented for this method.");
    YA_VecD rmsdv;
    if (method==PCA) {
    reinterpret_cast<YAPCAReduce<double> *>(redmet)->
      reconstruct_error(input_matrix,output_matrix,dimensions,rmsdv);
    if (cl['z'])
       plot(vm_cast<double>::sc(dimensions),rmsdv,"Reconstruction RMSD",'+');
    if (cl['o'])
      out << "Reconstruction_RMSD " << dimensions.numel() << " 2\n"
          << concat(vm_cast<double>::sc(dimensions),rmsdv) << endl;
    } else {
     YA_MatD reverse_in;
     redmet->reverse(output_matrix,reverse_in);
     rmsdv=VM_ScalarD(rmsd(input_matrix,reverse_in));
     cout << "Reconstruction error (RMSD): " << rmsdv;
     if (method == AE)
     {
       cout << " (Computed over entire input matrix, not just the AE test set)";
     }
     cout << endl;
     if (cl['o'])
       out << "Reconstruction_RMSD 1 2\n" << redmet->low_dim() << rmsdv 
       << endl;
    }
  }

  if (cl['o'])
  a::fileclose(out,error);
  delete redmet;

  tk.end();
  cout << "\nExecution complete; total run-time=" << tk << ".\n\n";

  return 0;
}

void Describe(CommandLine &cl,ostream &out) {
  string name=cl.program_name();
  string progname=a::strcenter(name,70);
  string gridversion=a::strcenter("Dr. L. Version "+string(DRL_VER),70);
  out << endl << progname << endl << gridversion << endl
      << "______________________________________________________________________\n"
      << a::strcenter(AUTHORS,70) << endl
      << "______________________________________________________________________\n"
      << "Perform dimensionality reduction on a set of coordinates\n\n";
  cl.write_text_synopsis(out,70);
  out  << "Use '" << name << " -h > " << name
      << ".1' to generate a man page for this\n"
      << "program and type 'man ./" << name << ".1' for help\n"
      << "______________________________________________________________________\n";
  return;
}

void HandleArgs(CommandLine &cl, int argc, char *argv[], Error *error, 
                EigenOptions &eigopts) {
  // Arguments
  cl.addmanditory(' ',2);
  cl.addargname(' ',"input_matrix");
  cl.addargname(' ',"output_file");

  cl.add('d',1);
  cl.addargname('d',"embedding_dimensionality");
  cl.adddescription('d',"Specify the number of dimension to project down to. The default is 2. For the AutoEncoder, all layers are specified in a string (-d \"3 8 4 2\"");
  cl.add('m',1);
  cl.addargname('m',"reduction_method");
  cl.adddescription('m',"Specify the dimensionality reduction method to be used. Options are PCA, MDS, LLE, ISOMAP, and AE. The default is PCA.");
  cl.add('p',1);
  cl.addargname('p',"epsilon_distance");
  cl.adddescription('p',"Specify the epsilon distance for neighbors to be used for ISOMAP method.");
  cl.add('k',1);
  cl.addargname('k',"num_neighbors");
  cl.adddescription('k',"Specify the number of neighbors to be used for methods which require k-nearest neighbors. The default is 10. If k<1, then we use it as epsilon distance for neighbors.");
  cl.add('e',1);
  cl.addargname('e',"eigentype");
  cl.adddescription('e',"Specify the type of eigen decomposition. Options are "+eigopts.methods()+". The default is "+eigopts.method_name(eigopts.method()));
  cl.add('c',1);
  cl.addargname('c',"converge_num");
  cl.adddescription('c',"Specify convergence criteria for power method. This is given in terms of the cosine of the angle between eigen vectors in successive iterations. The default is "+a::ftoa(eigopts.tol())+" which results in the default epsilon depending on the matrix and method.");
  cl.add('i',1);
  cl.addargname('i',"max_iters");
  cl.adddescription('i',"Specify the maximum number of iterations for iterative methods on each eigen vector. A warning is generated if any eigen vector does not converge within the specified number of iterations. The default is "+a::itoa(eigopts.max_iters())+".");
  cl.add('s',1);
  cl.addargname('s',"stat_calculation");
  cl.adddescription('s',"Perform calculations on the statistics for a reduction. Options are NONE, DISTANCE, RECONSTRUCT and ALL. The default is NONE. The dimensions for which statistics are calculated is specified with -x");
  cl.add('f',1);
  cl.addargname('f',"map_file");
  cl.adddescription('f',"Map from the high dimensional to the low dimensional space or vice versa using a previously calculated reduction as specified in map_file. The input_matrix can contain multiple points, but the number of columns must match either the high or low dimensionality. For LLE and ISOMAP, the forward and reverse maps are calculated using an inverse distance weighted average of coordinates from neighbors of the high and low dimensionalities used in the original reduction. The number of neighbors used for the mapping is specified with the -k option. Reconstruction errors for the mapping can be calculated using the -r option.");
  cl.add('x',1);
  cl.addargname('x',"dimensions");
  cl.adddescription('x',"Dimensions for which statistics should be calculated in matrix format. Examples are: -x \"[0:3]\" -x \"[0:2:4]\" -x \"[0 1 5]\". For eigenvalues, all dimensions are currently output regardless of the dimensions specified. Additionally, the dimensions specified with -x currently have no affect on autoencoder statistics.");
  cl.add('y',1);
  cl.addargname('y',"neighbor_string");
  cl.adddescription('y',"Do not do the reduction, but instead just generate statistics on the number of connected manifolds. Statistics are generated for each number of neighbors in neighbor_string (specified in the same manner as for -x).");
  cl.add('z',0);
  cl.adddescription('z',"Make plots with gnu plot.");
  cl.add('o',2);
  cl.addargname('o',"low_dim_file");
  cl.addargname('o',"stats_file");
  cl.adddescription('o',"Output the coordinates in the low dimensional space from the reduction to low_dim_file in text format. Statistics are output to the file stats_file, also in text format.");
  cl.add('a',5);
  cl.addargname('a',"train_test_split");
  cl.addargname('a',"rbm_iters");
  cl.addargname('a',"bp_iters");
  cl.addargname('a',"rbm_size");
  cl.addargname('a',"bp_size");
  cl.adddescription('a',"Set parameters for the autoencoder. train_test_split is the fraction of input data to be used for training (default is 0.8). rbm_iters (default=10) and bp_iters (default=30) are the number of iterations to be used for restricted boltzmann and back propagation steps. rbm_size (default=10) and bp_size (default=100) is the desired batch size for RBM and BP steps. rbm_size and bp_size should evenly divide the training set size.");
  cl.add('t',1);
  cl.addargname('t',"test_file");
  cl.adddescription('t',"Specify explicitly the test set for autoencoder training. In this case no randomization of data occurs.");
  cl.add('r',0);
  cl.adddescription('r',"Calculate the reconstruction error for the mapping specified with -f. If used with -x, the reconstruction error is calculated for each of the specified dimensions (note that the -x option cannot be used with the autoencoder). If this option is used during learning, the reconstruction error is calculated from the input_matrix. For the autoencoder, this is the training set and the test set if the -t option is not specified. If the -t option is specified, it is the reconstruction error for the data used for training. For LLE and Isomap, the reconstruction error for the training data will always be 0 due to the mapping algorithm. Therefore it is advised to use a separate test set with these algorithms.");
  cl.add('l',2);
  cl.addargname('l',"landmark_type");
  cl.addargname('l',"landmark_value");
  cl.adddescription('l',"Use landmark points for IsoMap reduction. If landmark_type is FILE, landmark_value specifies a file to load a matrix from (see file formats below). If landmark_type is STRING, a string specification of landmarks is used (see -x for examples). If landmark_type is RANDOM, a landmark value is a fraction>0 and <1 that specifies the random fraction of landmarks to be used. Landmarks are specified with 0 as the first point.");
  cl.add('b',1);
  cl.addargname('b',"random_seed");
  cl.adddescription('b',"Set the random number seed (ya_sizet integer value).");  
  cl.add('g',0);
  cl.adddescription('g',"If linked to the Intel math library, this will use the faster, low accuracy math functions.");
  cl.add('w',1);
  cl.addargname('w',"pymol_output");
  cl.adddescription('w',"Test output for pymol.");
  cl.add('v',0);
  cl.adddescription('v',"For MDS, LLE, and IsoMap, the forward and reverse maps are obtained from a weighted average of the neighbors. By default, the weights are obtained from the inverse of the distances to the neighbors. If the -v flag is set, weights are obtained by solving a least-squares problem as is perfromed in LLE.");
  cl.add('u',0);
  cl.adddescription('u',"Perform calculations to estimate the intrinsic dimensionality using a variant of local PCA. In this case, PCA is performed on the manifold around each point using the nearest neighbors and the residual varience around each point is calculated. Statistics on the residual variance for each dimension are written to the output file. The top line is the mean residual variance. The next line is the minimum, followed by the maximum and the standard deviation for each dimension. If a single dimension is specified, either with -d or -x the output file consists of the residual variance around each datapoint. The time required for calculation will not change, even if fewer dimensions are specified. If -z is specified, the output_file is plotted.");
  
  // Stuff for every executable
  cl.addhelp('h',0);
  cl.adddescription('h',"Print out the man page for help");
  cl.add('n',1);
  cl.addargname('n',"notice_level");
  cl.adddescription('n',"Set the degree of program output.  Use: \n\n\t-n  0\tNo output\n\t-n 10\tNormal program output\n\t-n 20\tParameters useful for reproducing the results\n\t-n 30\tAll output");

  // Short Description
  cl.addtoman_chapter("NAME","Dimensionality Reduction");

  // Version
  cl.addtoman_chapter("VERSION","Version "+string(DRL_VER));

  // Full Description
  const string desc[22]={
    "Perform dimensionality reduction on a set of coordinates using ",
   "principal component analysis (PCA), multidimensional scaling (MDS) ",
  "locally linear embedding (LLE), IsoMap, or an AutoEncoder (AE). ",
  "The input file consis is a matrix (see matrix file formats below) with ",
  "each row cooresponding to a data point. For MDS reduction, the input ",
  "file is a symmetric distance matrix. An internal test matrix ",
  "representing 400 samples from the 'swiss roll' can be used by specifying ",
  "INT_SWISSROLL as the input file name. 5000 samples of the swissroll can ",
  "be specified with INT_SWISSROLL5. Other internal test matrices are: ",
  "INT_SWISSROLLN for textbook version of swiss roll, INT_THINSWISSROLL for ",
  "thin swiss roll, INT_JAPAROLL for Japaness flag roll, INT_NONDEVROLL for ",
  "Nondevelopable swiss roll, INT_TKNOT for trefoil knot, INT_SPHERE for ",
  "a sphere, INT_TORUS for torus, and INT_BOX for the open box. ", 
  "(Add 5 at the end of string for 5000 samples). The output file uses an ",
  "internal format that stores the map from the high-dimensional ",
  "to the low-dimensional space and vice-versa. Analysis can be ",
  "performed without writing the map to disk by specifying ",
  "NO_OUTPUT as the output file name. Once a map has been ",
  "obtained, coordinates can be mapped back and forth using the ",
  "-f flag. In this case, the input_file contains the coordinates ",
  "in one dimension and the output_file contains the mapped ",
  "coordinates."
  };
  cl.addtoman_chapter("DESCRIPTION",22,desc);

  ya_addftmanpage(cl);

  // Authors
  cl.addtoman_chapter("AUTHORS",AUTHORS);

  // Parse the commandline
  if (!cl.parse(argc,argv,error)) {
    Describe(cl,cout);
    error->generate_error(0,a::filenameonly(argv[0]),"Bad Command Line\n");
  }

  // Set the notice level
  if (cl['n'])
    error->note.set_notice_level(cl.argint('n',0));

  // Generate a notice with the command line for records purposes
  string cm=cl.program_name();
  for (int j=1; j<argc; j++)
    cm+=' '+string(argv[j]);
  cm+="\n";
  error->note.notice(19,"CommandLine",cm);

  // Output the help
  if (cl['h']) {
    cl.write_man_page(cout,DRL_VER,"Dr. L. Utilities");
    exit(0);
  }
}

// Determine the type of map for a previously saved reduction
string determine_map_type(const string filename, Error &error) {
  ifstream in;
  a::fileopen(in,filename,error);
  string type;
  in >> type;
  if (type=="PCA" || type=="MDS" || type=="LLE" || type=="ISOMAP" ||
    type=="AE")
    return type;
  error.generate_error(0,"cml_drl","Unknown type: "+type+
                       "specified in map file: "+filename);
  return "";
}
