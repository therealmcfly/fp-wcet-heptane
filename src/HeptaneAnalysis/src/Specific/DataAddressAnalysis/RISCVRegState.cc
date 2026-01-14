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

#include <iostream>
#include <string>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <cassert>

#include "RegState.h"
#include "DAAInstruction.h"

#include "Logger.h"
#include "Utl.h"
#include "RISCV.h"

#include <limits.h>

using namespace std;

#define LOCTRACE(S)

/**********************************************************/
/**********************************************************/
/**********************************************************/
// RegState implementation.

RISCVRegState::RISCVRegState(int nbregs, int stackSize):MIPSRegState(nbregs, stackSize)
{
  RegState::reset();
  setPredefValues();
}

void RISCVRegState::setPredefValues()
{
  reset_sp();

  state[ RISCV_GP_REGISTER ] = "gp";
  precision[ RISCV_GP_REGISTER] = true;

  state[RISCV_ZERO_REGISTER] = "0";
  precision[RISCV_ZERO_REGISTER] = true;

  state[RISCV_RA_REGISTER] = "ra";
  precision[RISCV_RA_REGISTER] = true;

}

void RISCVRegState::reset_sp()
{
  state[ RISCV_SP_REGISTER ] = "sp";
  precision[ RISCV_SP_REGISTER] = true;
}

int RISCVRegState::getAuxRegister()
{
  return RISCV_AUX_REGISTER;
}

int RISCVRegState::getSPRegister()
{
  return RISCV_SP_REGISTER;
}

// Simple wrap-up to method simulate on instructionRISCV, except 
// the check that the instruction does not kill gp and sp
void RISCVRegState::simulate(Instruction * vinstr)
{
  string instr = vinstr->GetCode();

  long vaddress; getCodeAddress( vinstr, &vaddress);
  TRACE_simulate (cout << " +++ BEFORE simulate of " << instr << " at " << std::hex << vaddress << " " << std::dec << endl;  printStates(); printStack(););

  DAAInstruction *instruct = Arch::getDAAInstruction(instr);
  instruct->simulate(state, precision, vStack, vStackPrecision, instr);

  assert(precision[RISCV_ZERO_REGISTER]);
  assert(precision[RISCV_GP_REGISTER]);
  assert(precision[RISCV_SP_REGISTER]);

  TRACE_simulate (cout << " +++ AFTER simulate of " << instr << endl; printStates(); printStack(););
}

/** 
    @return true if value is a "lui" instruction, false otherwise.
    pattern: lui value [+- decimal_value]
    value = state[register_number]
*/
bool RISCVRegState::isAccessAnalysisLui(int register_number, string offset, vector < string > &result)
{
  // RISCV: LUI (load upper immediate) is used to build 32-bit constants and uses the U-type format. LUI
  //        places the U-immediate value in the top 20 bits of the destination register rd, filling in the lowest
  //        12 bits with zeros.
  string value = state[register_number];
  ostringstream ossAddr, xtmp;
  int addr;

  if (Utl::isDecNumber(value))
    {
      addr = strtol(value.c_str(), NULL, 10);
    }
  else
    {
      // example: 0x11lui
      size_t found = value.find("lui"); 
      if (found == EOS) return false;

      string Addrh = value;
      Addrh.erase(found); // remove 'lui'
      
      Addrh = Addrh +"000"; // 20 bits + 12 bits to 0  (0x11000)
      addr = strtol(Addrh.c_str(), NULL, 16);
      if (value.find("lui") + 3 != value.length())
	{
	  Logger::addError(" RISCV: accessAnalysis -> LUI access");
	  assert(false);
	}
    }
  long loffset = atol(offset.c_str());
  assert(Utl::isDecNumber(offset));
  addr = addr + loffset;
  ossAddr << addr ;
  // cout << "   RISCVRegState::isAccessAnalysisLui returns value = " << ossAddr.str() << endl;
  result.push_back("lui");
  result.push_back(ossAddr.str());
  result.push_back((precision[register_number] ? "1" : "0"));
  return true;
}

bool RISCVRegState::isAccessAnalysisGP(int register_number, string offset, vector < string > &result)
{
  return MIPSRegState::isAccessAnalysisGP(register_number, offset, result);
}

bool RISCVRegState::isAccessAnalysisSP(int register_number, string offset, vector < string > &result)
{
  return MIPSRegState::isAccessAnalysisSP(register_number, offset, result);
}

void RISCVRegState::accessAnalysis(Instruction * vinstr, vector < string > &result)
{
  return MIPSRegState::accessAnalysis(vinstr, result);
}

bool RISCVRegState::isAccessAnalysisOtherRegister(Instruction * vinstr, int register_number, string offset, vector < string > &result)
{
  return MIPSRegState::isAccessAnalysisOtherRegister(vinstr, register_number, offset, result);
}

string RISCVRegState::getAliasRegister(int ireg)
{
  string reg = Utl::int2string(ireg);
  if (ireg == 0) return "zero";
  if (ireg == 1) return "ra";
  if (ireg == 2) return "sp";
  if (ireg == 3) return "gp";
  if (ireg == 4) return "tp"; // thread pointer
  if (ireg == 5) return "t0"; // tmp
  if (ireg == 6) return "t1"; // tmp
  if (ireg == 7) return "t2"; // tmp
  if (ireg == 8) return "fp"; // frame pointer
  if (ireg == 9) return "s1"; // saved register
  if ((ireg >= 10) && (ireg <= 17)) return 'a' + Utl::int2string(ireg-10); // function argument
  if ((ireg >= 18) && (ireg <= 27)) return 's'  +  Utl::int2string(ireg-18+ 1); // saved register
  if ((ireg >= 28 ) && (ireg <= 31)) return 't'  +  Utl::int2string(ireg-28 + 3) ; // tmp register

  // FP

  if (ireg == 32) return "ft0";
  if (ireg == 33) return "ft1";
  if (ireg == 34) return "ft2";
  if (ireg == 35) return "ft3";
  if (ireg == 36) return "ft4";
  if (ireg == 37) return "ft5";
  if (ireg == 38) return "ft6";
  if (ireg == 39) return "ft7";

  if (ireg == 40) return "fs0";
  if (ireg == 41) return "fs1";

  if (ireg == 42) return "fa0";
  if (ireg == 43) return "fa1";
  if (ireg == 44) return "fa2";
  if (ireg == 45) return "fa3";
  if (ireg == 46) return "fa4";
  if (ireg == 47) return "fa5";
  if (ireg == 48) return "fa6";
  if (ireg == 49) return "fa7";

  if (ireg == 50) return "fs2";
  if (ireg == 51) return "fs3";
  if (ireg == 52) return "fs4";
  if (ireg == 53) return "fs5";
  if (ireg == 54) return "fs6";
  if (ireg == 55) return "fs7";
  if (ireg == 56) return "fs8";
  if (ireg == 57) return "fs9";
  if (ireg == 58) return "fs10";
  if (ireg == 59) return "fs11";

  if (ireg == 60) return "ft8";
  if (ireg == 61) return "ft9";
  if (ireg == 62) return "ft10";
  if (ireg == 63) return "ft11";
  return 'r'+reg;
}



