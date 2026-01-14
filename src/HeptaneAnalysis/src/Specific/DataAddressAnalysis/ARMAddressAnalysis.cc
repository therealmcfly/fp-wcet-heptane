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

#include "ARMAddressAnalysis.h"
#include "Utl.h"
#include "ARM.h"
#include "arch.h"

#define TRACE_STACK(s)
#define TRACE_setLoadStoreAddressAttribute(S)

int ARMAddressAnalysis::getStackSize(Instruction * vinstr)
{

  // assumed : the stack is managed at the begining of a cfg.
  // it may have several sub sp, sp, Val
  
  // Split the first instruction 
  vector < string > v_instr = Arch::splitInstruction(vinstr->GetCode());
  string codeop = v_instr[0];
  if (codeop == "sub")		// allocating locals sub sp, sp, VALUE
    {
      if (v_instr[1] != "sp") return 0;
      if (v_instr[2] != "sp") return 0;
      assert(Utl::isDecNumber(v_instr[3]));
      int vsize = atoi(v_instr[3].c_str());
      return abs(vsize);
    }

  // No occurrence (they are replaced)
  assert(codeop.find("push") == EOS);
  assert(codeop.find("stm") == EOS);
  assert(codeop.find("push") == EOS);
  assert(codeop.find("pop") == EOS);
  return 0;
}

/**
   It may have several : sub sp, sp, VALUE
*/
int ARMAddressAnalysis::getStackSize(const vector < Instruction * >&listInstr, int * ifrom)
{
  int nb=0, nbc;
  size_t i= *ifrom;
  while (i <  listInstr.size())
    {
      nbc = getStackSize(listInstr[i]);
      if (nbc == 0) 
	{
	  *ifrom = i;
	  return nb;
	}
      nb = nb + nbc;
      i++;
    }
  *ifrom = i;
  return nb;
}

int ARMAddressAnalysis::getStackSize(Cfg * cfg)
{
  int i, stackSize = 0;
  Instruction *vinstr;
  Node *firstNode = cfg->GetStartNode();
  const vector < Instruction * >&listInstr = firstNode->GetAsm();
  int n = listInstr.size();

  TRACE(cout << " DEBUG__LB ARMAddressAnalysis::getStackSize (" << cfg->getStringName() << ")" << endl);
  assert(n != 0);
  i=0;
  bool oncontinue = true;
  while(oncontinue && (i <n))
    {
      vinstr = listInstr[i];
      oncontinue = vinstr->HasAttribute(MetaInstructionAttributeName); // push, replaced by str, sub sp, sp, val
      if (oncontinue)
	{
	  stackSize = stackSize + getStackSize(vinstr);
	  i++;
	}
      else
	{
	  stackSize = stackSize + getStackSize(listInstr, &i);
	  if (i<n)
	    {
	      vinstr = listInstr[i];
	      oncontinue = vinstr->HasAttribute(MetaInstructionAttributeName); 
	    }
	}
    }
  TRACE(cout << " DEBUG__LB ARMAddressAnalysis::getStackSize() = " << stackSize << endl);
  // a leaf in the call graph (no dedicated stack frame in ARM) is accepted.
  if (stackSize == 0) assert(!isCfgWithCallNode(cfg));
  return stackSize;
}

//  Waiting for  mnemonic regout [regin, val]
bool ARMAddressAnalysis::extractRegVal(string instr, string & reg, string & val)
{
  vector < string > v_instr = Arch::splitInstruction(instr);
  assert(v_instr.size() == 3);	// mnemonic op1, [xxx]
  string mem_pattern = v_instr[2];
  if (!Utl::replace(mem_pattern, '[', ' '))
    return false;
  if (!Utl::replace(mem_pattern, ']', ' '))
    return false;
  Utl::replace(mem_pattern, ',', ' ');
  val = "";			// case "codeop [sp]"
  istringstream parse(mem_pattern);
  parse >> reg >> val;
  return true;
}

long ARMAddressAnalysis::GetOffsetValue(string asm_code)
{
  string reg, val;
  assert(extractRegVal(asm_code, reg, val));
  return Utl::string2long(val);
}

int ARMAddressAnalysis::getStackMaxOffset(string s_instr, int StackMaxOffset)
{
  // ldr/str [sp, val]
  size_t ideb = s_instr.find("[sp,");
  if (ideb == EOS) return StackMaxOffset;
  s_instr = s_instr.substr(ideb+5);
  size_t iend = s_instr.find("]");
  s_instr = s_instr.substr(0,iend);
  return Utl::imax(abs(Utl::string2int(s_instr)), StackMaxOffset);
}


bool ARMAddressAnalysis::analyzeStack(Cfg * cfg, Instruction * Instr, long offset, string access, int sizeOfMemoryAccess, bool precision, Context * context)
{
  long stack_size, addr, size;

  //get the StackInfoAttributeName of current cfg
  assert(cfg->HasAttribute(StackInfoAttributeName));
  StackInfoAttribute attribute = (StackInfoAttribute &) cfg->GetAttribute(StackInfoAttributeName);
  
  // get the stack information of cfg
  stack_size = attribute.getFrameSizeWithoutCaller();
  long stack_maxoffset = attribute.getFrameSizeWithCaller();
  
  long sp = attribute.getSP(context->getStringId()); // address in the stack
  TRACE_STACK( cout << "analyzeStack, context --sp = " << sp  << "(10), " << std::hex << sp << std::dec << "(16)" << " stack size = " << stack_size << endl; );

  if (offset < 0) offset=-offset;  // the offset may be < 0 for a metainstruction (push, ldmxx)

  long velem = (sp + offset);  
  if ( (velem < sp) ||  (velem > spinit))
    {
      cout << " PROBLEM: bad stack access, sp = " << sp << ", offset = " << offset << " and stack address = " << spinit << endl;
      Logger::addError("Stack analysis -> out of stack.");
      return false;
    }

  bool newImpl = true;
  if (newImpl)
    {
      if (precision)
	{
	  addr = sp + offset;
	  size = sizeOfMemoryAccess;
	}
      else
	{
	  addr = sp;
	  size = spinit - sp; // all elements of the stack.
	}
    }
  else
    { 
      if (stack_maxoffset + 1 < offset) // +1 ???
	{
	  cout << " PROBLEM: stack_maxoffset  = " <<    stack_maxoffset << " and offset = " << offset << endl;
	  Logger::addError("stack analysis -> maxOffset < offset");
	  return false;
	}
      
      long stack_maxoffset_caller;
      
      //check that the stack size is equal to the caller stack size
      //if the function is without stack frame (leaf of the context tree)
      //and keep the stack_maxoffset of the caller in case !precision
      if (stack_size == 0)
	{
	  Context *context_caller = context->getCallerContext();
	  assert(context_caller != NULL);
	  Cfg *caller = context_caller->getCurrentFunction();
	  assert(caller->HasAttribute(StackInfoAttributeName));
	  StackInfoAttribute caller_attribute = (StackInfoAttribute &) caller->GetAttribute(StackInfoAttributeName);
	  long sp_caller = caller_attribute.getSP(context_caller->getStringId());
	  stack_maxoffset_caller = caller_attribute.getFrameSizeWithCaller();
	  assert(sp == sp_caller);
	}
      
      if (precision)
	{
	  addr = sp + offset;
	  size = sizeOfMemoryAccess;
	}
      else
	{
	  addr = sp;
	  // TODO: check the ARM ABI if the parameters of the caller can be accessed if not stack_size_caller can be used instead
	  if (stack_size == 0)
	    size = stack_maxoffset_caller;
	  else
	    size = stack_maxoffset;
	}
    }
  
 AddressInfo contextual_info = mkAddressInfo(Instr, access, precision, "stack", "", addr, size);
 setContextualAddressAttribute(Instr, context, contextual_info);
 return true;
}

// set the AddressAttribute for a gp or lui access; 
// local to setLoadStoreAddressAttribute
void ARMAddressAnalysis::analyzeReg(Instruction * Instr, long addr, string access, int sizeOfMemoryAccess, bool precision, RegState * state)
{
  string var_name;
  unsigned long start_addr = 0;
  int size = 0;
  string section_name;

  if (! symbol_table.getInfo(addr, &var_name, &start_addr, &size, &section_name))
    {
      long addrInstruction;
      InstructionARM::getCodeAddress(Instr, &addrInstruction);
      cout << " ARMAddressAnalysis::analyzeReg NOT FOUND for " <<  Instr->GetCode() <<  ", addr = " << addr << " at " <<  std::hex <<  addrInstruction  <<  std::dec << endl;
      state->printStates();// state->printStack();
      cout << " ----------------- " << endl;
    }

  if (start_addr == 0 || size == 0)
    {
      Logger::addError("reg analysis -> access to unknown area");
    }
  else
    {
      mkAddressInfoAttribute(Instr, access, precision, section_name, var_name, (precision ? addr : start_addr), (precision ? sizeOfMemoryAccess : size));
    }
}

// Setting the address attribute for load and Store instructions (Memory instructions)
// @return true when it is applied, false otherwise.
// ***WARNING***: push, pop, ldm, stm are currently replaced in the HeptaneExtract step.
bool ARMAddressAnalysis::setLoadStoreAddressAttribute(Cfg * vCfg, Instruction * vinstr, RegState * state, Context * context)
{
  string reg, codeinstr, access, asm_code;
  vector < string > regList;
  long loffset;

  asm_code = vinstr->GetCode();
  bool isLoad = Arch::isLoad(asm_code);
  bool isStore = Arch::isStore(asm_code);

  if (!isLoad && !isStore)
    return false;

  // get the size of the access according to the op code
  int sizeOfMemoryAccess = Arch::getSizeOfMemoryAccess(asm_code);

  if (isLoad) access = "read"; else access = "write";	

  TRACE_setLoadStoreAddressAttribute(
     long vaddrxx; InstructionARM::getCodeAddress(vinstr, &vaddrxx);
     cout << "Begin setLoadStoreAddressAttribute, instr = " << asm_code << " at " <<  std::hex << vaddrxx << std::dec << endl;
     state->printStates();state->printStack(); );
 
  // access using $sp register
  if (asm_code.find("sp") != EOS)
    {
      loffset = GetOffsetValue(asm_code);
      TRACE(cout << "ARMAddressAnalysis::setLoadStoreAddressAttribute (sp) instr = " << asm_code << ", loffset= " << loffset << " , sizeOfMemoryAccess= " << sizeOfMemoryAccess << endl);
      if (! analyzeStack(vCfg, vinstr, loffset, access, sizeOfMemoryAccess, true, context))
	printInstrMessage(" *** ERROR analyzeStack for instruction, case 1 = ", vinstr);
    }
  else
    {
      vector < string > access_pattern;
      state->accessAnalysis(vinstr, access_pattern);

      TRACE(cout << "  === state->accessAnalysis returns, code = " << access_pattern[0] << endl);
      TRACE(cout << "  === state->accessAnalysis returns, value = " << access_pattern[1] << endl);
      TRACE(cout << "  === state->accessAnalysis returns, precision = " << access_pattern[2] << endl);
      TRACE(if (access_pattern.size() == 4) cout << "  === state->accessAnalysis returns,  4th operand = " << access_pattern[3] << endl) ;
      TRACE(if (access_pattern.size() == 5) cout << "  === state->accessAnalysis returns,  5th operand = " << access_pattern[4] << endl) ;

      string p0 = access_pattern[0];
      bool prec = access_pattern[2] == "1";
      // access using $reg different from $sp
      // Example ldr R, [pc, #val] et MEM[pc + val] is an immediate value ( load a immediate value)
      if (p0 == "immWord")
	{
	  string s_addr = access_pattern[3];
	  long addr = Utl::string2long(s_addr);;
	  TRACE(cout << "  === state->accessAnalysis returns, address of the IMMEDIATE VALUE = " << s_addr << endl);
	  mkAddressInfoAttribute(vinstr, access, prec, ".data", "", addr, sizeOfMemoryAccess);
	}
      else if (p0 == "lui")
	{
	  long addr = Utl::string2long(access_pattern[1]);
	  TRACE(cout << "  === state->accessAnalysis returns, address of the LUI= " <<  std::hex << addr << std::dec << endl;);
	  analyzeReg(vinstr, addr, access, sizeOfMemoryAccess, prec, state);
	}
      else if (p0 == "gp")
	{
	  long addr = Utl::string2long(access_pattern[1]);
	  analyzeReg(vinstr, addr, access, sizeOfMemoryAccess, prec, state);
	}
      else if (p0 == "sp")
	{
	  loffset = Utl::string2long(access_pattern[1]);
	  TRACE(cout << "ARMAddressAnalysis::setLoadStoreAddressAttribute CASE 2 (sp) instr = " << asm_code << ", loffset= " << loffset << " , sizeOfMemoryAccess= " << sizeOfMemoryAccess << endl;);
	  if (! analyzeStack(vCfg, vinstr, loffset, access, sizeOfMemoryAccess, prec, context))
	    printInstrMessage(" *** ERROR analyzeStack for instruction, case 2, value = " + access_pattern[1]+ " " , vinstr);
	}
      else //- unknown- pointer: all addresses can be accessed (stub)
	{
	  TRACE(cout << " POINTER ----*****------" << asm_code << endl;);
	  setPointerAccessInfo(vinstr, access);
	}
    }


  TRACE_setLoadStoreAddressAttribute(
     cout << " END setLoadStoreAddressAttribute" << endl;
     state->printStates();state->printStack(); );

  return true;
}

RegState *ARMAddressAnalysis::NewRegState(int stackSize)
{
  return new ARMRegState(ARM_NB_REGISTERS, stackSize);
}


bool ARMAddressAnalysis::importCallerArguments(AbstractRegMem & vAbstractRegMemCaller, AbstractRegMem & vAbstractRegMemCalled)
{
  //  return false;

  // R0-R3 may be used for argument in ARM. (aliases a1-a4)
  RegState *regsCalled, *regsCaller;
  bool modif = false;

  regsCaller = vAbstractRegMemCaller.getRegState();
  regsCalled = vAbstractRegMemCalled.getRegState();
  for (int i = 0; i < 4; i++)
    {
      if (regsCalled->importRegister(regsCaller, i))
	modif = true;
    }
  return modif;
}

void ARMAddressAnalysis::printPointerAccessInfos(Instruction* vinstr)
{ 
  printInstrMessage(" *** POINTER ACCESS in ARMAddressAnalysis::setPointerAccessInfo ", vinstr);
}


void ARMAddressAnalysis::printInstrMessage(string prefix, Instruction* vinstr)
{ 
  long vaddr;
  cout << prefix + vinstr->GetCode();
  if (InstructionARM::getCodeAddress(vinstr, &vaddr))
    cout << " at address = " << std::hex << vaddr << " " << std::dec;
  cout << endl;

}

//-----------Public -----------------------------------------------------------------

 ARMAddressAnalysis::ARMAddressAnalysis(Program * p, int sp):AddressAnalysis(p, sp)
{
}

bool ARMAddressAnalysis::PerformAnalysis()
{
  return AddressAnalysis::PerformAnalysis();
}

// Checks if all required attributes are in the CFG
// Returns true if successful, false otherwise
bool ARMAddressAnalysis::CheckInputAttributes()
{
  return AddressAnalysis::CheckInputAttributes();
}

//nothig to remove
void ARMAddressAnalysis::RemovePrivateAttributes()
{
  AddressAnalysis::RemovePrivateAttributes();
}
