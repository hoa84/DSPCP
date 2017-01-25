/**************************************************************************
                                vm_slice.cpp
                             -------------------

    Matrix Slicing

 __________________________________________________________________________
    This file is part of the Math Utilities package for command-line
    access to Yala++ library functions
__________________________________________________________________________

    begin      : Fri Feb 9 2007
    authors    : W. Michael Brown
    email      : wmbrown@sandia.gov
 ***************************************************************************/

/* -----------------------------------------------------------------------
   Copyright (2009) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under 
   the GNU General Public License.
   ----------------------------------------------------------------------- */

#include "vm.h"
#include "commandline.h"
#include "base/ya_commandline.h"

// Describe the program parameters
void Describe(CommandLine &cl,ostream &out);
// Parse the command line parameters
void HandleArgs(CommandLine &, int, char **, Error *);

int main(int argc, char *argv[]) {
  CommandLine cl;
  MathRandom<MathMersenneTwister> rng;
  Error error;
  ya_check_debug();

  YA_MatD input_matrix;
  YA_MatD output_matrix;

  // Parse the command line
  HandleArgs(cl,argc,argv,&error);

  string outputfile="";
  if (cl.argsize(' ')>0) {
    load(cl.argstring(' ',0),input_matrix);
    if (cl.argsize(' ')>1)
      outputfile=cl.argstring(' ',1);
  } else
    read(cin,input_matrix);

  // Select rows
  if (cl['r']) {
    output_matrix=input_matrix(YA_RowI(cl.argstring('r',0)),":");
    input_matrix=output_matrix;
  }
  
  // Select cols
  if (cl['c']) {
    output_matrix=input_matrix(":",YA_RowI(cl.argstring('c',0)));
    input_matrix=output_matrix;
  }

  // Reorder rows using modulus
  else if (cl['z']) {
    ya_sizet mod=cl.argint('z',0);
    if (mod==0)
      error.generate_error(0,"vm_slice","Cannot specify a mod_num of 0.");
    if (input_matrix.rows()%mod!=0) {
      error.buffer() << "When using -z, the number of rows in the matrix "
                     << "must be evenly divisible by the mod_num.";
      error.addbuf(0,"vm_slice");
    }
    YA_VecI row_order(input_matrix.rows());
    ya_sizet offset=input_matrix.rows()/mod;
    for (ya_sizet i=0; i<input_matrix.rows(); i++) {
      div_t index=div(int(i),int(mod));
      row_order(i)=index.quot+index.rem*offset;
    }
    output_matrix=input_matrix(row_order,":");
  } else    
    output_matrix=input_matrix;

  ya_sizet file_format=YA_DEFAULT_IO;
  if (cl['t'])
    file_format=YA_PRETTY_IO;
  if (cl['b'])
    file_format=YA_BINARY_IO;

  // Random subset
  if (cl['s']) {
    double percent=cl.argdouble('s',0);
    if (percent>1)
      error.generate_error(0,"mat_convert",
        "Random percentage must be between 0 and 1");
    YA_RowI rand_perm(randperm(output_matrix.rows(),rng));
    output_matrix=copy(output_matrix(rand_perm,":"));
    ya_sizet cut_frac=ya_sizet(percent*output_matrix.rows());
    if (cl.argstring('s',1)!="NO_OUTPUT")
      save(cl.argstring('s',1),
           output_matrix(vmcount(cut_frac,":",output_matrix.rows()-1),":"),
           file_format);
    output_matrix=copy(output_matrix(vmcount(cut_frac),":"));
  }

  if (cl['q'])
    ip_transpose(output_matrix);

  if (outputfile=="")
    write(cout,output_matrix,file_format);
  else
    save(outputfile,output_matrix,file_format);
  return 0;
}

void Describe(CommandLine &cl,ostream &out) {
  string name=cl.program_name();
  string progname=a::strcenter(name,70);
  string gridversion=a::strcenter("Yala++ Library Version "+
                     string(YALA_VERSION),70);
  out << endl << progname << endl << gridversion << endl
      << "______________________________________________________________________\n"
      << a::strcenter("W. Michael Brown",70) << endl
      << "______________________________________________________________________\n"
      << "Perform matrix slicing operations\n\n";
  cl.write_text_synopsis(out,70);
  out  << "Use '" << name << " -h > " << name
      << ".1' to generate a man page for this\n"
      << "program and type 'man ./" << name << ".1' for help\n"
      << "______________________________________________________________________\n";
  return;
}

void HandleArgs(CommandLine &cl, int argc, char *argv[], Error *error) {
  // Arguments
  cl.add(' ',2,0);
  cl.addargname(' ',"input_matrix");
  cl.addargname(' ',"output_matrix");

  cl.add('r',1);
  cl.addargname('r',"rows");
  cl.adddescription('r',"Only output a selected set rows specified in the string.");
  cl.add('c',1);
  cl.addargname('c',"columns");
  cl.adddescription('c',"Only output a selected set of columns specified in the string.");
  cl.add('z',1);
  cl.addargname('z',"mod_num");
  cl.adddescription('z',"Reorder the matrix rows using the supplied modulus number. For a mod_num of 3, the first row in the new matrix is 0, followed by 3, 6, 9, ..., 1, 4, 7, ...");
  cl.add('s',2);
  cl.addargname('s',"rand_percent");
  cl.addargname('s',"rest_file");
  cl.adddescription('s',"Output a random subset of rows. rand_percent is the fraction of the matrix to output and must be between 0 and 1. The random selection is taken after any other slicing. If rest_file is not NO_OUTPUT, the remaining slice is output to the specified filename.");
  cl.add('t',0);
  cl.adddescription('t',"Output the matix in 'pretty' format.");
  cl.add('b',0);
  cl.adddescription('b',"Output the matix in binary format.");
  cl.add('q',0);
  cl.adddescription('q',"Transpose the matrix.");

  // Stuff for every executable
  cl.addhelp('h',0);
  cl.adddescription('h',"Print out the man page for help");
  cl.add('n',1);
  cl.addargname('n',"notice_level");
  cl.adddescription('n',"Set the degree of program output.  Use: \n\n\t-n  0\tNo output\n\t-n 10\tNormal program output\n\t-n 20\tParameters useful for reproducing the results\n\t-n 30\tAll output");

  // Short Description
  cl.addtoman_chapter("NAME","Perform matrix slicing");

  // Version
  cl.addtoman_chapter("VERSION","Version "+string(YALA_VERSION));

  // Full Description
  const string desc[4]={
    "Perform matrix slicing. The input file and output file are optional. ",
    "If only one is specified, it is assumed to be the input file. ",
    "If the input file or output file not specified, STDIN or STDOUT ",
    "is used to allow for piping."
  };
  cl.addtoman_chapter("DESCRIPTION",4,desc);

  ya_addftmanpage(cl);

  // Authors
  cl.addtoman_chapter("AUTHORS","W. Michael Brown");

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
    cl.write_man_page(cout,YALA_VERSION,"YALA Utilities");
    exit(0);
  }
}

