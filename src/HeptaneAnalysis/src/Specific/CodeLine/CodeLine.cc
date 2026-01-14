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

#include "CodeLine.h"
#include "Utl.h"

static bool ACCEPT_NO_CODELINE_ATTRIBUTE;

CodeLine::CodeLine (Program * p, string bin, string archi, string addr2lineExe):Analysis (p)
{
  binary = bin;
  ADDR2LINE = addr2lineExe;
}

bool
CodeLine::associateCodeLineBB (vector < long >&addr, vector < Instruction * >&inst, string & bin)
{
  FILE *f;
  stringstream cmdLine;

  cmdLine << ADDR2LINE << " -e " << bin;

  // Generate command line
  for (unsigned int i = 0; i < addr.size (); i++)
    {
      cmdLine << " " << hex << addr[i];
    }
  
  //open a pipe to addr2line
  f = popen (cmdLine.str ().c_str (), "r");
  if (f != NULL)
    {
      char s[2000];

      long line;
      string file;
      string tmpStr;

      //extract line and file from addr2line for each instruction
      for (unsigned int i = 0; i < inst.size (); i++)
	{
	  fgets (s, 1000, f);
	  tmpStr = string (s);
	  file = string (tmpStr, 0, tmpStr.find (":"));
	  if (file == "??") // The source may be modified for MSP430.
	    {
	      if (! ACCEPT_NO_CODELINE_ATTRIBUTE)
		{
		  cout << "The binary file is maybe not compiled with the -ggdb option or the code has been modified \n" << "Stopping code line association" << endl;
		  return false;
		}
	      else
		{
		  associateCodeLine_complete = false;
		}
	    }
	  else
	    {
	      if (string (tmpStr, tmpStr.find (":") + 1) == "??")
		{
		  cout << "The binary file is maybe not compiled with the -ggdb option\n" << "Stopping code line association\n";
		  return false;
		}
	      else
		{
		  line = atoi (string (tmpStr, tmpStr.find (":") + 1).c_str ());
		}
		
	      CodeLineAttribute CLAttr (file, line);
	      inst[i]->SetAttribute (CodeLineAttributeName, CLAttr);
	    }
	}
      pclose (f);
    }
  else
    {
      cout << "Problem while opening addr2line binutil" << endl;
      return false;
    }
  return true;
}

// ----------------------------------------------------------------
// Checks if all required attributes are in the CFG
// Returns true if successful, false otherwise
// Here, nothing specific to check
// ----------------------------------------------------------------
bool
CodeLine::CheckInputAttributes ()
{
  ACCEPT_NO_CODELINE_ATTRIBUTE=Utl::getForceMode();
  return true;
}

// ----------------------------------------------
// Performs the analysis
// Returns true if successful, false otherwise
// ----------------------------------------------
bool
CodeLine::PerformAnalysis ()
{

  associateCodeLine_complete = true;

  if (ADDR2LINE.empty())
    {
      cout << endl;
      cout << " ---- addr2line is unknown." << endl;

      return false;
    }
  TRACE(cout << " ADDR2LINE = " << ADDR2LINE << endl;);

  bool ret = true;
  vector < Cfg * >cfgList = p->GetAllCfgs ();

  //getting all CFGs of the program
  for (unsigned int i = 0; i < cfgList.size (); i++)
    {
      vector < Node * >nodeList = cfgList[i]->GetAllNodes ();

      //getting all BBs from a CFG
      for (unsigned int j = 0; j < nodeList.size (); j++)
	{
	  vector < Instruction * >instructionList = nodeList[j]->GetAsm ();

	  //getting all instructions from a BB
	  vector < long >codeAddrList;
	  vector < Instruction * >codeInstructionList;
	  for (unsigned int k = 0; k < instructionList.size (); k++)
	    {
	      // getting code addresse
	      AddressAttribute attr = (AddressAttribute &) instructionList[k]->GetAttribute (AddressAttributeName);
	      codeAddrList.push_back (attr.getCodeAddress ());
	      codeInstructionList.push_back (instructionList[k]);
	    }
	  //associate code line to a BB
	  ret = ret && associateCodeLineBB (codeAddrList, codeInstructionList, binary);
	}
    }

  if (! associateCodeLine_complete)
    cout << " *************** CodeLine::associateCodeLineBB: not complete (objdump code has been perhaps modified ...)" << endl;

  return ret;
}

// Remove all private attributes
void
CodeLine::RemovePrivateAttributes ()
{

}
