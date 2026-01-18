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

Rq: The implementation is done for FLEXPRET architecture only !!!

*****************************************************************/
#include <vector>
#include <string>
#include <iostream>
#include "DAAInstruction.h"

using namespace std;

class DAAInstruction_FLEXPRET : public DAAInstruction
{
public:
	// This is the specific hook required by PipelineAnalysis.cc
	virtual unsigned int getExecutionTime(Arch *arch);

protected:
	void add(regTable &regs, regPrecisionTable &precision, bool bAugmentPrecision);
	void minus(regTable &regs, regPrecisionTable &precision);
	bool getRegisterAndIndexStack(const string &instructionAsm, string &reg, int *i);
};

///-------------------------------
///     Abstract class
///-------------------------------
#ifndef _DAAINSTRUCTION_FLEXPRET
#define _DAAINSTRUCTION_FLEXPRET

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
class FLEXPRET_NOP : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

//---------------------------------------------
//      LOAD
//
// Classes of instruction that load information from memory
//---------------------------------------------
class FLEXPRET_DLOAD : public DAAInstruction_FLEXPRET
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

//---------------------------------------------
//      STORE
//
// Classes of instruction that store information to memory (referencing sp)
//---------------------------------------------
class FLEXPRET_DSTORE : public DAAInstruction_FLEXPRET
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

//---------------------------------------------
//      CALL
//
// Call instruction
// Calls have to be in a specific category
// because in the FLEXPRET they kill the registers
// used for the return values of functions
//---------------------------------------------
class FLEXPRET_DCALL : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

//---------------------------------------------
//      MOVE
//
// Transfer from register to register
//---------------------------------------------
class FLEXPRET_MOVE : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
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
class FLEXPRET_KillOp1 : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
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
class FLEXPRET_KillOp2 : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

///------------------------------------------------------------
///     Arithmetic and Logical Instructions
///------------------------------------------------------------

//---------------------------------------------
//      ADD
//
// Signed addition on registers
//---------------------------------------------
class FLEXPRET_ADD : public DAAInstruction_FLEXPRET
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

//---------------------------------------------
//      ADDIU
//
// Unsigned addition of immediate to register
// (rt <- rs + immediate)
//---------------------------------------------
class FLEXPRET_ADDIU : public DAAInstruction_FLEXPRET
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

//---------------------------------------------
//      SUBU
//
// Unsigned substraction between registers
//---------------------------------------------
class FLEXPRET_SUBTRACT : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

///------------------------------------------------------------
///     Specific Load Instructions
///------------------------------------------------------------

///-------------------------------
///     LUI (Load Upper Immediate)
///-------------------------------
class FLEXPRET_LUI : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

///-------------------------------
///     LI (Load Immediate)
///-------------------------------
class FLEXPRET_LI : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

///-------------------------------
///     SHIFT (sll)
///-------------------------------
class FLEXPRET_SHIFT : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

/// Defined but nothing to do ???
class FLEXPRET_BRANCH : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

class FLEXPRET_LOGICAL : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

class FLEXPRET_LOGICAL_I : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

class FLEXPRET_MUL : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

class FLEXPRET_DIV : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

class FLEXPRET_NEGATE : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

class FLEXPRET_REMAINDER : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

class FLEXPRET_SIGN_EXTENDED_WORD : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

class FLEXPRET_CONVERSION : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

class FLEXPRET_FP_COMPARE : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

class FLEXPRET_SETIF : public DAAInstruction
{
public:
	void simulate(vector<string> &, vector<bool> &, stackType &vstack, stackPrecType &vStackPrecision, const string &);
};

#endif
