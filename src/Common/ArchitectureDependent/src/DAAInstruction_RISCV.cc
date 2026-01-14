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

#include <vector>
#include <string>
#include <iostream>

#include <fstream>
#include <sstream>
#include <cassert>

#include "DAAInstruction_RISCV.h"
#include "RISCV.h"
#include "arch.h"
#include "Utl.h"

using namespace std;

#define LOCTRACE(s) 
#define DAA_TRACE(instr_cl, instr) LOCTRACE(cout << " ---> " << instr_cl << "::simulate (" << instr << " )" <<  endl)
#define DAA_TRACE_NOT_IMPLEMENTED(instr_cl, instr) cout << " *** NOT IMPLEMENTED: " << instr_cl << "::simulate (" << instr << " )" <<  endl

#define REGISTERS_SWITCH(num1, num2) { int _aux_ = num1; num1 = num2; num2 = _aux_; }

bool DAAInstruction_RISCV::getRegisterAndIndexStack(const string &instructionAsm, string &reg, int *i)
{
  if (Arch::getRegisterAndIndexStack( instructionAsm, reg, i )) return true;
  // sw ra,20(a5), and a5 = SP + VAL
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  // num_register1 = Arch::getRegisterNumber(operands[1]);
  //   cout << "  DAAInstruction_RISCV::getRegisterAndIndexStack = " << operands[1] << endl;
  return false;
}

void DAAInstruction_RISCV::minus(regTable & regs, regPrecisionTable & precision)
{
  bool todo = true;
  string operand1 = regs[num_register1];
  size_t found = operand1.find("lui");
  if (found != EOS)  // && operand1.size() == found + 3 ??
    { 
      string operand2 = regs[num_register2]; // value ?
      if (Utl::isDecNumber(operand2))
	{
	  // -----
	  string Addrh = operand1;
	  Addrh.erase(found); // remove 'lui'
	  Addrh = Addrh +"000"; // 20 bits + 12 bits to 0  (0x11000)
	  int addr = strtol(Addrh.c_str(), NULL, 16); // ATTENTION BASE 16
	  // -----
	  int val = addr - Utl::string2int(operand2);
	  regs[num_register0] = Utl::int2string(val);
	  precision[num_register0] = true;
	  todo= false;
	  // cout << " DAAInstruction_RISCV: " << operand1 << " - " << operand2 << " = " << regs[num_register0] << endl;
	}
    }
  if (todo) 
    DAAInstruction::minus(regs, precision);
}


void DAAInstruction_RISCV::add(regTable & regs, regPrecisionTable & precision, bool bAugmentPrecision)
{
  bool todo = true;
  string operand1 = regs[num_register1];
  size_t found = operand1.find("lui");
  if (found != EOS)  // && operand1.size() == found + 3 ??
    { 
      string operand2 = regs[num_register2]; // value ?
      if (Utl::isDecNumber(operand2))
	{
	  // -----
	  string Addrh = operand1;
	  Addrh.erase(found); // remove 'lui'
	  Addrh = Addrh +"000"; // 20 bits + 12 bits to 0  (0x11000)
	  int addr = strtol(Addrh.c_str(), NULL, 16); // ATTENTION BASE 16
	  // -----
	  int val = addr + Utl::string2int(operand2);
	  regs[num_register0] = Utl::int2string(val);
	  precision[num_register0] = true;
	  todo= false;
	  // cout << " DAAInstruction_RISCV: " << operand1 << " + " << operand2 << " = " << regs[num_register0] << endl;
	}
    }
  if (todo) 
    DAAInstruction::add(regs, precision, bAugmentPrecision);
}

//--------------------------------------------
//      NOP
//
// Classes of instructions that neither produce nor destroy contents of 
// registers -> no impact on address analysis
//--------------------------------------------
void
RISCV_NOP::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
}

//---------------------------------------------
//      LOAD
//
// Classes of instruction that load information from memory
//
//---------------------------------------------
void RISCV_DLOAD::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
 bool b;
  int i,n;
  string reg;
  int vindex;

  b = getRegisterAndIndexStack(instructionAsm, reg, &n);
  if (b) 
    {
      bool i_set = getStackIndexFromRegister(regs, RISCV_SP_REGISTER, &i);
      num_register0 = Arch::getRegisterNumber(reg);
      if ( i_set && getEffectiveStackIndex(vstack, n+i, (n == 0 ? 0 : -1 ), &vindex))
	{
	  regs[num_register0] = vstack[vindex];
	  precision[num_register0] = vStackPrecision[vindex];
	}
      else
	{
	  regs[num_register0] = "*";
	  precision[num_register0] = false;
	}
    }
  else
    {
      vector < string > operands = getOperands(instructionAsm);
      num_register0 = Arch::getRegisterNumber(operands[0]);
      size_t found = instructionAsm.find("gp");  
      if ( found != EOS ) // codop  R, -val(gp)
	{
	  size_t vindex = instructionAsm.find("-");
	  if (vindex != EOS)
	    {
	      regs[num_register0] = instructionAsm.substr (vindex); // -val(gp)
	      precision[num_register0] = true;
	    }
	  else
	    {
	      LOCTRACE( cout << "DLoad::simulate = " << instructionAsm << endl; );
	      killop1(regs, precision);
	    }
	}
      else
	{
	  LOCTRACE( cout << "DLoad::simulate = " << instructionAsm << endl; );
	  killop1(regs, precision);
	}
    }
}


//---------------------------------------------
//      STORE
//
// Classes of instruction that load information from memory
//
//---------------------------------------------
void RISCV_DSTORE::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision,   const string & instructionAsm)
{
  bool b;
  int i,n;
  string reg;

  // example : sw ra,20(sp)
  b = getRegisterAndIndexStack(instructionAsm, reg, &n);
  if (b)  
    {
      bool i_set = getStackIndexFromRegister(regs, RISCV_SP_REGISTER, &i);
      num_register0 = Arch::getRegisterNumber(reg);
      int vindex;
      if ( i_set && getEffectiveStackIndex(vstack, n+i, (n == 0 ? 0 : -1 ), &vindex))
	{
	  vstack[vindex] = regs[num_register0];
	  vStackPrecision[vindex] = precision[num_register0];
	}
      else; // ????
      /*
      vstack[vindex] =  regs[num_register0];
      vStackPrecision[vindex] = precision[num_register0]; */
    }
  else
    {
      // vector < string > operands = getOperands(instructionAsm);
     cout << "*** DEBUG: RISCV_DSTORE::simulate, getRegisterAndIndexStack returns false for " <<  instructionAsm << endl;
    }
}

//---------------------------------------------
//      CALL
//
// Call instruction.
// Calls have to be in a specific category
//---------------------------------------------
void RISCV_DCALL::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
 // @1  jal ra, @call
  // ra : return address = @1+4

//  num_register0 = Arch::getRegisterNumber("v0");
//  num_register1 = Arch::getRegisterNumber("v1");
}

//---------------------------------------------
//      MOVE
//
// Transfer from register to register
//---------------------------------------------
void RISCV_MOVE::simulate(regTable & regs, regPrecisionTable & precision,  stackType &vstack, stackPrecType &vStackPrecision, const string & instructionAsm)
{
  // mv | fmv.X.Y | fmv.X    RD, RS
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  move(regs, precision);
}

//---------------------------------------------
//      KILL_OP1
//
// Classes of instruction that modify a register
// (destination operand, first in the list of operands) 
// and for which all information
// on the register content is lost.
//
// In case the instruction kills the second register
// operand and not the first one, category KILL_OP2
// should be used instead.
//---------------------------------------------
void RISCV_KillOp1::simulate(regTable & regs, regPrecisionTable & precision,  stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  killop1(regs, precision);
}

//---------------------------------------------
//      KILL_OP2
//
// Classes of instruction that modify a register
// that is the second in the list of operands,
// and for which all information
// on the register content is lost.
//
// In case the instruction kills the first register operand and not
// the second one, category KILL_OP1 should be used instead.
// ---------------------------------------------
void RISCV_KillOp2::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  killop2(regs, precision);
}

///------------------------------------------------------------
///     Arithmetic and Logical Instructions
///------------------------------------------------------------

//---------------------------------------------
//      ADD
//
// Signed addition on registers
//---------------------------------------------
void RISCV_ADD::simulate(regTable & regs, regPrecisionTable & precision,stackType &vstack, stackPrecType &vStackPrecision,   const string & instructionAsm)
{
  // add | addw | fadd.X  RD, RS1,RS2
  vector < string > operands = getOperands(instructionAsm);
  size_t vindex;

  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  num_register2 = Arch::getRegisterNumber(operands[2]);

  // swithing the operands for sp 
  if ( num_register2 == RISCV_SP_REGISTER )
    REGISTERS_SWITCH(num_register1, num_register2);

  string regOperand2 =  regs[num_register2];
  vindex=  regOperand2.find ("-");
  if (vindex != EOS)
    {
      regs[num_register2] = regOperand2.substr(vindex+1);
      minus(regs, precision);
    }
  else
    {
      add(regs, precision, true);
    }
}

//---------------------------------------------
//      ADDIU
//
// Unsigned addition of immediate to register
// (rt <- rs + immediate)
//---------------------------------------------
void RISCV_ADDIU::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  size_t vindex;

  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  num_register2 = RISCV_AUX_REGISTER;
  regs[num_register2] = operands[2];
  precision[num_register2] = true;

  string regOperand2 =  regs[num_register2];
  vindex=  regOperand2.find ("-");
  if (vindex != EOS)
    {
      regs[num_register2] = regOperand2.substr(vindex+1);
      minus(regs, precision);
    }
  else
    {
      add(regs, precision, false);
    }
}

void RISCV_SUBTRACT::simulate(regTable & regs, regPrecisionTable & precision,stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
  // sub | subw | fsub.X    RD,RS1,RS2
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  num_register2 = Arch::getRegisterNumber(operands[2]);
  minus(regs, precision);
}

///------------------------------------------------------------
///     Specific Load Instructions
///------------------------------------------------------------

///-------------------------------
///     LUI
///-------------------------------
void RISCV_LUI::simulate(regTable & regs, regPrecisionTable & precision,stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  // the lui flag is used to specify that the immediate value must be shifted left 16 bits.
  loadConstant(regs, precision, operands[1] + "lui");
}

///-------------------------------
///     LI
///-------------------------------
void RISCV_LI::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision, const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  loadConstant(regs, precision, operands[1]);
}


/// SHIFT 
// ssl R1,R2,i with i in [0,31], ssl v0,v0,0  is a nop.
void RISCV_SHIFT::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision, const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  num_register2 = RISCV_AUX_REGISTER;
  regs[num_register2] = operands[2];
  precision[num_register2] = true;
  //   num_register2 = Arch::getRegisterNumber(operands[2]);
  logical_shift_left(regs, precision);
}


void RISCV_BRANCH::simulate(regTable & regs, regPrecisionTable & precision, stackType & vstack, stackPrecType & vStackPrecision, const string & instructionAsm)
{
  DAA_TRACE("RISCV_BRANCH", instructionAsm);
};

void RISCV_NEGATE::simulate(regTable & regs, regPrecisionTable & precision, stackType & vstack, stackPrecType & vStackPrecision, const string & instructionAsm)
{
  // neg | negw | fneg.X   RD, RS
  DAA_TRACE("RISCV_NEGATE", instructionAsm);
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  mult(regs, precision);
}

  
void RISCV_LOGICAL::simulate(regTable & regs, regPrecisionTable & precision, stackType & vstack, stackPrecType & vStackPrecision, const string & instructionAsm)
{
  DAA_TRACE("RISCV_LOGICAL", instructionAsm);
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  if (voperator == "not") return op_not(regs, precision);
  
  num_register2 = Arch::getRegisterNumber(operands[2]);
  if (voperator == "and") return op_and(regs, precision);
  if (voperator == "or")  return op_or(regs, precision);
  if (voperator == "xor") return op_xor(regs, precision);
  ERROR_ACCESS("RISCV_LOGICAL_BAD_OPERATOR", instructionAsm); 
}


void RISCV_LOGICAL_I::simulate(regTable & regs, regPrecisionTable & precision, stackType & vstack, stackPrecType & vStackPrecision, const string & instructionAsm)
{
  // ANDI, ORI, XORI are logical operations that perform bitwise AND, OR, and XOR on register rs1
  // and the sign-extended 12-bit immediate and place the result in rd.
  // Note, XORI rd, rs1, -1 performs a bitwise logical inversion of register rs1 (assembler pseudo-instruction NOT rd, rs).
  DAA_TRACE("RISCV_LOGICAL_I", instructionAsm);
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  killop1(regs, precision); // NYI
  assert( (voperator == "andi") || (voperator == "ori") || (voperator == "xori"));

  /*  num_register1 = Arch::getRegisterNumber(operands[1]);
      num_register2 = RISCV_AUX_REGISTER;
      regs[num_register2] = operands[2];
      precision[num_register2] = true;
      
      // if (voperator == "andi") return op_andi(regs, precision);
      // if (voperator == "ori")  return op_ori(regs, precision);
      // if (voperator == "xoir") return op_xori(regs, precision);
      ERROR_ACCESS("RISCV_LOGICAL_I_BAD_OPERATOR", instructionAsm); 
  */
}


void RISCV_MUL::simulate(regTable & regs, regPrecisionTable & precision, stackType & vstack, stackPrecType & vStackPrecision, const string & instructionAsm)
{
  // mul | mulw | fmul.X  rd,rs1,rs2
  DAA_TRACE("RISCV_MUL", instructionAsm);
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  num_register2 = Arch::getRegisterNumber(operands[2]);
  mult(regs, precision);
}

void RISCV_DIV::simulate(regTable & regs, regPrecisionTable & precision, stackType & vstack, stackPrecType & vStackPrecision, const string & instructionAsm)
{
  // fdiv.X | div | divw rd,rs1,rs2
  DAA_TRACE("RISCV_DIV", instructionAsm);
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  num_register2 = Arch::getRegisterNumber(operands[2]);
  divi(regs, precision);
}

void RISCV_REMAINDER::simulate(regTable & regs, regPrecisionTable & precision, stackType & vstack, stackPrecType & vStackPrecision, const string & instructionAsm)
{
  // If the second operand of / or % is zero the behavior is undefined; otherwise (a/b)*b + a%b is equal to a. 
  // If both operands are nonnegative then the remainder is nonnegative; if not, the sign of the remainder is implementation-defined.
  // REMW and REMUW instructions are only valid for RV64, and provide the corresponding signed and unsigned remainder operations respectively
  // Both REMW and REMUW always sign-extend the 32-bit result to 64 bits, including on a divide by zero
  DAA_TRACE("RISCV_REMAINDER", instructionAsm);
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  num_register2 = Arch::getRegisterNumber(operands[2]);
  remainder(regs, precision);
}



void RISCV_SIGN_EXTENDED_WORD::simulate(regTable & regs, regPrecisionTable & precision, stackType & vstack, stackPrecType & vStackPrecision, const string & instructionAsm)
{
  DAA_TRACE("RISCV_SIGN_EXTENDED_WORD", instructionAsm);
  // sext.w rd, rs
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  regs[num_register0] = regs[num_register1];
  precision[num_register0] = precision[num_register1];
}


void RISCV_CONVERSION::simulate(regTable & regs, regPrecisionTable & precision, stackType & vstack, stackPrecType & vStackPrecision, const string & instructionAsm)
{
  DAA_TRACE("RISCV_CONVERSION", instructionAsm);
  // fcvt.d.l, fcvt.d.lu, fcvt.d.w,fcvt.d.s, fcvt.s.w : 2 operands
  // fcvt.w.d, fcvt.l.d,  fcvt.s.d : 3 operands (third is the rounding mode : )
  vector < string > operands = getOperands(instructionAsm);
  
  num_register0 = Arch::getRegisterNumber(operands[0]);
  if ( operands.size() == 2)
    {
      num_register1 = Arch::getRegisterNumber(operands[1]);
      regs[num_register0] = regs[num_register1];
      precision[num_register0] = precision[num_register1];
    }
  else
    // MOVE with rounding
    killop1(regs, precision); // NYI
  assert(voperator.find("fcvt.") != EOS);  
}

/* Floating-point compare instructions perform the specified comparison (equal, less than, or less
than or equal) between floating-point registers rs1 and rs2 and record the Boolean result in integer
register rd.
FLT.S and FLE.S perform what the IEEE 754-2008 standard refers to as signaling comparisons:
that is, an Invalid Operation exception is raised if either input is NaN. FEQ.S performs a quiet comparison: only signaling NaN inputs cause an Invalid Operation exception. For all three instructions,
the result is 0 if either operand is NaN. */
void RISCV_FP_COMPARE::simulate(regTable & regs, regPrecisionTable & precision, stackType & vstack, stackPrecType & vStackPrecision, const string & instructionAsm)
{
  // feq | flt | fle RD, RS1,RS2 
  DAA_TRACE("RISCV_FP_COMPARE", instructionAsm);
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  killop1(regs, precision); // NYI
}


void RISCV_SETIF::simulate(regTable & regs, regPrecisionTable & precision, stackType & vstack, stackPrecType & vStackPrecision, const string & instructionAsm)
{
  // seqz, snez, sltz, sgtz 
  // seqz rd, rs :  rd = 1 if rs = zero   <=> sltiu rd, rs, 1
  // snez rd, rs :  rd = 1 if rs != zero  <=> sltu rd, x0, rs 
  // sltz rd, rs :  rd = 1 if < zero      <=> slt rd, rs, x0  
  // sgtz rd, rs :  rd = 1 if rs> zero    <=> slt rd, x0, rs  

  DAA_TRACE("RISCV_SETIF", instructionAsm);
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  killop1(regs, precision); // NYI
}
