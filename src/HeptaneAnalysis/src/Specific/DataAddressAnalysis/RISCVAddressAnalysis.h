/* ------------------------------------------------------------------------------------

   Copyright IRISA, 2003-2017

   This file is part of Heptane, a tool for Worst-Case Execution Time (WCET) estimation.
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

   ------------------------------------------------------------------------------------ */

/*****************************************************************
                             RISCVAddressAnalysis

This is the entry point of the dataflow analysis of 
data and stack addresses for RISCV architecture.

This analysis is very basic in the sense:
- it does not consider pointers 
- it is perfomed on an intra-basic block basis

*****************************************************************/


#ifndef RISCVADDRESSANALYSIS_H
#define RISCVADDRESSANALYSIS_H

#include <ostream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <vector>
#include <set>
#include <map>

#include <errno.h>

#include "RISCV.h"
#include "AddressAnalysis.h"
#include "MIPSAddressAnalysis.h"
#include "RegState.h"

using namespace std;

class RISCVAddressAnalysis:public MIPSAddressAnalysis
{
 private:
  virtual RegState* NewRegState(int stackSize);
  virtual bool TryToComputeStackSize( const vector < Instruction * > &listInstr, int ifrom, int iTo, int *stackSize);
 public:

  /** Constructor */
  RISCVAddressAnalysis (Program * p, int sp);
  
  /** Checks if all required attributes are in the CFG
      @return true if successful, false otherwise
  */
  bool CheckInputAttributes ();
  
  /** Performs the analysis
      @return true if successful, false otherwise.
  */
  bool PerformAnalysis ();
  
  /** Remove all StackInfoAttributes (never used elsewhere) */
  void RemovePrivateAttributes ();

  bool setLoadStoreAddressAttribute(Cfg * vCfg, Instruction* vinstr, RegState *state, Context *context);
  
};
#endif
