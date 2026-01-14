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

#include "RISCVAddressAnalysis.h"
#include "Utl.h"

#define TRACE_setLoadStoreAddressAttribute(S) 


RegState* RISCVAddressAnalysis::NewRegState(int stackSize)
{
  return new RISCVRegState( RISCV_NB_REGISTERS,stackSize);
}

bool RISCVAddressAnalysis::TryToComputeStackSize( const vector < Instruction * > &listInstr, int iFrom, int iTo, int *stackSize)
{
  // take into account the case :
  // 00000000000100b0 <main>:
  // 100b0:	fffff337          	lui	t1,0xfffff  -- 0xfffff = -4096
  // 100b4:	42030313          	addi	t1,t1,1056 
  // 100b8:	00610133          	add	sp,sp,t1
  if ( iTo - iFrom > 3 ) return false;
  
  string instr1 = listInstr[iFrom]->GetCode();
  string instr2 = listInstr[iFrom+1]->GetCode();
  string instr3 = listInstr[iTo]->GetCode();
  vector < string > v_instr1 = Arch::splitInstruction (instr1);
  if (v_instr1[0] != "lui") return false;
  vector < string > v_instr2 = Arch::splitInstruction (instr2);
  if (v_instr2[0] != "addi") return false;
  vector < string > v_instr3 = Arch::splitInstruction (instr3);

  if ( v_instr3[1] != "sp")  return false;
  if ( v_instr3[2] != "sp") return false;

  string usedReg = v_instr1[1];
  if ( v_instr3[3] !=  usedReg) return false;
  if ( v_instr2[1] !=  usedReg) return false;
  if ( v_instr2[2] !=  usedReg) return false;

  string luivalue = v_instr1[2];
  string addedValue= v_instr2[3];

  string v = v_instr1[2] + "000"; // 20 bits + 12 bits to 0  (0x11000)
  int ival = strtol(v.c_str(), NULL, 16);
  int addedval = strtol(addedValue.c_str(), NULL, 10);
  *stackSize = ival + addedval;
  return true; 
}

//-----------Public -----------------------------------------------------------------

RISCVAddressAnalysis::RISCVAddressAnalysis (Program * p, int sp):MIPSAddressAnalysis (p,sp)
{ }

bool
RISCVAddressAnalysis::PerformAnalysis ()
{
  return MIPSAddressAnalysis::PerformAnalysis ();
}

// Checks if all required attributes are in the CFG
// Returns true if successful, false otherwise
bool
RISCVAddressAnalysis::CheckInputAttributes ()
{
  return MIPSAddressAnalysis::CheckInputAttributes ();
}

//nothig to remove
void
RISCVAddressAnalysis::RemovePrivateAttributes ()
{
  MIPSAddressAnalysis::RemovePrivateAttributes ();
}


bool RISCVAddressAnalysis::setLoadStoreAddressAttribute(Cfg * vCfg, Instruction* vinstr, RegState *state, Context *context)
{
  return MIPSAddressAnalysis::setLoadStoreAddressAttribute(vCfg, vinstr, state, context);
}

