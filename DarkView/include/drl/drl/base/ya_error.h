/***************************************************************************
                                  ya_error.h
                             -------------------
                               W. Michael Brown

  Error helper functions for YALA++

 __________________________________________________________________________
    This file is part of the YALA++ Library
 __________________________________________________________________________

    begin      : Sat Mar 7 2009
    authors    : W. Michael Brown
    email      : wmbrown@sandia.gov
 ***************************************************************************/

/* -----------------------------------------------------------------------
   Copyright (2009) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under 
   the GNU General Public License.
   ----------------------------------------------------------------------- */

#ifndef YA_ERROR_H
#define YA_ERROR_H

#include <iomanip>

YALANAMESPACE

#ifndef YA_MAX_ERROR
#define YA_MAX_ERROR 9
#endif

#ifndef YA_MIN_ERROR
#define YA_MIN_ERROR 2
#endif

#ifndef YA_MAX_NOTE
#define YA_MAX_NOTE 10
#endif

#define YA_ERROR_WIDTH 70

void ya_writeline() {
  cerr << "+";
  for (unsigned i=0; i<YA_ERROR_WIDTH-2; i++)
    cerr << "-";
  cerr << "+\n";
}

void ya_addwarn(int ID, int level, const string calling_class,
                const string warning) {
  if (level<YA_MIN_ERROR)
    return;
  
  cerr << "\n";
  ya_writeline();
  cerr.setf(ios::left);
  cerr.unsetf(ios::right);

  // Output the IDs
  unsigned width12=(unsigned)floor((double)(YA_ERROR_WIDTH-10)/3.0);
  unsigned width3=YA_ERROR_WIDTH-10-width12-width12;
  string et;
  unsigned width1;
  if (level>YA_MAX_ERROR) {
    et="| Error: "; width1=width12-7;
  } else {
    et="| Warning: "; width1=width12-9;
  }
  cerr << et << setw(width1) << ID << " | Level: "
       << setw(width12-7) << level << " | " << setw(width3)
       << calling_class << " |\n";
  ya_writeline();

  // Output the message
  vector <string> messages;
  ya_format_fit(YA_ERROR_WIDTH-3,warning,messages);
  for (unsigned i=0; i<messages.size(); i++)
    cerr << "| " << setw(YA_ERROR_WIDTH-3) << messages[i] << "|\n";
  ya_writeline();
  if (level>YA_MAX_ERROR)
    exit(1);
}

}
#endif
