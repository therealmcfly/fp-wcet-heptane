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

/*****************************************************************
    
                         DAAInstruction

This abstract class is a generic definition of an instruction when
used to compute the contents of registers for the determination of
addresses of load/stores.

All the concrete instructions (or rather classes of instructions) are
derived from this abstract class.
        
Instructions are grouped into different categories depending on
the way they modify registers (nop, load, store, add, move,
etc). There is a concrete instruction class corresponding to each
category.

The correspondance between instruction names (mnemonics in asm code)
and instruction categories for address analysis (DAAInstruction *) is
done in InstructionType stored in Arch_dep (see files in 
ArchitectureDependent directory).

Rq: The implementation is done for RISCV architecture only !!!
 
*****************************************************************/
#include <vector>
#include <string>
#include <iostream>
#include "DAAInstruction.h"

using namespace std;


class DAAInstruction_RISCV:public DAAInstruction
{
 protected:
  void add(regTable & regs, regPrecisionTable & precision, bool bAugmentPrecision);
  void minus(regTable & regs, regPrecisionTable & precision);
  bool getRegisterAndIndexStack(const string &instructionAsm, string &reg, int *i);
};

///-------------------------------
///     Abstract class
///-------------------------------
#ifndef _DAAINSTRUCTION_RISCV
#define _DAAINSTRUCTION_RISCV

/********************************************
 *
 * Interface of classes of instructions
 *
 *******************************************/



//--------------------------------------------
//      NOP
//
// Classes of instructions that neither produce nor
// destroy contents of registers -> no impact on
// address analysis (Nop, ...)
//--------------------------------------------
class RISCV_NOP:public DAAInstruction
{
  public:void simulate (vector < string > &, vector < bool > &,stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};


//---------------------------------------------
//      LOAD
//
// Classes of instruction that load information from memory
//---------------------------------------------
class RISCV_DLOAD:public DAAInstruction_RISCV
{
  public:void simulate (vector < string > &, vector < bool > &,stackType &vstack, stackPrecType &vStackPrecision,   const string &);
};


//---------------------------------------------
//      STORE
// 
// Classes of instruction that store information to memory (referencing sp)
//---------------------------------------------
class RISCV_DSTORE:public DAAInstruction_RISCV
{
 public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

//---------------------------------------------
//      CALL
//
// Call instruction
// Calls have to be in a specific category
// because in the RISCV they kill the registers
// used for the return values of functions
//---------------------------------------------
class RISCV_DCALL:public DAAInstruction
{
  public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

//---------------------------------------------
//      MOVE
//
// Transfer from register to register
//---------------------------------------------
class RISCV_MOVE:public DAAInstruction
{
  public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

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
class RISCV_KillOp1:public DAAInstruction
{
  public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

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
class RISCV_KillOp2:public DAAInstruction
{
  public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

///------------------------------------------------------------
///     Arithmetic and Logical Instructions
///------------------------------------------------------------

//---------------------------------------------
//      ADD
//
// Signed addition on registers
//---------------------------------------------
class RISCV_ADD:public DAAInstruction_RISCV
{
  public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

//---------------------------------------------
//      ADDIU
//
// Unsigned addition of immediate to register
// (rt <- rs + immediate)
//---------------------------------------------
class RISCV_ADDIU:public DAAInstruction_RISCV
{
  public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

//---------------------------------------------
//      SUBU
//
// Unsigned substraction between registers
//---------------------------------------------
class RISCV_SUBTRACT:public DAAInstruction
{
  public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

///------------------------------------------------------------
///     Specific Load Instructions
///------------------------------------------------------------

///-------------------------------
///     LUI (Load Upper Immediate)
///-------------------------------
class RISCV_LUI:public DAAInstruction
{
  public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

///-------------------------------
///     LI (Load Immediate)
///-------------------------------
class RISCV_LI:public DAAInstruction
{
  public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

///-------------------------------
///     SHIFT (sll)
///-------------------------------
class RISCV_SHIFT:public DAAInstruction
{
  public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};


/// Defined but nothing to do ???
class RISCV_BRANCH:public DAAInstruction
{
  public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

class  RISCV_LOGICAL:public DAAInstruction
{
  public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

class  RISCV_LOGICAL_I:public DAAInstruction
{
  public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

class  RISCV_MUL:public DAAInstruction
{
 public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

class  RISCV_DIV:public DAAInstruction
{
 public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

class RISCV_NEGATE:public DAAInstruction
{
  public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

class  RISCV_REMAINDER:public DAAInstruction
{
 public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};


class RISCV_SIGN_EXTENDED_WORD:public DAAInstruction
{
 public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};


class RISCV_CONVERSION:public DAAInstruction
{
 public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};

class RISCV_FP_COMPARE:public DAAInstruction
{
 public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};


class RISCV_SETIF:public DAAInstruction
{
 public:void simulate (vector < string > &, vector < bool > &, stackType &vstack, stackPrecType &vStackPrecision,  const string &);
};




#endif
