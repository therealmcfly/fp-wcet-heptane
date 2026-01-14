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
#include "MIPS.h"

#include <limits.h>

using namespace std;

#define LOCTRACE(S)

/**********************************************************/
/**********************************************************/
/**********************************************************/
// RegState implem

MIPSRegState::MIPSRegState(int nbregs, int stacksize):RegState(nbregs, stacksize)
{
  setPredefValues();
}

void MIPSRegState::setPredefValues()
{
  reset_sp();

  // gp is  known precisely
  state[MIPS_GP_REGISTER] = "gp";
  precision[MIPS_GP_REGISTER] = true;

  state[MIPS_ZERO_REGISTER] = "0";
  precision[MIPS_ZERO_REGISTER] = true;

  // return address
  state[MIPS_RA_REGISTER] = "ra";
  precision[MIPS_RA_REGISTER] = true;
}

void MIPSRegState::reset_sp()
{
  // sp is known precisely
  state[MIPS_SP_REGISTER] = "sp";
  precision[MIPS_SP_REGISTER] = true;
}

int MIPSRegState::getAuxRegister()
{
  return MIPS_AUX_REGISTER;
}

int MIPSRegState::getSPRegister()
{
  return MIPS_SP_REGISTER;
}

// Simple wrap-up to method simulate on instructionMIPS, except
// the check that the instruction does not kill gp and sp
void MIPSRegState::simulate(Instruction * vinstr)
{
  string instr = vinstr->GetCode();

  long vaddress; getCodeAddress( vinstr, &vaddress);
  TRACE_simulate (cout << " +++ BEFORE simulate of " << instr << " at " << std::hex << vaddress << " " << std::dec << endl;  printStates(); printStack());

  DAAInstruction *instruct = Arch::getDAAInstruction(instr);
  instruct->simulate(state, precision, vStack, vStackPrecision, instr);

  assert(precision[MIPS_ZERO_REGISTER]); 
  assert(precision[MIPS_GP_REGISTER]);
  assert(precision[MIPS_SP_REGISTER]);

  TRACE_simulate (cout << " +++ AFTER simulate of " << instr << endl; printStates(); printStack());
}

/** 
    @return true if value is a "lui" instruction, false otherwise.
    pattern: 68lui [+ decimal_value]
    value = state[register_number]
*/
bool MIPSRegState::isAccessAnalysisLui(int register_number, string offset, vector < string > &result)
{
  // Warning: the state of a register is modified by the state->simulate() method.

  // examples : MIPS (I): lui v0, 0x40 ==> state[0] = "lui64" in simulate() method. 0x40(hexa) = 64 (dec)
  // if (state[0] == f("lui")) may be true (if not modified from (I))

  unsigned long addr;
  string value = state[register_number];
  size_t found = value.find("lui");
  long val;

  if (found == EOS) return false;

  // -------------------------------------
  // Computing v * (2**16) + v' + offset, with  state[register_number] = vlui + v'

  // Getting the base address from the lui.
  // addr can be in two distinct part: Addrh bits and Addrl bits
  string Addrh = value;
  Addrh.erase(found); // remove 'lui'

  // check that Addrh is a number
  // DH: FIXME: faire plus clean et voir pourquoi ici c'est en hexa
  if (! Utl::isDecNumber(Addrh))
    {
      addr = strtoul(value.c_str(), NULL, 16);
      assert(addr != 0 && addr != ULONG_MAX);
    }
  else
    {
      addr = atol(Addrh.c_str());
    }
  addr = addr * 65536; // the immediate value is shifted left 16 bits (see LUI instruction)

  found = value.find("+");
  if (found != EOS)
    {
      string Addrl = value;
      assert(Utl::getValueAfter(Addrl, found, &val));
      addr = addr + val;
    }
  else	
    {
      found = value.find("-");
      if (found != EOS)
	{
	  string Addrl = value;
	  assert(Utl::getValueAfter(Addrl, found, &val));
	  addr = addr - val;
	}
      else //check that 'lui' is not followed by something else in the register value
	{
	  if (value.find("lui") + 3 != value.length())
	    {
	      Logger::addError("accessAnalysis -> LUI access");
	      assert(value.find("lui") + 3 == value.length());
	    }
	}
    }
  
  // Add the offset to the base address
  long loffset = atol(offset.c_str());
  assert(Utl::isDecNumber(offset));
  addr = addr + loffset;
  ostringstream ossAddr;
  ossAddr << addr;

  result.push_back("lui");
  result.push_back(ossAddr.str());
  result.push_back((precision[register_number] ? "1" : "0"));
  LOCTRACE(cout << "**DEBUG** MIPSRegState::isAccessAnalysisLui. Registre = " << register_number << ", ossADDR = " << ossAddr.str() << endl);
  return true;
}

//--------------------------------------------------
//     This is the second case,
//     where the address is relative to the global
//     pointer gp.
//     pattern: gp + val
//--------------------------------------------------
bool MIPSRegState::isAccessAnalysisGP(int register_number, string offset, vector < string > &result)
{
  // Warning: the state of a register is modified by the state->simulate() method.
  ostringstream ossOffset;

  string value = state[register_number];
  size_t found = value.find("gp");	// Not only for register_number=28, but mov ... copies the value.
  if (found == EOS)
    return false;

  long loffset = atol(offset.c_str());
  assert(Utl::isDecNumber(offset));
  string oper = value;
  found = oper.find("-");
  if (found == EOS)
    {
      Logger::addError("reg analysis -> '-' not found for gp + -value");
    }
  else
    {
      oper.erase(oper.begin(), oper.begin() + found + 1);
      assert(Utl::isDecNumber(oper));	//check it is a number
      assert(oper[0] != '-');	//check it is a positive number
      assert(Utl::isDecNumber(oper));
      loffset = loffset - atol(oper.c_str());
      ossOffset << loffset;

      result.push_back("gp");
      result.push_back(ossOffset.str());
      result.push_back((precision[register_number] ? "1" : "0"));
    }
  LOCTRACE(cout << "**DEBUG**, MIPSRegState::isAccessAnalysisGP. Registre = " << register_number << ", ossADDR = " << ossOffset.str() << endl);
  return true;
}

/* 
   This is the third case, where the address is relative to the stack
   pointer sp, in a transitive fashion, as it  appears.
*/
bool MIPSRegState::isAccessAnalysisSP(int register_number, string offset, vector < string > &result)
{
  // Warning: the state of a register is modified by the state->simulate() method.

  ostringstream ossOffset;
  string prec;

  string value = state[register_number];
  size_t found = value.find("sp");
  if (found == EOS)
    return false;

  long loffset = atol(offset.c_str());
  assert(Utl::isDecNumber(offset));

  string oper = value;
  found = oper.find("+");
  if (found != EOS)
    {
      long val;
      if ( !Utl::getValueAfter(oper, found, &val))
	{
	  cout << " getValueAfter returns false for value = " <<  value << endl;
	  assert(false);
	}
      loffset = loffset + val;
      prec = (precision[register_number] ? "1" : "0");
    }
  else
    {
      found = oper.find("-");
      if (found != EOS)
	{
	  long val;
	  assert(Utl::getValueAfter(oper, found, &val));
	  loffset = loffset - val;
	  prec = (precision[register_number] ? "1" : "0");
	}
      else
	{
	  //that case can be found in case of an array in the stack
	  //benchmark: ud
	  //4006b0:   8fa20004        lw      v0,4(sp)
	  //4006b4:   00000000        nop
	  //4006b8:   00021080        sll     v0,v0,0x2
	  //4006bc:   03a21021        addu    v0,sp,v0
	  //4006c0:   8c420010        lw      v0,16(v0)
	  
	  assert(oper.length() == 2);	//only sp
	  assert(!precision[register_number]);	//not precise (array access)
	  prec = "0";
	}
    }

  ossOffset << loffset;
  result.push_back("sp");
  result.push_back(ossOffset.str());
  result.push_back(prec);
  LOCTRACE(cout << "**DEBUG**, MIPSRegState::isAccessAnalysisSP. Registre = " << register_number << ", ossADDR = " << ossOffset.str() << endl);
  return true;
}

/*-----------------------------------------------------------------------------
 * This function returns a vector of string which represents the memory access of the instruction.
 * this information is analysed after to determine the address, size etc... of this access
 *
 * possible returns (follow the order of the code):
 *	- in case the address comes from a lui
 *	0   "lui"
 *	1	addr
 *	2	precision (=="1" if precise analysis and "0" otherwise)
 *	- in case the address is based on gp
 *	0   "gp"
 *	1	offset
 *	2	precision (=="1" if precise analysis and "0" otherwise)
 *	- in case the address is based on sp
 *	0   "sp"
 *	1	offset
 *	2	precision (=="1" if precise analysis and "0" otherwise)
 *	- unknown
 *	0   "*"
 *	1	"*"
 *	2	"*"
 *
 -----------------------------------------------------------------------------*/
void MIPSRegState::accessAnalysis(Instruction * vinstr, vector < string > &result)
{
  vector < string > split_instruction;
  string op2, reg, offset, instr;
  int register_number;

  instr = vinstr->GetCode();
  // Warning: the state of a register is modified by the state->simulate() method.
  split_instruction = Arch::splitInstruction(instr);
  assert(split_instruction.size() == 3);

  op2 = split_instruction[2];

  reg = Arch::extractInputRegistersFromMem(op2)[0];
  register_number = Arch::getRegisterNumber(reg);

  offset = op2.erase(op2.find("("));
  // Analyzing the expression associated with the target register of the instuction
  if (!isAccessAnalysisLui(register_number, offset, result))
    if (!isAccessAnalysisGP(register_number, offset, result))
      if (!isAccessAnalysisSP(register_number, offset, result))
	if (! isAccessAnalysisOtherRegister(vinstr, register_number, offset, result))
	  {
	    bool b = AccessAnalysisDefault(register_number, result);
	    if (!b)
	      {
		long vaddr;
		getCodeAddress(vinstr, &vaddr);
		cout << "**DEBUG**, AccessAnalysisDefault  " << instr << " at address = " << std::hex << vaddr << " " << std::dec << endl;
	      }
	    assert(b); // Default is not allowed for all instructions!
	  }
}

/* 

*/
bool MIPSRegState::isAccessAnalysisOtherRegister(Instruction * vinstr, int register_number, string offset, vector < string > &result)
{
  string value = state[register_number];
  cout << " instr = " << vinstr->GetCode();
  cout  << " state[ " << register_number << " ] = " << value;
  cout << endl;
  return false;
}

string MIPSRegState::getAliasRegister(int ireg)
{
  string reg = Utl::int2string(ireg);
  if (ireg == 0) return "zero";
  if (ireg == 1) return "at";
  if (ireg == 2) return "v0";
  if (ireg == 3) return "v1";
  if ((ireg >= 4) && (ireg <=7)) return 'a' +  Utl::int2string(ireg-4);  // arguments
  if ((ireg >= 8) && (ireg <=15)) return 't'  +  Utl::int2string(ireg-8); // tmp
  if ((ireg >= 16) && (ireg <=23)) return 's'  +  Utl::int2string(ireg-16); // tmp

  if (ireg == 24) return "t8";
  if (ireg == 25) return "t9";

  if (ireg == 26) return "k0"; // reserved for kernel
  if (ireg == 27) return "k1";
  if (ireg == 28) return "gp";
  if (ireg == 29) return "sp";
  if (ireg == 30) return "fp";
  if (ireg == 31) return "ra";

  // FP
  if (ireg == 32)  return "$f0";
  if (ireg == 33)  return "$f1";
  if (ireg == 34)  return "$f2";
  if (ireg == 35)  return "$f3";
  if (ireg == 36)  return "$f4";
  if (ireg == 37)  return "$f5";
  if (ireg == 38)  return "$f6";
  if (ireg == 39)  return "$f7";
  if (ireg == 40)  return "$f8";
  if (ireg == 41)  return "$f9";
  if (ireg == 42)  return "$f10";
  if (ireg == 43)  return "$f11";
  if (ireg == 44)  return "$f12";
  if (ireg == 45)  return "$f13";
  if (ireg == 46)  return "$f14";
  if (ireg == 47)  return "$f15";
  if (ireg == 48)  return "$f16";
  if (ireg == 49)  return "$f17";
  if (ireg == 50)  return "$f18";
  if (ireg == 51)  return "$f19";
  if (ireg == 52)  return "$f20";
  if (ireg == 53)  return "$f21";
  if (ireg == 54)  return "$f22";
  if (ireg == 55)  return "$f23";
  if (ireg == 56)  return "$f24";
  if (ireg == 57)  return "$f25";
  if (ireg == 58)  return "$f26";
  if (ireg == 59)  return "$f27";
  if (ireg == 60)  return "$f28";
  if (ireg == 61)  return "$f29";
  if (ireg == 62)  return "$f30";
  if (ireg == 63)  return "$f31";
  if (ireg == 64)  return "$fcc0";

  return 'r'+reg;
}



