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
   
#ifndef ARCH_MSP430
#define ARCH_MSP430

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

#define MSP430_ZERO_REGISTER 3
#define MSP430_SP_REGISTER 1
#define MSP430_GP_REGISTER 0
// #define MIPS_RA_REGISTER 31 /* return address . */
#define MSP430_NB_REGISTERS 16
#define MSP430_AUX_REGISTER 15

class MSP430 : public Arch_dep
{
public:
  
  /*!constructor*/
  MSP430(const bool is_big_endian_p, const string & dataPath);
  
  /*!destructor*/
  ~MSP430();
  
    // Architecture endianess
    bool isBigEndian() {return is_big_endian;}
    
    
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
    
    /*! Returns the names of the input registers present in operand */
    // operand: offset (base)
    vector<string> extractInputRegistersFromMem(const string& operand);

    /*! Returns true if instr is a return instruction*/
    bool isReturn(const ObjdumpInstruction& instr);

    /*! Returns the name of the function called in a Call Instruction */
    string getCalleeName(const ObjdumpInstruction& instr);
    
    bool getRegisterAndIndexStack(const string &instructionAsm, string &reg, int *i);


    /*******************************************************************
		MODIFICATIONS FOR ARM/ NEVER USED IN MSP430
    ********************************************************************/

    /*! Returns the names of the output registers present in operand */
    // empty for MIPS
    vector<string> extractOutputRegistersFromMem(const string& operand);

    /*! Returns true if the instructions is a Load of multiple data */
    bool isLoadMultiple(const string& instr);

    /*! Returns the number of Loads in an instruction */
    int getNumberOfLoads(const string& instr);

    /*! Returns true if the instructions is a Store of multiple data */
    bool isStoreMultiple(const string& instr);

    /*! Returns the number of Stores in an instruction */
    int getNumberOfStores(const string& instr);

    /*! Parse a line of the symbol table and update the ObjdumpSymbolTable object in parameter */
    void parseSymbolTableLine(const string& line, ObjdumpSymbolTable& table);
    
    /*! Returns a Word object containing all the useful information from instr */
    ObjdumpWord readWordInstruction(const ObjdumpInstruction& instr, ObjdumpSymbolTable& table);
    
    
    /*! Returns true if the instruction has PC (program counter) in input registers */
    bool isPcInInputResources(const ObjdumpInstruction& instr);

    /*! Returns true if the instruction has PC (program counter) in output registers */
    bool isPcInOutputResources(const ObjdumpInstruction& instr);
    
    
    /*! Returns the .word used by a an instruction */
    vector<ObjdumpWord> getWordsFromInstr(const ObjdumpInstruction& instr1, const ObjdumpInstruction& instr2, vector<ObjdumpWord> words, bool& is_instr2_consumed);
    
    /*! Returns true if the operand is an Input register which is written */
    bool isInputWrittenRegister(const string& operand);
    
    /*! Returns the name of the register in an Input Written Register pattern */
    string extractRegisterFromInputWrittenOperand(const string& operand);
    
    /*! Returns true if the operand is a register list */
    bool isRegisterList(const string& operand);
    
    /*! Returns the name of the registers in the list */
    vector<string> extractRegistersFromRegisterList(const string& operand);

    /*! Returns -1 */
    int getSizeRegisterList(const string& operand);

    /*! Returns true if the operand is a shifter */
    bool isShifterOperand(const string& operand);
    
    /*! Returns the name of the register which is used by the shifter (result can be empty) */
    string extractRegisterFromShifterOperand(const string& operand);
    uint32_t getStackFrameReservedSize(const string &instr);

    bool getLoadStoreARMInfos(bool strongContext, string &instr, string &codeinstr, string &oregister, AddressingMode *vaddrmode, offsetType *TypeOperand, string &operand1, string &operand2, string &operand3);
    bool isPcLoadInstruction(string & instr);
    bool getInstr1ARMInfos(string &instr, string &codeinstr, string &oregister, offsetType *TypeOperand, string &operand1, string &operand2);
    bool getInstr2ARMInfos(string &instr, string &codeinstr, string &oregister, offsetType *TypeOperand, string &operand1, string &operand2, string &operand3);
    bool isARMClassInstr(const string &codeop, const string &prefix);
    bool isConditionnedARMInstr(const string &codeop, const string &prefix);
    bool getMultipleLoadStoreARMInfos(string & instr, string & codeinstr, string & oregister, vector < string > &regList, bool *writeBack);
    /***********************************************************************/
    /***********************************************************************/
    /***********************************************************************/
    
    
    /***** Useful functions *****/
    
    /*! Returns the number of instructions in the delay slot*/
    int getNBInstrInDelaySlot(){return 0;}
    
    /*! Returns the size in bytes of an instruction*/
    int getInstructionSize(){return 4;}
    
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
    

    /***** Specific functions for MSP430 *****/
    
    /*! Removes useless characters of an objdump line*/
    string removeUselessCharacters(const string &line);

    /*! Removes useless characters of a MemPattern*/
    string removeUselessCharactersMemPattern(const string &line);
    
};

#endif






 
