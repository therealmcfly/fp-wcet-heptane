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

#include "DAAInstruction_MIPS.h"
#include "MIPS.h"
#include "arch.h"
#include "Utl.h"

using namespace std;

#define NOT_YET_IMPLEMENTED "--- Not yet implemented ::simulate "

#define LOCTRACE(s)

//--------------------------------------------
//      NOP
//
// Classes of instructions that neither produce nor destroy contents of 
// registers -> no impact on address analysis
//--------------------------------------------
void
Nop::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
}


//---------------------------------------------
//      LOAD
//
// Classes of instruction that load information from memory
//
//---------------------------------------------
void DLoad::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
  bool b;
  int i,n;
  string reg;
  int vindex;

  b = Arch::getRegisterAndIndexStack(instructionAsm, reg, &n);
  if (b) 
    {
      bool i_set = getStackIndexFromRegister(regs, Arch::getRegisterNumber("sp") , &i);
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
void DStore::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision,   const string & instructionAsm)
{
  bool b;
  int i, n;
  string reg;

  /* example : sw ra,20(sp)  */
  b = Arch::getRegisterAndIndexStack(instructionAsm, reg, &n);
  if (b)  
    {
      bool i_set = getStackIndexFromRegister(regs, Arch::getRegisterNumber("sp") , &i);
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
}

//---------------------------------------------
//      CALL
//
// Call instruction.
// Calls have to be in a specific category
// because in the MIPS they kill the registers
// used for the return values of functions
//---------------------------------------------
void DCall::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
  // removed by LBesnard, because:
  // 4005b4:	0c1004d2 	jal	401348 <filtez>
  // 4005b8:	244525f0 	addiu	a1,v0,9712 : executed before the call

  /*
  num_register0 = Arch::getRegisterNumber("v0");
  num_register1 = Arch::getRegisterNumber("v1");

  // Contents of registers $v0 and $v1, used to return function results,
  // are possibly destroyed, which is reflected in regs and precision
  killop1(regs, precision);
  killop2(regs, precision);
  */
}

//---------------------------------------------
//      MOVE
//
// Transfer from register to register
//---------------------------------------------
void Move::simulate(regTable & regs, regPrecisionTable & precision,  stackType &vstack, stackPrecType &vStackPrecision, const string & instructionAsm)
{
  // mtc1 $t0, $f0      # f0 = t0   Note that destination is *second* reg.
  size_t vindex = instructionAsm.find ("mtc1");
  vector < string > operands = getOperands(instructionAsm);
  if (vindex == EOS) 
    {
      num_register0 = Arch::getRegisterNumber(operands[0]);
      num_register1 = Arch::getRegisterNumber(operands[1]);
    }
  else
    {
      num_register1 = Arch::getRegisterNumber(operands[0]);
      num_register0 = Arch::getRegisterNumber(operands[1]);

    }
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
void KillOp1::simulate(regTable & regs, regPrecisionTable & precision,  stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
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
void KillOp2::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
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
void Add::simulate(regTable & regs, regPrecisionTable & precision,stackType &vstack, stackPrecType &vStackPrecision,   const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  size_t vindex;
  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  num_register2 = Arch::getRegisterNumber(operands[2]);

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
void Addiu::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  size_t vindex;

  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  num_register2 = MIPS_AUX_REGISTER;
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
      add(regs, precision, true);
    }
}

//---------------------------------------------
//      SUBU
//
// Unsigned substraction between registers
//---------------------------------------------
//TODO:2nd case: regs[num_register0]= "-" + operand2;
/* to be added if needed (should be similar to the add simulate function)
void Subu::simulate(vector<string>& regs, vector<bool>& precision, stackType &vstack, stackPrecType &vStackPrecision,  const string& instructionAsm)
{
    vector<string> operands= getOperands(instructionAsm);
    
    int num_register0 = MapRegistersMIPS::getRegisterNumber(operands[0]);
    int num_register1 = MapRegistersMIPS::getRegisterNumber(operands[1]);
    int num_register2 = MapRegistersMIPS::getRegisterNumber(operands[2]);

    string operand1=regs[num_register1];
    string operand2=regs[num_register2];
        
    if(operand1=="*" && operand2=="*")
    {
        regs[num_register0]="*";
    }
    else if(operand1=="*")
    {
        regs[num_register0]= "-" + operand2; // for induction variable
    }
    else if(operand2=="*")
    {
        regs[num_register0]=operand1; // for induction variable
    }
    else
    {
        regs[num_register0] = operand1 + " - " + operand2;
    }
	
    precision[num_register0]= precision[num_register1] && precision[num_register2];
}
*/

void Subu::simulate(regTable & regs, regPrecisionTable & precision,stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
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
void Lui::simulate(regTable & regs, regPrecisionTable & precision,stackType &vstack, stackPrecType &vStackPrecision,  const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  // the lui flag is used to specify that the immediate value must be shifted left 16 bits.
  loadConstant(regs, precision, operands[1] + "lui");
}

///-------------------------------
///     LI
///-------------------------------
void Li::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision, const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  loadConstant(regs, precision, operands[1]);
}


/// SHIFT 
//  ssl R1,R2,i with i in [0,31], ssl v0,v0,0  is a nop.
//  sll, srl, sllv, srlv sra
void Shift::simulate(regTable & regs, regPrecisionTable & precision, stackType &vstack, stackPrecType &vStackPrecision, const string & instructionAsm)
{
  vector < string > operands = getOperands(instructionAsm);
  num_register0 = Arch::getRegisterNumber(operands[0]);
  num_register1 = Arch::getRegisterNumber(operands[1]);
  num_register2 = MIPS_AUX_REGISTER;
  if (  (voperator == "sll") || (voperator == "srl") || (voperator == "sra") )
    { 
      long v = Utl::hexa2dec(operands[2]) ;
      regs[num_register2] = Utl::int2cstring(v);
    }
  else
    regs[num_register2] = operands[2];


  precision[num_register2] = true;
  //   num_register2 = Arch::getRegisterNumber(operands[2]);
  if (voperator == "sll")
    logical_shift_left(regs, precision);
  else
  if (voperator == "srl")
    logical_shift_right(regs, precision);
  else
    if (voperator == "sra")
      arithmetic_shift_right(regs, precision);
    else
      if (voperator == "sllv")  
	// Shift left logical variable: TODO
	killop1(regs, precision);
      else
	if (voperator == "srlv")
	  // Shift right logical variable: TODO
	  killop1(regs, precision);
	else
	  if (voperator == "srav")
	    killop1(regs, precision);
	  else 
	    ERROR_ACCESS("MIPS_SHIFT_BAD_OPERATOR", instructionAsm); 
}
