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

#include "MSP430.h"
#include "DAAInstruction.h"

///-------------------------------
///     Abstract class
///-------------------------------

#ifndef DAAINSTRUCTION_MSP430
#define DAAINSTRUCTION_MSP430

/********************************************
 *
 * Interface of classes of instructions
 *
 *******************************************/
 
//--------------------------------------------
//      REMINDER DAAInstruction
//
// Definition of paramters for instructions
// typedef vector < bool > regPrecisionTable;
// typedef vector < string > regTable;
// typedef vector < string > stackType;
// typedef vector < bool > stackPrecType;
//--------------------------------------------

//--------------------------------------------
//      NOP OKAY
//
// Classes of instructions that neither produce nor
// destroy contents of registers -> no impact on
// address analysis (Nop, ...)
//--------------------------------------------
class MSP_NOP:public DAAInstruction
{
  public:void simulate (regTable &regs, regPrecisionTable &precision, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

//---------------------------------------------
//      CALL OKAY
//
// Call instruction
// Calls have to be in a specific category
// because in the MIPS they kill the registers
// used for the return values of functions
//---------------------------------------------
class MSP_CALL:public DAAInstruction
{
  public:void simulate (regTable &regs, regPrecisionTable &precision, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

//---------------------------------------------
//      MOVE OKAY
//
// Transfer from register to register
//---------------------------------------------
class MSP_MOVE:public DAAInstruction
{
  public:void simulate (regTable &regs, regPrecisionTable &precision, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

//---------------------------------------------
//      KILL_OP1 OKAY
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
class MSP_KILLOP1:public DAAInstruction
{
  public:void simulate (regTable &regs, regPrecisionTable &precision, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

///------------------------------------------------------------
///     Arithmetic and Logical Instructions
///------------------------------------------------------------

//---------------------------------------------
//      ADD OKAY
//
// Signed addition on registers
//---------------------------------------------
class MSP_ADD:public DAAInstruction
{
  public:void simulate (regTable &regs, regPrecisionTable &precision, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

//---------------------------------------------
//      SUB OKAY
//
// Signed substraction between registers
//---------------------------------------------
class MSP_SUB:public DAAInstruction
{
  public:void simulate (regTable &regs, regPrecisionTable &precision, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

///------------------------------------------------------------
///     Specific Load Instructions
///------------------------------------------------------------

///-------------------------------
///     SHIFT (sll) OKAY
///-------------------------------
class MSP_SHIFT:public DAAInstruction
{
  public:void simulate (regTable &regs, regPrecisionTable &precision, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

#endif



