/* ---------------------------------------------------------------------

   Copyright IRISA, 2003-2017

   This file is part of Heptane, a tool for Worst-Case Execution Time (WCET)
   estimation.
   APP deposit IDDN.FR.001.510039.000.S.P.2003.000.10600

   Heptane is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Heptane is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details (COPYING.txt).

   See CREDITS.txt for credits of authorship

   ------------------------------------------------------------------------ */

#include "Utl.h"
#include <sstream>
#include <math.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include"arch.h"
#include <algorithm>

#define isHexaCode(c) ((c >='0' && c <='9') || ( c >= 'a' && c <='f'))
#define NULLCHAR '\0'
#define SPACE ' '


//singleton declaration
Utl * Utl;

Utl::Utl ()
{}

Utl::~Utl ()
{}

string Utl::getEnvDef(string varName)
{
  char *aStringValue = getenv (varName.c_str());
  if (aStringValue == NULL) return "UNDEFINED";
  return (string) aStringValue;
}


bool Utl::getForceMode( )
{
  string vmode;
  vmode = Utl::getEnvDef("HEPTANE_FORCE_MODE");
  return (vmode == "TRUE") || (vmode == "true")  || (vmode == "1");
}

void Utl::rmSpaces(string &str)
{
  //  std::string whitespaces (" \t\f\v\n\r");
  std::string whitespaces (" ");

  std::size_t i0 = str.find_first_not_of(whitespaces);
  if (i0 != 0) str.erase(0, i0);
  std::size_t found = str.find_last_not_of(whitespaces);
  if (found!= EOS ) 
    str.erase(found+1); 
  else 
    str.clear(); // str is all whitespace
}


bool Utl::isHexNumber(const string& str)
{
  if(str.length()<=2) { return false; }
  if(str[0] != '0' || str[1] != 'x' ) return false;
    
  for (size_t i=2;i<str.length();i++)
    { 
      if( ! isHexaCode(str[i]) ) return false;
    }
  return true;
}

/**
   @return true if a decimal value (val) is detected in a string (vstr) after an index (ifrom), false otherwise.
 */
bool Utl::getValueAfter (string & vstr, size_t ifrom, long *val)
{
  string v = vstr.substr(ifrom+1);
  if (! Utl::isDecNumber(v)) return false;
  *val = atol (v.c_str ());
  return true;
}

// from arch.cc
bool Utl::isAddr(const string& str) 
{ 
  /* Specificity of Address in MSP430 */
  if (Arch::getArchitectureName() == "MSP430")
    {
      if(str.length() <= 2) return false;
      if(str[0] != '0' || str[1] != 'x' ) return false;
    
      for (size_t i=2;i<str.length();i++)
	{ 
	  if( ! isHexaCode(str[i]) ) return false;
	}
      return true;
    }
	
  assert(str.length()>0);
  for (size_t i=0;i<str.length();i++)
    {
      if( ! isHexaCode(str[i]) ) return false;
    }
  return true;
}

bool Utl::isDecNumber(const string& str)
{
  char c;
  size_t lg = str.length();
  assert(lg>0);
  for (size_t i=0;i< lg;i++)
    {
      c = str[i];
      if(c<'0' || c>'9')
        {
	  if (i==0 && c=='-')
	    {;}
	  else 
	    if(c != SPACE) return false;
        }
    }
  return true;
}

int Utl::string2int(string vpattern)
{
  if (vpattern.length() == 0) return 0;
  assert (isDecNumber (vpattern));
  return atoi (vpattern.c_str ());
}

long Utl::string2long(string vpattern)
{
  if (vpattern.length() == 0) return (long)0;
  bool b = isDecNumber (vpattern);
  assert(b);
  return atol (vpattern.c_str ());
}
	  
int Utl::imax(int i, int j)
{
  if (i>j) return i;
  return j;
}

bool Utl::replace(string &line, char c1, char c2)
{
  size_t pos= line.find(c1);
  if (pos == EOS) return false;
  line[pos]=c2;
  return true;
}

bool Utl::replaceAll(string &line, char c1, char c2)
{
  bool b = false;
  while(replace(line, c1,c2) ) b=true;
  return b;
}

int Utl::count(string &line, char sep)
{
  if (line.length () == 0) return 0;
  int n = 0;
  for (size_t i = 0; i < line.length (); i++) 
    if (line[i] == sep) n++;
  return n;
}


string Utl::complement(string v)
{
  std::stringstream ss; 
  ss << ~ string2int(v);
  return ss.str();
}


string Utl::int2cstring(long v)
{
  ostringstream ossOffset;
  ossOffset << v;
  return ossOffset.str();
}

string Utl::int2string(long v)
{
  return  int2cstring(v);
};

bool Utl::eval(string &operand1, string &codop, string &operand2, long *v)
{
  long v1, v2;

  // cout << endl << "eval (" << operand1 << "," << ocodeop <<"," <<  operand2 << ")" << endl;
  if (! isDecNumber(operand1)) return false;
  if (! isDecNumber(operand2)) return false;
  v1 = atol (operand1.c_str ());
  v2 = atol (operand2.c_str ());
  if (codop.find("+") != EOS) *v = v1 + v2;
  else if (codop.find("-") != EOS) *v = v1 - v2;
  else if (codop.find("*") != EOS) *v = v1 * v2;
  else return false;
  return true;
}

bool Utl::evalRemainder(string &operand1, string &operand2, long *v)
{
  long v1, v2;

  // cout << endl << "eval (" << operand1 << "," << ocodeop <<"," <<  operand2 << ")" << endl;
  if (! isDecNumber(operand1)) return false;
  if (! isDecNumber(operand2)) return false;
  v1 = atol (operand1.c_str ());
  v2 = atol (operand2.c_str ());
  if (v2 == 0) return false;
  *v = v1 % v2;
  return true;
}


bool Utl::evalDivide(string &operand1, string &operand2, long *v)
{
  long v1, v2;

  if (! isDecNumber(operand1)) return false;
  if (! isDecNumber(operand2)) return false;
  v1 = atol (operand1.c_str ());
  v2 = atol (operand2.c_str ());
  if (v2 == 0) return false;
  *v = v1 / v2;
  return true;
}

bool Utl::evalNegate(string &operand1, long *v)
{
  long v1;
  if (! isDecNumber(operand1)) return false;
  v1 = atol (operand1.c_str ());
  *v = -v1;
  return true;
}


bool Utl::evalexpr(string &exp, long *val)
{
  bool foundPlus, foundMinus, foundMult;
  size_t index;
  string op1, op2, codeop;
  
  // looking for ONE +|-|* in exp. otherwise return false.
  index= exp.find ("+");
  foundPlus= (index != EOS );

  index= exp.find ("-");
  foundMinus= (index != EOS );
  if (foundPlus && foundMinus) return false;

  index= exp.find ("*");
  foundMult= (index != EOS );
  if (foundPlus && foundMult) return false;
  if (! foundPlus && !foundMult && !foundMinus) return false;

  op1 = exp.substr(0, index);
  rmSpaces(op1);
  op2 = exp.substr(index+1);
  rmSpaces(op2);
  codeop= (foundPlus ? "+" : (foundMinus ? "-": "*"));
  return eval(op1, codeop, op2, val);
}


string Utl::extractStringValue(string &arg)
{
  size_t pos;
  string res;

  pos=arg.find("\"");
  if (pos == EOS ) return string("");
  res=arg.substr (pos+1);
  pos=res.find("\"");
  if (pos == EOS ) return string("");
  res=res.substr (0, pos);
  return res;
}


//  mem_pattern is assume to be "val(sp)"
void Utl::extractRegVal( string mem_pattern, string &reg, string &val)
{
  replace( mem_pattern, '(', SPACE);
  replace( mem_pattern, ')', SPACE);
  istringstream parse (mem_pattern);
  parse >> val >> reg;
}

bool Utl::getintvalue(string &valstr, int *n)
{
  if (isDecNumber(valstr))
    {
      *n = atoi (valstr.c_str ());
      return true;
    }
  if (isHexNumber(valstr)) 
    {
      std::istringstream iss(valstr);
      iss >> std::hex >> *n;
      return true;
    }
  return false;
}

vector<string> Utl::split(string stro, string seps) {
  std::vector<std::string> elems;
  char * pch;
  const char* delims = seps.c_str();
  // Working with a copy of stro (modified by strok function).
  string str = (char*) stro.c_str();
  char *s= (char*) str.c_str();

  pch = strtok (s, delims);
  while (pch != NULL)
    {
      elems.push_back(pch);
      pch = strtok (NULL, delims);
    }
  return elems;
}


// ---------------- 

bool Utl::mktmpfile(string prefix, string  &res)
{
  char *fileNameTemplate;

  string S= prefix + "XXXXXX";
  int lg = strlen(S.c_str());
  fileNameTemplate = (char*) malloc(lg+1);
  strncpy(fileNameTemplate, S.c_str(), lg);
  fileNameTemplate[lg] = NULLCHAR;

  // Create the temporary file, this function will replace the 'X's
  int filedes = -1;
  filedes = mkstemp(fileNameTemplate);
  // printf("\n Try to create the temp file %s\n", fileNameTemplate);
  // errno = 0;
  // Call unlink so that whenever the file is closed or the program exits the temporary file is deleted
  unlink(fileNameTemplate);

  if(filedes == -1)
    {
      printf("\n Creation of temp file ([%s]), failed with error [%s]\n", fileNameTemplate, strerror(errno));
      return false;
    }
  res = string(fileNameTemplate);
  // printf("\n Temporary file [%s] created\n",  fileNameTemplate);
  return true;
}


bool  Utl::getIndexFirstValue(vector<string> &vectOperand, int *i)
{
  int n = vectOperand.size();
  if (n < 2) return false;
  bool found = false;
  int ii=0;
  // getting the first decimal value
  while (! found && ii <n)
    {
      found = isDecNumber(vectOperand[ii]);
      ii++;
    }
  
  if (!found) return false;
  *i = ii - 1;
  return true;
}


string Utl::demangle(const string & name) {
  string res;
  int status;
  char   *realname = abi::__cxa_demangle(name.c_str(), 0, 0, &status);
  if(status == 0) {
    res = realname;
    res = res.substr(0, res.find_first_of('('));
  }
  else
    res = name;
    
  if(realname != NULL)
    free(realname);
    
  return res;
}

/*
 * Check if a file exist using stat() function
 * return 1 if the file exist otherwise return 0
 */
bool Utl::file_exists(const string &filename) {
  struct stat buffer;
  return stat(filename.c_str(),&buffer) == 0;
}

bool Utl::isFPNumber(const string& str)
{
  size_t pos; pos=str.find(".");
  if (pos == EOS) return isDecNumber(str);
  string prefix=str.substr(0, pos);
  if (! isDecNumber(prefix)) return false;
  return isDecNumber(str.substr(pos+1));
}


long Utl::hexa2dec(const string& hexavalue)
{
  int lg, i, val;
  long result;

  result = 0;
  lg = hexavalue.size() -1;
  for(i=0; hexavalue[i]!=NULLCHAR; i++)
    {
      if(hexavalue[i]>='0' && hexavalue[i]<='9')
	val = hexavalue[i] - 48;
      else if(hexavalue[i]>='a' && hexavalue[i]<='f')
	val = hexavalue[i] - 97 + 10;
      else if(hexavalue[i]>='A' && hexavalue[i]<='F')
	val = hexavalue[i] - 65 + 10;
      
      result += val * pow(16, lg);
      lg--;
    }
  return result;
}

bool Utl::getNextChar(const string &vstr, size_t *vindex )
{
  while (vstr[*vindex] == SPACE)
    {
      if (vstr[*vindex] == NULLCHAR) return false;
      (*vindex)++;
    }
  return true;
}

bool Utl::parseRefStackPointer(const string &vstr, int *i)
{
  // Scanning : sp + val, sp - val

  size_t vindex = 0;
  if (!getNextChar(vstr, &vindex)) return false;

  // wait "sp"
  if (vstr[vindex] != 's') return false;
  vindex++;
  if (vstr[vindex] != 'p') return false;
  vindex++;
  if (!getNextChar(vstr, &vindex)) return false;
  // sign
  char c = vstr[vindex];
  if ((c == '-') || (c == '+'))
    {
      vindex++;
      if (!getNextChar(vstr, &vindex)) return false;
      *i=0;
      string val = vstr.substr(vindex);
      bool b = getintvalue(val, i);
      if (b && (c == '-')) *i= -(*i);
      return b;
    }
  return false;
}

int Utl::scanIntegerfrom(string vstr, int ifrom) {
  char c;
  size_t lg = vstr.length();
  int j = ifrom;
  while (vstr[j] == SPACE || vstr[j] == '\t') j++;
  for (size_t i=j;i < lg;i++)
    {
      c = vstr[i];
      if(c<'0' || c>'9')
        {
	  if (i==0 && c=='-')
	    {;}
	  else {
	    if(c == SPACE) return i-ifrom;
	    if (c == '\t') return i-ifrom;
	    if (c == ';') return i-ifrom;
	    if (c == ',') return i-ifrom;
	  }
	}
    }
  return -1;
}

void Utl::removeAndKill(string & vstr, int ifrom, char c) {
  std::string::iterator itx = std::remove(vstr.begin()+ifrom, vstr.end(), c);
  vstr.erase(itx, vstr.end());
}


