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

#include "MSP430.h"
#include "Logger.h"
#include "DAAInstruction_MSP430.h"
#include "Utl.h"

using namespace std;


//--------------------------------------------
//      NOP OKAY
//
// Classes of instructions that neither produce nor
// destroy contents of registers -> no impact on
// address analysis (Nop, ...)
//--------------------------------------------
void
MSP_NOP::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
}

//---------------------------------------------
//      CALL 
//
// Call instruction
// Calls have to be in a specific category
// because in the MIPS they kill the registers
// used for the return values of functions
//---------------------------------------------
void MSP_CALL::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
  num_register0 = Arch::getRegisterNumber("v0");
  num_register1 = Arch::getRegisterNumber("v1");

  // Contents of registers $v0 and $v1, used to return function results,
  // are possibly destroyed, which is reflected in regs and precision
  killop1(regs, precision);
  killop2(regs, precision);
}


//---------------------------------------------
//      MOVE 
//
// Transfer from register to register
//---------------------------------------------
void MSP_MOVE::simulate(regTable & regs, regPrecisionTable & precision,  stackType &vstack, stackPrecType &vStackPrecision, const string & instructionAsm)
{
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
void MSP_KILLOP1::simulate(regTable & regs, regPrecisionTable & precision,  stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  killop1(regs, precision);
}


///------------------------------------------------------------
///     Arithmetic and Logical Instructions
///------------------------------------------------------------

//---------------------------------------------
//      ADD 
//
// Signed addition on registers
//---------------------------------------------
void MSP_ADD::simulate(regTable & regs, regPrecisionTable & precision,stackType &vstack, stackPrecType &vStackPrecision,   const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  add(regs, precision, true);
}


//---------------------------------------------
//      SUB 
//
// Signed substraction between registers
//---------------------------------------------
void MSP_SUB::simulate(regTable & regs, regPrecisionTable & precision,stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  minus(regs, precision);
}


///------------------------------------------------------------
///     Specific Load Instructions
///------------------------------------------------------------

///-------------------------------
///     SHIFT  
///-------------------------------
void MSP_SHIFT::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision, const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  num_register2 = MSP430_AUX_REGISTER;
  regs[num_register2] = operands[2];
  precision[num_register2] = true;
  //   num_register2 = Arch::getRegisterNumber(operands[2]);
  logical_shift_left(regs, precision);
}
