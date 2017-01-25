/**************************************************************************
                                  mdiff.cpp
                             -------------------

    Numerical diff for regression testing

  __________________________________________________________________________
    Miscellaneous Utilities
  __________________________________________________________________________

    begin      : Thu Feb 14 2008
    authors    : W. Michael Brown
    email      : wmbrown@sandia.gov
 ***************************************************************************/

/* -----------------------------------------------------------------------
   Copyright (2009) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under 
   the GNU General Public License.
   ----------------------------------------------------------------------- */

#include "misc.h"
#include "commandline.h"
#include "error.h"

// Describe the program parameters
void Describe(CommandLine &cl,ostream &out);
// Parse the command line parameters
void HandleArgs(CommandLine &, int, char **, Error *);

int main(int argc, char *argv[]) {
  CommandLine cl;
  Error error;
  // Parse the command line
  HandleArgs(cl,argc,argv,&error);
  
  ifstream in1, in2;
  in1.open(cl.argstring(' ',0).c_str());
  in2.open(cl.argstring(' ',1).c_str());
  if (!in1 || !in2) {
    cout << "ERROR " << cl.argstring(' ',0) << " " << cl.argstring(' ',1) << " "
         << "Files Does Not Exist\n";
    return 0;
  }
  
  bool ok=true;
  double max_error=0;
  double mean_error=0;
  unsigned number_tokens=0;
  unsigned sign_changes=0;
  double precision=0.1;
  if (cl['p'])
    precision=cl.argdouble('p',0);
  
  while (true) {
    string token1,token2;
    in1 >> token1;
    in2 >> token2;
    
    if (in1.eof() || !in1) {
      if (in2.eof() || !in2)
        break;
      ok=false;
      break;
    }
    
    if (in2.eof() || !in2) {
      ok=false;
      break;
    }
    
    if (token1==token2)
      continue;
      
    istringstream sin1, sin2;
    double num1, num2;
    sin1.str(token1);
    sin2.str(token2);
    sin1 >> num1;
    sin2 >> num2;
    if (!sin1 || !sin2) {
      ok=false;
      break;
    }
    double errorv;
    double denr=num2;
    if (num1==0 || num2==0)
      denr=1;
    if (cl['s']) {
      errorv=fabs((fabs(num1)-fabs(num2))/denr);
      if ( (num1<0 && num2>0) || (num1>0 && num2<0) )
        sign_changes++;
    } else
      errorv=fabs((num1-num2)/denr);
    max_error=max(errorv,max_error);
    mean_error+=errorv;
    number_tokens++;
  }
  
  if (ok==false) {
    cout << "ERROR " << cl.argstring(' ',0) << " " << cl.argstring(' ',1) << " "
         << "Files Differ\n";
    return 0;
  }
  
  if (number_tokens>0)
    mean_error/=number_tokens;
    
  if (max_error<precision)
    cout << "OK ";
  else
    cout << "ERROR ";
  
  cout << cl.argstring(' ',0) << " " << cl.argstring(' ',1) << " MAX: "
       << max_error << " MEAN: " << mean_error;

  if (cl['s'])
    cout << " SIGNS: " << sign_changes;
    
  cout << endl;

  return 0;
}

void Describe(CommandLine &cl,ostream &out) {
  string name=cl.program_name();
  string progname=a::strcenter(name,70);
  out << endl << progname << endl
      << "______________________________________________________________________\n"
      << a::strcenter("W. Michael Brown",70) << endl
      << a::strcenter("2/9/2007",70) << endl
      << "______________________________________________________________________\n"
      << "Perform diffs for regression testing\n\n"
      << cl.format_synopsis("","","") << endl << endl
      << "Use '" << name << " -h > " << name
      << ".1' to generate a man page for this\n"
      << "program and type 'man ./" << name << ".1' for help\n"
      << "______________________________________________________________________\n";
  return;
}

void HandleArgs(CommandLine &cl, int argc, char *argv[], Error *error) {
  // Arguments
  cl.add(' ',2,0);
  cl.addargname(' ',"input_file");
  cl.addargname(' ',"correct_file");

  cl.add('p',1);
  cl.addargname('p',"precision");
  cl.adddescription('p',"If the maximum relative error between a number in input_file and correct_file is greater than precision, mdiff reports an error.");
  cl.add('s',0);
  cl.adddescription('s',"Ignore sign changes, but report the number of sign changes.");

  // Stuff for every executable
  cl.addhelp('h',0);
  cl.adddescription('h',"Print out the man page for help");
  cl.add('n',1);
  cl.addargname('n',"notice_level");
  cl.adddescription('n',"Set the degree of program output.  Use: \n\n\t-n  0\tNo output\n\t-n 10\tNormal program output\n\t-n 20\tParameters useful for reproducing the results\n\t-n 30\tAll output");

  // Short Description
  cl.addtoman_chapter("NAME","Numerical diff for regression testing");

  // Version
  cl.addtoman_chapter("VERSION","Version 0.1");

  // Full Description
  const string desc[9]={
    "Compares two files. If the files contain a different number of whitespace ",
    "delimited tokens, 'ERROR input_file1 input_file2 Files Differ' is output. ",
    "If one of the files does not exist, 'File Does Not Exist' is output ",
    "instead of 'Files Differ'. ",
    "If the maximum relative error between the two files is below 0.01 ",
    "or the value specified with -p, OK is output along with the ",
    "maximum relative error, the mean relative error, and (optionally) ",
    "the number of sign changes. Otherwise ERROR is output instead of OK.",
    "If one of the values is zero, the absolute error is used."
  };
  cl.addtoman_chapter("DESCRIPTION",9,desc);

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
    cl.write_man_page(cout,"0.1","Math Utilities");
    exit(0);
  }
}

