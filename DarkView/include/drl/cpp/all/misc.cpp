/***************************************************************************
                                    misc.cpp
                             -------------------
                               W. Michael Brown

  Miscellaneous functions that do not deserve their own class

  __________________________________________________________________________
    Miscellaneous Utilities
  __________________________________________________________________________

    begin      : May 30 2003
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

#include <cctype>
#include <cstdlib>

void a::copyright(ostream &out, unsigned year) {
  out << "Copyright (" << year << ") Sandia Corporation. Under the terms of "
      << "Contract\nDE-AC04-94AL85000, there is a non-exclusive license for "
      << "use of this\nwork by or on behalf of the U.S. Government. Export "
      << "of this program\nmay require a license from the United States "
      << "Government.\n";
}

// Get the SNL Copyright info as a string
string a::copyrightstring(unsigned year) {
  return string("Copyright (")+a::itoa(year)+") Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive license for use of this work by or on behalf of the U.S. Government. Export of this program may require a license from the United States Government.";
}

void a::fileopen(ifstream &in, const char *filename, Error &error) {
  in.clear();
  in.open(filename);
  if (!in) {
    error.addwarning(1,15,"Misc",
                     "Could not open "+string(filename)+" for input!");
    error.writewarnings();
  }
}

void a::fileopen(ifstream &in, const string &filename, Error &error) {
  in.clear();
  in.open(filename.c_str());
  if (!in) {
    error.addwarning(1,15,"Misc",
                     "Could not open "+string(filename)+" for input!");
    error.writewarnings();
  }
}

void a::fileopenbinary(ifstream &in, const string &filename, Error &error) {
  in.clear();
  in.open(filename.c_str(), ios::binary);
  if (!in) {
    error.addwarning(1,15,"Misc",
                     "Could not open "+string(filename)+" for input!");
    error.writewarnings();
  }
}

void a::fileopen(ofstream &out, const char *filename, Error &error) {
  out.clear();
  out.open(filename);
  if (!out) {
    error.addwarning(2,15,"Misc",
                     "Could not open "+string(filename)+" for output!");
    error.writewarnings();
  }
}

void a::fileopen(ofstream &out, const string &filename, Error &error) {
  out.clear();
  out.open(filename.c_str());
  if (!out) {
    error.addwarning(2,15,"Misc",
                     "Could not open "+string(filename)+" for output!");
    error.writewarnings();
  }
}

void a::fileopenbinary(ofstream &out, const string &filename, Error &error) {
  out.clear();
  out.open(filename.c_str(),ios::binary);
  if (!out) {
    error.addwarning(2,15,"Misc",
                     "Could not open "+string(filename)+" for output!");
    error.writewarnings();
  }
}

void a::fileopenapp(ofstream &out, const string &filename, Error &error) {
  out.clear();
  out.open(filename.c_str(),ios::app);
  if (!out) {
    error.addwarning(2,15,"Misc",
                     "Could not open "+string(filename)+" for output!");
    error.writewarnings();
  }
}

void a::fileclose(ifstream &in, Error &error) {
  in.close();
}

void a::fileclose(ofstream &out, Error &error) {
  if (out.fail()) {
    error.addwarning(10,15,"Misc","Error writing to output file!");
    error.writewarnings();
  }
  out.close();
}

// Put a string back into an istream
void a::putback(istream &in, const string &s) {
  if (s.length()==0)
    return;
  unsigned i=unsigned(s.length())-1;
  while (true) {
    in.putback(s[i]);
    if (i==0)
      return;
    i--;
  }
}

string a::date() {
  char datestr[40];

  time_t t;
  time(&t);

  strftime(datestr,40,"%B %d, %Y",localtime(&t));
  return string(datestr);
}

// Return the filename without the extension
string a::namewoext(const string &filename) {
  return (filename.substr(0,filename.find_last_of('.')));
}

// Return the filename without extension or directory
string a::filenameonly(const string &filename) {
  // Find the start of the filename
  size_t start=0;
  size_t last_s=filename.find_last_of('/');
   if (last_s<(filename.length()-1))
     start=last_s+1;
   last_s=filename.find_last_of('\\');
   if (last_s<(filename.length()-1))
     start=max(start,last_s);

  // Find the end of the filename
  size_t end=filename.find_last_of('.')-start;
  return(filename.substr(start,end));
}

// Return the extension of a filename
string a::extension(const string &filename) {
  // Find the start of the extension
  size_t start=filename.find_last_of('.')+1;
  if (start>=filename.length())
    return "";

  return filename.substr(start,filename.length()-start);
}

// Center a string over the specified length
string a::strcenter(const string &s, unsigned length) {
  string empty("                                                           ");
  unsigned half=length/2;
  unsigned spacer=half-(s.length()/2)-1;
  return (empty.substr(0,spacer)+s);
}

// True if a character is whitespace
bool a::whitespace(char c) {
  if (c==' ' || c=='\n' || c=='\t' || c=='\f' || c=='\r' || c=='\v')
    return true;
  return false;
}

// Check if a string is only whitespace
bool a::whitespace(const string &s) {
  for (unsigned i=0; i<s.length(); i++)
    if (!whitespace(s[i]))
      return false;
  return true;
}

/// Remove all whitespace from a string
string a::remove_whitespace(const string &s) {
  string n;
  for (unsigned i=0; i<s.length(); i++)
    if (!whitespace(s[i]))
      n+=s[i];
  return n;
}

void a::str_replace(const string &source, const string &target, string &s) {
  size_t slength=source.length();
  size_t tlength=target.length();
  size_t loc=0;

  #ifdef DEBUG
  assert(slength>0 && tlength>0);
  #endif
  
  while (true) {
    if (loc>=s.length())
      break;
    loc=s.find(source,loc);
    if (loc>=s.length())
      break;
    s.replace(loc,slength,target,0,tlength);
    loc+=tlength;
  }
}

// Replace any instance of \e source with \e target within the string \e s
void a::str_replace(const char source, const char target, string &s) {
  for (unsigned i=0; i<s.length(); i++)
    if (s[i]==source)
      s[i]=target;
  return;
}

// Convert all alpha characters to lower case
string a::tolower(const string &s) {
  string r="";
  for (unsigned i=0; i<s.length(); i++)
    r+=std::tolower(s[i]);
  return r;
}

// Return a string of num underscores
string a::underline(unsigned num) {
  return string(num,'_');
}

// The tokens parsed from cstring are \e added to the input vector
void a::get_tokens(const char *line, vector<string> &tokens) {
  string sline=line;
  get_tokens(sline,tokens);
}

void a::get_tokens(const string &sline, vector<string> &tokens) {
  string token="";
  unsigned i=0;
  while (i<sline.length()) {
    if (whitespace(sline[i])) {
      i++;
      continue;
    }
    while (i<sline.length()) {
      if (whitespace(sline[i])) {
        tokens.push_back(token);
        token="";
        break;
      }
      token+=sline[i];
      i++;
    }
  }
  if (token!="")
    tokens.push_back(token);
}

// Return the first token in a string
string a::get_first_token(const char *line) {
  string sline=line;
  string token="";
  unsigned i=0;
  while (i<sline.length()) {
    if (whitespace(sline[i])) {
      i++;
      continue;
    }
    while (i<sline.length()) {
      if (whitespace(sline[i])) 
        return token;
      token+=sline[i];
      i++;
    }
  }
  return token;
}  
          
void a::get_tokens(char delimiter, const string &sline,vector<string> &tokens) {
  string token="";
  unsigned i=0;
  while (i<sline.length()) {
    if (delimiter==sline[i]) {
      tokens.push_back(token);
      token="";
    } else
      token+=sline[i];
    i++;
  }
  if (token!="")
    tokens.push_back(token);
}

// Format a string to fit within a specified column width
void a::format_fit(unsigned column_width, const string &input,
                   vector<string> &output) {
  vector<string> forced;
  a::get_tokens('\n',input,forced);
  for (unsigned i=0; i<forced.size(); i++) {
    string current_line;
    vector<string> tokens;
    a::get_tokens(forced[i],tokens);
    for (unsigned j=0; j<tokens.size(); j++) {
      if (current_line.length()+tokens[j].length()<column_width) {
        current_line+=tokens[j]+' ';
      } else {
        if (tokens[j].length()+1>column_width) {
          unsigned this_count=column_width-current_line.length();
          current_line+=tokens[j].substr(0,this_count);
          output.push_back(current_line);
          current_line="";
          tokens[j]=tokens[j].substr(this_count,tokens[j].length()-this_count);
          j--;
        } else {
          output.push_back(current_line);
          current_line=tokens[j]+' ';
        }
      }
    }
    output.push_back(current_line);
  }
  return;
}

string a::itoa(unsigned i) {
  ostringstream o;
  o << i;
  return o.str();
}

string a::itoa(int i) {
  ostringstream o;
  o << i;
  return o.str();
}

string a::itoa(long i) {
  ostringstream o;
  o << i;
  return o.str();
}

string a::ftoa(double i) {
  ostringstream o;
  o << i;
  return o.str();
}

// Seed the random number generator
void a::seedrandom(unsigned seed) {
#ifdef _MSC_VER
  srand(seed);
#else
  srandom(seed);
#endif
}

// Seed the random number generator with the current time
void a::seedrandom_time() {
#ifdef _MSC_VER
  srand(unsigned(time(0)));
#else
  srandom(unsigned(time(0))); //+getpid());
#endif
}

// Return an integer between 0 and max
unsigned a::irandom(unsigned max) {
#ifdef _MSC_VER
  return unsigned(double(rand())/double(RAND_MAX)*max);
#else
  return unsigned(double(random())/double(RAND_MAX)*max);
#endif
}

// Return an integer between 0 and max
long a::lrandom(long max) {
#ifdef _MSC_VER
  return long(double(rand())/double(RAND_MAX)*max);
#else
  return long(double(random())/double(RAND_MAX)*max);
#endif
}

double a::frandom(double max) {
#ifdef _MSC_VER
  return (double(rand())/double(RAND_MAX)*max);
#else
  return (double(random())/double(RAND_MAX)*max);
#endif
}

// Empty constructer with no header, no extension, no lead zeros
FileIterator::FileIterator() {
  file_num=0;
  digits=0;
  header="";
  extension="";
}

// Specify the filename format with leading zeros
FileIterator::FileIterator(const string &h,const string &e,unsigned d) {
  file_num=0;
  digits=d;
  header=h;
  extension=e;
}

// Specify the filename format without leading zeros
FileIterator::FileIterator(const string &h,const string &e) {
  digits=0;
  file_num=0;
  header=h;
  extension=e;
}

// Set the current file number
void FileIterator::set_file_num(unsigned fnum) {
  file_num=fnum;
}

// Set the file header
void FileIterator::set_file_header(const string &head) {
  header=head;
}

// Set the file extension
void FileIterator::set_file_extensions(const string &ext) {
  extension=ext;
}
  
// Set the number of leading zeros
void FileIterator::set_lead_zeros(unsigned digs) {
  digs=digits;
}  

// Set the current file number, header, extension, leading zeros
void FileIterator::set(unsigned fnum, const string &head, const string &ext,
                       unsigned digs) {
  file_num=fnum;
  header=head;
  extension=ext;
  digits=digs;
}

string FileIterator::nextfilename() {
  // Get an output filename from a string
  string filename;
  filename=a::itoa(file_num);
  while (filename.length()<digits)
    filename='0'+filename;
  file_num++;
  return header+filename+extension;
}

