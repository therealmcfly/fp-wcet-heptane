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
 
    RISCV architecture definition
 
 *****************************************************************/

#ifndef ARCH_RISCV
#define ARCH_RISCV

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <vector>
#include <set>
#include <map>
#include <stdlib.h>

#include "arch.h"

using namespace std;

#define RISCV_ZERO_REGISTER 0
#define RISCV_RA_REGISTER 1 /* return address */
#define RISCV_SP_REGISTER 2
#define RISCV_GP_REGISTER 3
#define RISCV_AUX_REGISTER 64 /*  auxiliary register. */

#define RISCV_NB_REGISTERS 65

class RISCV : public Arch_dep
{
public:
    
  /*!constructor*/
  RISCV(const bool is_big_endian_p, const string &dataPath);
  
  /*!destructor*/
  ~RISCV();
  
    // Architecture endianess
    bool isBigEndian() { return is_big_endian; }

    
    /***** Objdump parsing functions *****/
    
    /*! Returns true if the line corresponds to a function declaration in the objdump file*/
    bool isFunction(const string &line);
    
    /*! Returns true if the line corresponds to an instruction in the objdump file*/
    bool isInstruction(const string &line);
    
    /*! Parser of a function declaration line*/
    ObjdumpFunction parseFunction(const string &line);
    
    string rebuiltObjdumpInstruction(const string & vcode, t_address addrinstr);

    /*! Parser of an instruction line*/
    ObjdumpInstruction parseInstruction(const string &line);
    
    /*! Returns the jump target of instr*/
    t_address getJumpDestination(const ObjdumpInstruction& instr);
    
    /***** Format functions *****/
    
    /*! Returns true if operand is a memory access pattern */
    // operand: offset (base)
    bool isMemPattern(const string& operand);

    
    /***** Useful functions *****/
    
    /*! Returns the number of instructions in the delay slot*/
    int getNBInstrInDelaySlot(){return 0;}
    
    /*! Returns the size in bytes of an instruction*/
    int getInstructionSize(){return 4;}




    /*******************************************************************
		MODIFICATIONS FOR ARM
    ********************************************************************/


    /*! Returns the names of the input registers present in operand */
    // operand: offset (base)
    vector<string> extractInputRegistersFromMem(const string& operand);

  
    /*! Returns the names of the output registers present in operand */
    // empty for RISCV
    vector<string> extractOutputRegistersFromMem(const string& operand);



    /*! Returns true if the instructions is a Load of multiple data */
    bool isLoadMultiple(const string& instr);

    /*! Returns the number of Loads in an instruction */
    int getNumberOfLoads(const string& instr);

    /*! Returns true if the instructions is a Store of multiple data */
    bool isStoreMultiple(const string& instr);

    /*! Returns the number of Stores in an instruction */
    int getNumberOfStores(const string& instr);

    /*! Returns true if instr is a return instruction*/
    bool isReturn(const ObjdumpInstruction& instr);

    /*! Returns the name of the function called in a Call Instruction */
    string getCalleeName(const ObjdumpInstruction& instr);

    /*! Getting the section in a table */
    bool getSection(ObjdumpSymbolTable & table, string &section,   vector < ObjdumpSection >::const_iterator &it);

    /*! Parse a line of the symbol table and update the ObjdumpSymbolTable object in parameter */
    void parseSymbolTableLine(const string& line, ObjdumpSymbolTable& table);
    
    /*! Returns a Word object containing all the useful information from instr */
    /*! NEVER used in RISCV */
    ObjdumpWord readWordInstruction(const ObjdumpInstruction& instr, ObjdumpSymbolTable& table);
    
    
    /*! Returns true if the instruction has PC (program counter) in input registers */
    /*! NEVER used in RISCV */
    bool isPcInInputResources(const ObjdumpInstruction& instr);

    /*! Returns true if the instruction has PC (program counter) in output registers */
    /*! NEVER used in RISCV */
    bool isPcInOutputResources(const ObjdumpInstruction& instr);
    
    
    /*! Returns the .word used by a an instruction */
    /*! NEVER used in RISCV */
    vector<ObjdumpWord> getWordsFromInstr(const ObjdumpInstruction& instr1, const ObjdumpInstruction& instr2, vector<ObjdumpWord> words, bool& is_instr2_consumed);
    
    /*! Returns true if the operand is an Input register which is written */
    /*! NEVER used in RISCV */
    bool isInputWrittenRegister(const string& operand);
    
    /*! Returns the name of the register in an Input Written Register pattern */
    /*! NEVER used in RISCV */
    string extractRegisterFromInputWrittenOperand(const string& operand);
    
    /*! Returns true if the operand is a register list */
    /*! NEVER used in RISCV */
    bool isRegisterList(const string& operand);
    
    /*! Returns the name of the registers in the list */
    /*! NEVER used in RISCV */
    vector<string> extractRegistersFromRegisterList(const string& operand);

    /*! Returns -1 */
    /*! NEVER used in RISCV */
    int getSizeRegisterList(const string& operand);

    /*! Returns true if the operand is a shifter */
    /*! NEVER used in RISCV */
    bool isShifterOperand(const string& operand);
    
    /*! Returns the name of the register which is used by the shifter (result can be empty) */
    /*! NEVER used in RISCV */
    string extractRegisterFromShifterOperand(const string& operand);

    /*! NEVER used in RISCV */

    bool getLoadStoreARMInfos(bool strongContext, string &instr, string &codeinstr, string &oregister, AddressingMode *vaddrmode, offsetType *TypeOperand, string &operand1, string &operand2, string &operand3);
    bool isPcLoadInstruction(string & instr);
    bool getInstr1ARMInfos(string &instr, string &codeinstr, string &oregister, offsetType *TypeOperand, string &operand1, string &operand2);
    bool getInstr2ARMInfos(string &instr, string &codeinstr, string &oregister, offsetType *TypeOperand, string &operand1, string &operand2, string &operand3);
    bool isARMClassInstr(const string &codeop, const string &prefix);
    bool isConditionnedARMInstr(const string &codeop, const string &prefix);
    bool getMultipleLoadStoreARMInfos(string & instr, string & codeinstr, string & oregister, vector < string > &regList, bool *writeBack);
    bool getRegisterAndIndexStack(const string &instructionAsm, string &reg, int *i);
    /***********************************************************************/
    /***********************************************************************/
    /***********************************************************************/

    int getMemoryStoreLatency();
    int getMemoryLoadLatency();
    void setMemoryStoreLatency(int val);
    void setMemoryLoadLatency(int val);
    int getPipelineFlushCost() {return 0;};

private:
    // Architecture endianness
    bool is_big_endian;

    
    /***** Pipeline analysis functions *****/
    /*! split the operands into a vector*/
    vector<string> splitOperands(const string& operands);
   
    /***** Specific functions for RISCV *****/
    
    /*! Removes useless characters of an objdump line*/
    string removeUselessCharacters(const string &line);

};

#endif
