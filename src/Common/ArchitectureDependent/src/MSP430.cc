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
   
#include <climits>
#include <algorithm>
#include "MSP430.h"
#include "Logger.h"
#include "DAAInstruction_MSP430.h"
#include "Utl.h"
#include "FileLoader.h"

#define BTRACE 0

// No load/Store ???

#define GetLatencyDataValue(s)  F->GetLatencyDataValue(s)

MSP430::MSP430(const bool is_big_endian_p, const string &dataPath)
{
 
  FileLoader *F = new FileLoader();
  F->loadDataLatency("MSP430", dataPath);

  is_big_endian = is_big_endian_p;
  zero_register_num = 3;	

  // ---------------------------------------
  // markers for ReadELF and Objdump files
  // ---------------------------------------
  
  readelf_sections_markers.push_back("Section Headers:");
  readelf_sections_markers.push_back("Entetes de section:");

  readelf_flags_markers.push_back("Key to Flags:");
  readelf_flags_markers.push_back("Cle des fanions:");

  objdump_text_markers.push_back("Disassembly of section .text:");
  objdump_text_markers.push_back("Deassemblage de la section .text:");

  objdump_symboltable_markers.push_back("SYMBOL TABLE:");
  
  
  //------------------------------------------
  // sections to be extracted in ReadELF file
  //------------------------------------------
  
  sectionsToExtract.push_back(".text");
  sectionsToExtract.push_back(".bss");
  sectionsToExtract.push_back(".sbss");
  sectionsToExtract.push_back(".data");
  sectionsToExtract.push_back(".sdata");
  sectionsToExtract.push_back(".rodata");

  
  //------------------------------------------
  // formats declaration
  //------------------------------------------
    
  formats["empty"].insert(new Empty());

  formats["return"].insert(new Empty());
  formats["return"].insert(new Int());
  
  formats["jumpa"].insert(new rd("HI"));
  formats["jumpa"].insert(new Addr());
  
  formats["simpla"].insert(new rd("HI"));
  formats["simpla"].insert(new mem());
  formats["simpla"].insert(new Addr());
  
  formats["movia"].insert(new mem_rd()); // ex : mov 4(r1), r13  
  formats["movia"].insert(new mem_mem()); // ex : sub 2(r1), 4(R1)
  formats["movia"].insert(new mem_addr()); // ex : mov 8(r1), 0x0502
  formats["movia"].insert(new addr_mem()); // ex : mov &0x05cc, 0(r1)
  formats["movia"].insert(new addr_rd()); // ex : mov &0x05cc, r12
  formats["movia"].insert(new addr_addr()); // ex : mov &0x05cc, &0x06a8
  formats["movia"].insert(new rs_rd()); // ex : mov r12, r13  
  formats["movia"].insert(new rs_mem()); // ex : mov r13, 4(r1)
  formats["movia"].insert(new rs_addr()); // ex : mov r12, &0x05cc
  formats["movia"].insert(new int_rd()); // ex : mov #8, r13  
  formats["movia"].insert(new int_mem()); // ex : mov #0, 0(r1)
  formats["movia"].insert(new int_addr()); // ex : mov #8, &0x0502

  formats["rpta"].insert( new int_instr());  // ex : rpt 14 { rrax.w r12 ;
  //---------------------------------------------------------------------
  // mnemonics declaration 27 core instructions 24 emulated instructions
  //---------------------------------------------------------------------

  //------------------------------------------
  // Emulated
  //------------------------------------------

  mnemonicToInstructionTypes["adc"] = new Basic("alu", formats["simpla"], new MSP_ADD(), GetLatencyDataValue("adc"));
  mnemonicToInstructionTypes["adc.b"] = new Basic("alu", formats["simpla"], new MSP_ADD(), GetLatencyDataValue("adc.b"));
  mnemonicToInstructionTypes["br"] = new UnconditionalJump("alu", formats["jumpa"], new MSP_NOP(), GetLatencyDataValue("br"));
  mnemonicToInstructionTypes["clr"] = new Basic("alu", formats["simpla"], new MSP_KILLOP1(), GetLatencyDataValue("clr"));
  mnemonicToInstructionTypes["clr.b"] = new Basic("alu", formats["simpla"], new MSP_KILLOP1(), GetLatencyDataValue("clr.b"));
  mnemonicToInstructionTypes["clrc"] = new Basic("alu", formats["empty"], new MSP_KILLOP1(), GetLatencyDataValue("clrc"));
  mnemonicToInstructionTypes["clrn"] = new Basic("alu", formats["empty"], new MSP_KILLOP1(), GetLatencyDataValue("clrn"));
  mnemonicToInstructionTypes["clrz"] = new Basic("alu", formats["empty"], new MSP_KILLOP1(), GetLatencyDataValue("clrz"));
  mnemonicToInstructionTypes["dadc"] = new Basic("alu", formats["simpla"], new MSP_ADD(), GetLatencyDataValue("dadc"));
  mnemonicToInstructionTypes["dadc.b"] = new Basic("alu", formats["simpla"], new MSP_ADD(), GetLatencyDataValue("dadc.b"));
  mnemonicToInstructionTypes["dec"] = new Basic("alu", formats["simpla"], new MSP_SUB(), GetLatencyDataValue("dec"));
  mnemonicToInstructionTypes["dec.b"] = new Basic("alu", formats["simpla"], new MSP_SUB(), GetLatencyDataValue("dec.b"));
  mnemonicToInstructionTypes["decd"] = new Basic("alu", formats["simpla"], new MSP_SUB(), GetLatencyDataValue("decd"));
  mnemonicToInstructionTypes["decd.b"] = new Basic("alu", formats["simpla"], new MSP_SUB(), GetLatencyDataValue("decd.b"));
  mnemonicToInstructionTypes["dint"] = new Basic("alu", formats["empty"], new MSP_NOP(), GetLatencyDataValue("dint"));
  mnemonicToInstructionTypes["eint"] = new Basic("alu", formats["empty"], new MSP_NOP(), GetLatencyDataValue("eint"));
  mnemonicToInstructionTypes["inc"] = new Basic("alu", formats["simpla"], new MSP_ADD(), GetLatencyDataValue("inc"));
  mnemonicToInstructionTypes["inc.b"] = new Basic("alu", formats["simpla"], new MSP_ADD(), GetLatencyDataValue("inc.b"));
  mnemonicToInstructionTypes["incd"] = new Basic("alu", formats["simpla"], new MSP_ADD(), GetLatencyDataValue("incd"));
  mnemonicToInstructionTypes["incd.b"] = new Basic("alu", formats["simpla"], new MSP_ADD(), GetLatencyDataValue("incd.b"));
  mnemonicToInstructionTypes["inv"] = new Basic("alu", formats["simpla"], new MSP_NOP(), GetLatencyDataValue("inv"));
  mnemonicToInstructionTypes["inv.b"] = new Basic("alu", formats["simpla"], new MSP_ADD(), GetLatencyDataValue("inv.b"));
  mnemonicToInstructionTypes["nop"] = new Basic("alu", formats["empty"], new MSP_NOP(), GetLatencyDataValue("nop"));
  mnemonicToInstructionTypes["pop"] = new Basic("alu", formats["simpla"], new MSP_NOP(), GetLatencyDataValue("pop"));
  mnemonicToInstructionTypes["pop.b"] = new Basic("alu", formats["simpla"], new MSP_NOP(), GetLatencyDataValue("pop.b"));

   mnemonicToInstructionTypes["ret"] = new Return("alu", formats["return"], new MSP_NOP(), GetLatencyDataValue("ret"));

  mnemonicToInstructionTypes["rla"] = new Basic("alu", formats["simpla"], new MSP_SHIFT(), GetLatencyDataValue("rla"));
  mnemonicToInstructionTypes["rla.b"] = new Basic("alu", formats["simpla"], new MSP_SHIFT(), GetLatencyDataValue("rla.b"));
  mnemonicToInstructionTypes["rlc"] = new Basic("alu", formats["simpla"], new MSP_SHIFT(), GetLatencyDataValue("rlc"));
  mnemonicToInstructionTypes["rlc.b"] = new Basic("alu", formats["simpla"], new MSP_SHIFT(), GetLatencyDataValue("rlc.b"));
  mnemonicToInstructionTypes["sbc"] = new Basic("alu", formats["simpla"], new MSP_SUB(), GetLatencyDataValue("sbc"));
  mnemonicToInstructionTypes["sbc.b"] = new Basic("alu", formats["simpla"], new MSP_SUB(), GetLatencyDataValue("sbc.b"));
  mnemonicToInstructionTypes["setc"] = new Basic("alu", formats["empty"], new MSP_NOP(), GetLatencyDataValue("setc"));
  mnemonicToInstructionTypes["setn"] = new Basic("alu", formats["empty"], new MSP_NOP(), GetLatencyDataValue("setn"));
  mnemonicToInstructionTypes["setz"] = new Basic("alu", formats["empty"], new MSP_NOP(), GetLatencyDataValue("setz"));
  mnemonicToInstructionTypes["tst"] =  new Basic("alu", formats["simpla"], new MSP_NOP(), GetLatencyDataValue("tst"));
  mnemonicToInstructionTypes["tst.b"] =  new Basic("alu", formats["simpla"], new MSP_NOP(), GetLatencyDataValue("tst.b"));
    
  //------------------------------------------
  // Jump Condition
  //------------------------------------------
 
  mnemonicToInstructionTypes["jc"] = new ConditionalJump("alu", formats["jumpa"], new MSP_NOP(), GetLatencyDataValue("jc"));
  mnemonicToInstructionTypes["jhs"] = new ConditionalJump("alu", formats["jumpa"], new MSP_NOP(), GetLatencyDataValue("jhs")); 
  mnemonicToInstructionTypes["jeq"] = new ConditionalJump("alu", formats["jumpa"], new MSP_NOP(), GetLatencyDataValue("jeq"));
  mnemonicToInstructionTypes["jz"] = new ConditionalJump("alu", formats["jumpa"], new MSP_NOP(), GetLatencyDataValue("jz"));
  mnemonicToInstructionTypes["jge"] = new ConditionalJump("alu", formats["jumpa"], new MSP_NOP(), GetLatencyDataValue("jge"));
  mnemonicToInstructionTypes["jl"] = new ConditionalJump("alu", formats["jumpa"], new MSP_NOP(), GetLatencyDataValue("jl"));
  mnemonicToInstructionTypes["jmp"] = new UnconditionalJump("alu", formats["jumpa"], new MSP_NOP(), GetLatencyDataValue("jmp"));
  mnemonicToInstructionTypes["jn"] = new ConditionalJump("alu", formats["jumpa"], new MSP_NOP(), GetLatencyDataValue("jn"));
  mnemonicToInstructionTypes["jnc"] = new ConditionalJump("alu", formats["jumpa"], new MSP_NOP(), GetLatencyDataValue("jnc")); 
  mnemonicToInstructionTypes["jlo"] = new ConditionalJump("alu", formats["jumpa"], new MSP_NOP(), GetLatencyDataValue("jlo"));
  mnemonicToInstructionTypes["jne"] = new ConditionalJump("alu", formats["jumpa"], new MSP_NOP(), GetLatencyDataValue("jne"));
  mnemonicToInstructionTypes["jnz"] = new ConditionalJump("alu", formats["jumpa"], new MSP_NOP(), GetLatencyDataValue("jnz"));

    
  //------------------------------------------
  // Single Operand
  //------------------------------------------
  mnemonicToInstructionTypes["call"] = new Call("alu", formats["jumpa"], new MSP_CALL(), GetLatencyDataValue("call"));
  mnemonicToInstructionTypes["push"] = new Basic("alu", formats["simpla"], new MSP_NOP(), GetLatencyDataValue("push"));
  mnemonicToInstructionTypes["push.b"] = new Basic("alu", formats["simpla"], new MSP_NOP(), GetLatencyDataValue("push.b"));
  mnemonicToInstructionTypes["reti"] = new Return("alu", formats["empty"], new MSP_NOP(), GetLatencyDataValue("reti"));    
  mnemonicToInstructionTypes["rra"] = new Basic("alu", formats["simpla"], new MSP_SHIFT(), GetLatencyDataValue("rra"));
  mnemonicToInstructionTypes["rra.b"] = new Basic("alu", formats["simpla"], new MSP_SHIFT(), GetLatencyDataValue("rra.b"));
  mnemonicToInstructionTypes["rrc"] = new Basic("alu", formats["simpla"], new MSP_SHIFT(), GetLatencyDataValue("rrc"));
  mnemonicToInstructionTypes["rrc.b"] = new Basic("alu", formats["simpla"], new MSP_SHIFT(), GetLatencyDataValue("rrc.b"));
  mnemonicToInstructionTypes["swpb"] = new Basic("alu", formats["simpla"], new MSP_NOP(), GetLatencyDataValue("swpb"));
  mnemonicToInstructionTypes["sxt"] = new Basic("alu", formats["simpla"], new MSP_NOP(), GetLatencyDataValue("sxt"));

  //------------------------------------------
  // Double Operand
  //------------------------------------------
    mnemonicToInstructionTypes["add"] = new Basic("alu", formats["movia"], new MSP_ADD(), GetLatencyDataValue("add"));
  mnemonicToInstructionTypes["add.b"] = new Basic("alu", formats["movia"], new MSP_ADD(), GetLatencyDataValue("add.b"));
  mnemonicToInstructionTypes["addc"] = new Basic("alu", formats["movia"], new MSP_ADD(), GetLatencyDataValue("addc"));
  mnemonicToInstructionTypes["addc.b"] = new Basic("alu", formats["movia"], new MSP_ADD(), GetLatencyDataValue("addc.b"));
  mnemonicToInstructionTypes["and"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("and"));
  mnemonicToInstructionTypes["and.b"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("and.b"));
  mnemonicToInstructionTypes["bic"] = new Basic("alu", formats["movia"], new MSP_KILLOP1(), GetLatencyDataValue("bic"));
  mnemonicToInstructionTypes["bic.b"] = new Basic("alu", formats["movia"], new MSP_KILLOP1(), GetLatencyDataValue("bic.b"));
  mnemonicToInstructionTypes["bis"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("bis"));
  mnemonicToInstructionTypes["bis.b"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("bis.b"));
  mnemonicToInstructionTypes["bit"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("bit"));
  mnemonicToInstructionTypes["bit.b"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("bit.b"));
  mnemonicToInstructionTypes["cmp"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("cmp"));
  mnemonicToInstructionTypes["cmp.b"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("cmp.b"));
  mnemonicToInstructionTypes["dadd"] = new Basic("alu", formats["movia"], new MSP_ADD(), GetLatencyDataValue("dadd"));
  mnemonicToInstructionTypes["dadd.b"] = new Basic("alu", formats["movia"], new MSP_ADD(), GetLatencyDataValue("dadd.b"));
  mnemonicToInstructionTypes["mov"] = new Basic("alu", formats["movia"], new MSP_MOVE(), GetLatencyDataValue("mov"));
  mnemonicToInstructionTypes["mov.b"] = new Basic("alu", formats["movia"], new MSP_MOVE(), GetLatencyDataValue("mov.b"));
  mnemonicToInstructionTypes["sub"] = new Basic("alu", formats["movia"], new MSP_SUB(), GetLatencyDataValue("sub"));
  mnemonicToInstructionTypes["sub.b"] = new Basic("alu", formats["movia"], new MSP_SUB(), GetLatencyDataValue("sub.b"));
  mnemonicToInstructionTypes["subc"] = new Basic("alu", formats["movia"], new MSP_SUB(), GetLatencyDataValue("subc"));
  mnemonicToInstructionTypes["subc.b"] = new Basic("alu", formats["movia"], new MSP_SUB(), GetLatencyDataValue("subc.b"));
  mnemonicToInstructionTypes["xor"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("xor"));
  mnemonicToInstructionTypes["xor.b"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("xor.b"));


  // Address Instructions, Operate on 20-bit Registers Data
  mnemonicToInstructionTypes["adda"] = new Basic("alu", formats["movia"], new MSP_ADD(), GetLatencyDataValue("adda")); 
  mnemonicToInstructionTypes["suba"] = new Basic("alu", formats["movia"], new MSP_SUB(), GetLatencyDataValue("suba")); 
  mnemonicToInstructionTypes["mova"] = new Basic("alu", formats["movia"], new MSP_MOVE(), GetLatencyDataValue("mova")); 
  mnemonicToInstructionTypes["cmpa"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("cmpa")); 

  mnemonicToInstructionTypes["rrum"] = new Basic("alu", formats["movia"], new MSP_SHIFT(), GetLatencyDataValue("rrum"));

  // Added LBesnard for msp430fr5739
  mnemonicToInstructionTypes["pushm.a"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("pushm.a")); // PUSHM.A #2,R14 ; Save R14 and R13 (20-bit data)
  mnemonicToInstructionTypes["popm.a"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("popm.a"));  // POPM.A #2,R14 ; Restore R13 and R14 (20-bit data)
  mnemonicToInstructionTypes["pushm"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("pushm"));   // PUSHM #2,R14 ; Save R14 and R13 (16-bit data)
  mnemonicToInstructionTypes["popm"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("popm"));    // POPM #2,R14 ; Restore R13 and R14 (16-bit data)
  mnemonicToInstructionTypes["pushm.w"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("pushm.w")); // PUSHM.W <=> PUSHM
  mnemonicToInstructionTypes["popm.w"] = new Basic("alu", formats["movia"], new MSP_NOP(), GetLatencyDataValue("popm.w"));  // POPM.W <=> POPM

 // rpt N { rrax.w	r12 ; repeat N times rrax.w	r12  : currently no effect (NOP) 
  mnemonicToInstructionTypes["rpt"] = new Repeat("alu", formats["rpta"], new MSP_NOP(), GetLatencyDataValue("rpt")); 

  // ------------------------------------------
  // regs declaration
  // ------------------------------------------

  regs["r0"] = 0;
  regs["r1"] = 1;
  regs["r2"] = 2;
  regs["r3"] = 3;
  regs["r4"] = 4;
  regs["r5"] = 5;
  regs["r6"] = 6;
  regs["r7"] = 7;
  regs["r8"] = 8;
  regs["r9"] = 9;
  regs["r10"] = 10;
  regs["r11"] = 11;
  regs["r12"] = 12;
  regs["r13"] = 13;
  regs["r14"] = 14;
  regs["r15"] = 15;
}  
  
  //-----------------------------------------------------
  //
  //  MSP430 class functions
  //
  //-----------------------------------------------------

MSP430::~MSP430()
{
  // delete formats map
  for (map < string, set < InstructionFormat * > >::iterator it1 = formats.begin(); it1 != formats.end(); it1++)
    {
      set < InstructionFormat * >current = (*it1).second;
      for (set < InstructionFormat * >::iterator it2 = current.begin(); it2 != current.end(); it2++)
	{
	  delete(*it2);
	}
    }

  // delete mnemonicToInstructionTypes map
  for (map < string, InstructionType * >::iterator it = mnemonicToInstructionTypes.begin(); it != mnemonicToInstructionTypes.end(); it++)
    {
      delete((*it).second);
    }
}

bool MSP430::isFunction(const string & line)
{
  return (line[0] != ' ');
}

bool MSP430::isInstruction(const string & line)
{
  return (line[0] == ' ');
}

ObjdumpFunction MSP430::parseFunction(const string & line)
{
  ObjdumpFunction result;
  result.line = line;

  string clean_line = removeUselessCharacters(line);

  string s_addr;
  istringstream parse(clean_line);
  parse >> s_addr >> result.name;

  //Convert the string in hexa to an addr
  result.addr = strtoul(s_addr.c_str(), NULL, 16);
  assert(result.addr != 0 && result.addr != ULONG_MAX);

  return result;
}

string MSP430::rebuiltObjdumpInstruction(const string & vcode, t_address addrinstr)
{
  ostringstream oss;
  oss << "0x" << hex << addrinstr;
  string l = oss.str () + ":\t" + vcode;
  Utl::replaceAll(l, ' ' , '\t');
  return l;
}

ObjdumpInstruction MSP430::parseInstruction(const string & line)
{
  ObjdumpInstruction result;
  result.line = line;
  string clean_line = removeUselessCharacters(line);
  string operands;
  string s_addr;
  string binary_code;
  string itorepeat; // intruction to be repeated (rpt instruction).
  bool LOCTRACE=false;

  /*** Modify objdump to respect MSP430 rules ***/
    if (LOCTRACE) cout << " 0 >> TRACE LBESNARD clean_line = " << clean_line << endl ;
   std::size_t rptfound = clean_line.find("rpt");
   bool isRpt = rptfound <= 20;
   if (isRpt) {
     Utl::replaceAll(clean_line, '\t',' ');
     Utl::removeAndKill(clean_line, 0, ';');
     // instruction to be repeated from { to ;
     std::size_t ip = clean_line.find("{"); 
     Utl::replace(clean_line, '{', ','); // to have addr rpt N,INSTR
     bool prev=false;
     for (std::size_t i = ip+1; i < clean_line.length (); i++)
       if ( clean_line[i] == ' ') 
	{ if (! prev) { clean_line[i]='_'; prev = true;} }
       else
	 prev=false;
     if ( clean_line[ip-1] == ' ') clean_line.erase (ip-1,1);
     if (LOCTRACE) cout << " Instruction to be repeated: " << clean_line << endl;
   }
   else {
     Utl::removeAndKill(clean_line, 0, ' ');
     if (LOCTRACE) cout << " 1 >> TRACE LBESNARD clean_line = " << clean_line << endl;  
     //Remove tabulations after the ,
     int tabfound = 0;
     std::size_t tfound = clean_line.find(",");  
     if (tfound <= 30) tabfound = static_cast<int>(tfound); else tabfound = clean_line.length();
     Utl::removeAndKill(clean_line, tabfound, '\t');

     if (LOCTRACE) cout << " 2 >> TRACE LBESNARD clean_line = " << clean_line << endl ;
     //Modify address to respect Utl::isAddr
     std::size_t absfound = clean_line.find("&0x");   
     if (absfound <= 30 && absfound >= 6) { //WARNING AFFECT RESULT.ADDR IF <6
       int abfound = static_cast<int>(absfound);
       clean_line.erase(clean_line.begin() + abfound, clean_line.begin()+ abfound + 1);	
     }
     else {
       // Added October 2020 LBesnard.
       // Address expressed in decimal, such that "mova r1,&82691 ; 0x14303"  (0x14303 HEXA =  82691 DECIMAL)
       if (absfound == string::npos) {
	  absfound = clean_line.find("&");
          if (absfound != string::npos) {
	    if (clean_line[absfound-1] == ',') {
	      int nb = Utl::scanIntegerfrom(clean_line, absfound + 1);
	       if (nb>0) {
		 // cout << "  Address expressed in decimal BEFORE = " << clean_line << endl;
		 clean_line.erase (absfound , nb+2); 
		 // cout << "  Address expressed in decimal AFTER  = " << clean_line << endl;
	       }
	    }
	  }
       }
     }

      if (LOCTRACE) cout << " 3 >> TRACE LBESNARD clean_line = " << clean_line << endl ;      
     //Modify call to conform to rules  
     std::size_t calfound = clean_line.find("call");  
     if (calfound <= 30) {
       //  "call	 -15422	; 0xc3c2  cc2500_update_status "
       //  replaced by  "call 0xc3c2  cc2500_update_status"
       // -- added June 2020 lbesnard
       std::size_t iCallEnd = calfound + 4;
       int nb = Utl::scanIntegerfrom(clean_line, calfound + 4);
       if (nb>0) {
	 clean_line.erase (iCallEnd, nb); 
	 std::size_t ivirg = clean_line.find(";"); 
	 clean_line.insert(ivirg + 7," ");
	 clean_line.erase (ivirg,1); 
       }  
       // -- End added June 2020 lbesnard
       else {
	 Utl::removeAndKill(clean_line, 0, ';');
	 std::size_t regcafound = clean_line.find("r1");  
	 if (regcafound <= 12) {
	   int regcarfound = static_cast<int>(regcafound);
	   clean_line.erase(clean_line.begin()+regcarfound, clean_line.begin()+regcarfound+4);
	 }
	 else {
	   regcafound = clean_line.find("r");  
	   if (regcafound <= 12) {
	     int regcarfound = static_cast<int>(regcafound);
	     clean_line.erase(clean_line.begin()+regcarfound, clean_line.begin()+regcarfound+3);
	   }
	 }
	 if (clean_line.length() > 17) clean_line.insert(16," ");
       }
       if (LOCTRACE) cout << " 4.1 >> TRACE LBESNARD clean_line = " << clean_line << endl ;
     }
     else {
       // Modify jump to conform to rules
       std::size_t jumpfound = clean_line.find("$");    
       if (jumpfound <= 30) {
	 std::size_t afound = clean_line.find(";");
	 int asfound = static_cast<int>(afound);
	 int jumfound = static_cast<int>(jumpfound);
	 clean_line.erase(clean_line.begin()+jumfound, clean_line.begin()+asfound+4);  
	 clean_line.insert(jumfound+6," ");
	  if (LOCTRACE) cout << " 4.2 >> TRACE LBESNARD clean_line = " << clean_line << endl ;
       }
       else { 
	 bool bTodo= true;
	 std::size_t jumpfound = clean_line.find("br");  
	 if (jumpfound <= 30) {
	   // Modify Br to conform to rules 
	   std::size_t brfound = clean_line.find(";PC");   
	   if (brfound <= 30)
	     {
	       int brrfound = static_cast<int>(brfound);
	       clean_line.erase(clean_line.begin()+brrfound-8, clean_line.begin()+brrfound+7);    
	        if (LOCTRACE) cout << " 4.3 >> TRACE LBESNARD clean_line = " << clean_line << endl ;
	       bTodo= false;
	     }	
	 }
	 if (bTodo) {
	   //Remove last part of the instruction
	   std::size_t afound = clean_line.find(";");  
	   if (afound <= 30) {
	     int asfound = static_cast<int>(afound);
	     std::string::iterator itas = std::remove(clean_line.begin(), clean_line.begin()+asfound, 'Z');
	     clean_line.erase(itas, clean_line.end());
	   }
	 }
	  if (LOCTRACE) cout << " 4.4 >> TRACE LBESNARD clean_line = " << clean_line << endl ;
       }
     } 
   }
   if (LOCTRACE) cout << " 5 >> TRACE LBESNARD clean_line = " << clean_line << endl ;
   istringstream parse(clean_line);
   // parse >> s_addr >> binary_code >> result.mnemonic >> operands >> result.extra;
   parse >> s_addr >> result.mnemonic >> operands >> result.extra;

   result.addr = strtoul(s_addr.c_str(), NULL, 16);
   assert(result.addr != 0 && result.addr != ULONG_MAX);
   result.operands = splitOperands(operands);

   // RPT insruction : keeping the instruction to be repeated.
   // if (isRpt) result.operands.push_back(itorepeat); 
     
  /* check that the mnemonic is defined and the operand format is correct */
  if (! getInstructionTypeFromMnemonic(result.mnemonic)->checkFormat(result.operands)) {
    Logger::addFatal("Error: instruction asm \"" + result.mnemonic + "\" format not valid (" + clean_line + ")" );
  }
  
  //--
  if (operands != "")
    result.asm_code = result.mnemonic + " " + operands;
  else
    result.asm_code = result.mnemonic;
  return result;
}

t_address MSP430::getJumpDestination(const ObjdumpInstruction & instr)
{
  //Get the last operand
  string s_addr = instr.operands[instr.operands.size() - 1];

  //Convert the string in hexa to an addr
  t_address result = strtoul(s_addr.c_str(), NULL, 16);
  assert(result != 0 && result != ULONG_MAX);

  return result;
}

bool MSP430::isMemPattern(const string & operand)
{
  if (operand.size() == 0) return false;
  string op_local = operand;
  //Remove "("
  size_t pos = op_local.find("(");
  if (pos == EOS) return false;
  op_local[pos] = ' ';

  //Remove ")"
  pos = op_local.find(")");
  if (pos == EOS) return false;
  if (pos != op_local.size() - 1) return false;
  //if not the last char
  op_local[pos] = ' ';

  //get the offset and the base register
  string offset, reg_name;
  istringstream parse(op_local);
  parse >> offset >> reg_name;

  //Check if reg_name is defined
  if ( !isRegisterName(reg_name)) return false;
  //Check if offset is a positive or negative integer
  if (! Utl::isDecNumber(offset)) return false;
  return true;
}

vector < string > MSP430::extractInputRegistersFromMem(const string & operand)
{
  string op_local = operand;
  vector < string > result;

  //Removing ")"
  size_t pos = op_local.find("(");
  op_local[pos] = ' ';

  //Removing "("
  pos = op_local.find(")");
  op_local[pos] = ' ';

  string offset, reg_name("");
  istringstream parse(op_local);

  //Extracting the offset and the base register
  parse >> offset >> reg_name;
  assert(reg_name != "");
  result.push_back(reg_name);
  return result;
}

bool MSP430::isReturn(const ObjdumpInstruction & instr)
{
  return getInstructionTypeFromMnemonic(instr.mnemonic)->isReturn();
}

string MSP430::getCalleeName(const ObjdumpInstruction & instr)
{
  assert(getInstructionTypeFromMnemonic(instr.mnemonic)->isCall());

  //We assume that the extra field is in the format "FunctionName"
  assert(instr.extra != "");
  string result = instr.extra;

  return result;
}

bool MSP430::getRegisterAndIndexStack(const string &instructionAsm, string &reg, int *i)
{
  string val, sp;
  if (instructionAsm.find ("sp") == EOS) return false;
  // instruction references "sp"
  vector < string > v_instr = Arch::splitInstruction (instructionAsm);
  reg= v_instr[1]; 
  Utl::extractRegVal( v_instr[2], sp, val);
  *i = Utl::string2int(val);
 return true;
}

string MSP430::removeUselessCharacters(const string & line)
{
  char c;
  string result = line;
  for (size_t i = 0; i < line.length(); i++) {
    c = result[i];
    if (( c == ':') || ( c == '#') || ( c == '<') || (c == '>') || ( c == '=') )
      result[i] = ' ';
  }
  return result;
}

/***************************************************************

	    MODIFICATIONS FOR ARM

**************************************************************/

vector < string > MSP430::extractOutputRegistersFromMem(const string & operand)
{
  //There are no output registers in memory access with MSP430
  vector < string > result;
  assert(result.empty());
  return result;
}

bool MSP430::isLoadMultiple(const string & instr)
{
  //There are no multiple load instructions in MSP430
  assert(false);
  return false;
}

int MSP430::getNumberOfLoads(const string & instr)
{
  int result = 1;		//only 1 in MSP430
  assert(getInstructionTypeFromAsm(instr)->isLoad());
  return result;
}

bool MSP430::isStoreMultiple(const string & instr)
{
  //There are no multiple store instructions in MSP430
  assert(false);
  return false;
}

int MSP430::getNumberOfStores(const string & instr)
{
  int result = 1;		//only 1 in MSP430
  assert(getInstructionTypeFromAsm(instr)->isStore());
  return result;
}

void MSP430::parseSymbolTableLine(const string & line, ObjdumpSymbolTable & table)
{
    /* We assume that the input line is in the format : Value  Location  Type  Section  Size  Name
     for some lines, the field Type is empty */

  string value, location, type, section, size, name;
  istringstream parse(line);

  // Extracting each field of the line. Several formats:
  // 000082e8 l       .text     00000000 .divsi3_skip_div0_test
  // 000082e0 g     F .text     00000000 .hidden __aeabi_idiv
  // 00008028 g     F .text     0000003c foo

  // parse >> value >> location >> type >> section >> size >> name;
  parse >> value >> location >> type;
  if (type == ".text")
    {
      section = ".text";
      type = "UNDEF";
      parse >> size >> name;
    }
  else
    parse >> section >> size >> name;

  bool hidden = (name == ".hidden");
  if (hidden) parse >> name;

  // Function
  if (section == ".text" && (type == "F" || (type == "UNDEF" && location == "g") || (type == "UNDEF" && location == "l") || hidden ))
    {
      assert(name != "");
      // filtering some predefined names.
      bool bDontCare = (name == "__ctors_end") || (name == "__dtors_start")  ||  (name == "__ctors_start")  ||  (name =="__dtors_end");
      if (!bDontCare)
	{
	  if (BTRACE) 
	    cout << " value=" << value << ", location=" << location << ", type= " << type << ", section= " << section << ", size=" << size << ", name=" << name << endl;
	  
	  // Converting the string to Unsigned Long
	  t_address addr = strtoul(value.c_str(), NULL, 16);
	  assert((addr != 0) && (addr != ULONG_MAX));
	  // Adding the function in the map
	  // BUT WE MAY HAVE SAME VALUE FOR TWO FUNCTIONS..
	  if (BTRACE)
	    {
	      ListOfString lnames = table.functions[addr];
	      if (!lnames.empty())
		cout << ">>>>>> Collision for " << name << " and " << *(lnames.begin()) << endl;
	    }
	  table.functions[addr].push_front(name);
	  if (type == "F") table.userFunctions[addr].push_front(name);
	  // cout << " ********** FUNCTION = " << name << " hidden = " << (hidden ? "yes" : "no") << " addr = " << std::hex << addr  << std::dec  << endl;
	}
    }
  // Variable
  else if (type == "O")
    {
      assert(section != "");
      assert(size != "");
      assert(name != "");

      // Converting the string to Unsigned Long
      t_address address = strtoul(value.c_str(), NULL, 16);
      assert(address != 0 && address != ULONG_MAX);

      // Converting the string to Integer
      int size_var = (int)strtol(size.c_str(), NULL, 16);
      assert(size_var != 0 && size_var != INT_MAX);

      // Searching the section of the variable in the vector of ObjdumpSymbolTable
      vector < ObjdumpSection >::const_iterator it;

      for (it = table.sections.begin(); it != table.sections.end(); it++)
	{
	  if ((*it).name == section)
	    {
	      break;
	    }
	}

      // Checking if the section of the variable is a section we extracted
      assert(it != table.sections.end());

      // Checking if the address of the variable is really in the section
      unsigned long end_of_section = (*it).addr + (unsigned long)(*it).size;
      assert(address >= (*it).addr && address < end_of_section);

      ObjdumpVariable var;
      var.name = name;
      var.addr = address;
      var.size = size_var;	// in bytes
      var.section_name = section;

      // Adding the variable in the vector
      table.variables.push_back(var);
    }
}

ObjdumpWord MSP430::readWordInstruction(const ObjdumpInstruction & instr, ObjdumpSymbolTable & table)
{
  assert(false);
  ObjdumpWord result;
  return result;
}

bool MSP430::isPcInInputResources(const ObjdumpInstruction & instr)
{
  assert(false);
  return false;
}


bool MSP430::isPcInOutputResources(const ObjdumpInstruction & instr)
{
  assert(false);
  return false;
}


vector < ObjdumpWord > MSP430::getWordsFromInstr(const ObjdumpInstruction & instr1, const ObjdumpInstruction & instr2, vector < ObjdumpWord > words,
					       bool & is_instr2_consumed)
{
  assert(false);
  vector < ObjdumpWord > result;
  return result;
}

bool MSP430::isInputWrittenRegister(const string & operand)
{
  assert(false);
  return false;
}

string MSP430::extractRegisterFromInputWrittenOperand(const string & operand)
{
  assert(false);
  return "";
}

bool MSP430::isRegisterList(const string & operand)
{
  assert(false);
  return "";
}

vector < string > MSP430::extractRegistersFromRegisterList(const string & operand)
{
  assert(false);
  vector < string > result;
  return result;
}

int MSP430::getSizeRegisterList(const string & operand)
{
  assert(false);
  return -1;
}

bool MSP430::isShifterOperand(const string & operand)
{
  assert(false);
  return false;
}

string MSP430::extractRegisterFromShifterOperand(const string & operand)
{
  assert(false);
  return "";
}

uint32_t MSP430::getStackFrameReservedSize(const string &instr)
{
  assert(false);
  return 0;
}

bool MSP430::getLoadStoreARMInfos(bool strongContext, string & instr, string & codeinstr, string & oregister, AddressingMode * vaddrmode,
				offsetType * TypeOperand, string & operand1, string & operand2, string & operand3)
{
  assert(false);
  return false;
}

bool MSP430::isPcLoadInstruction(string & instr)
{
  // assert(false);
  return false;
}

bool MSP430::getInstr1ARMInfos(string & instr, string & codeinstr, string & oregister, offsetType * TypeOperand, string & operand1, string & operand2)
{
  assert(false);
  return false;
}

bool MSP430::getInstr2ARMInfos(string & instr, string & codeinstr, string & oregister, offsetType * TypeOperand, string & operand1, string & operand2, string & operand3)
{
  assert(false);
  return false;
}

bool MSP430::getMultipleLoadStoreARMInfos(string & instr, string & codeinstr, string & oregister, vector < string > &regList, bool *writeBack)
{
  assert(false);
  return false;
}

bool MSP430::isARMClassInstr(const string &codeop, const string &prefix)
{
  assert(false);
  return false;
}

bool MSP430::isConditionnedARMInstr(const string &codeop, const string &prefix)
{
  assert(false);
  return false;
}

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/


string MSP430::removeUselessCharactersMemPattern(const string & line)
{
  string result = line;
  size_t pos = result.find("[");
  if (pos != EOS)
    {
      result[pos] = ' ';
    }
  do
    {
      pos = result.find(",");
      if (pos != EOS)
	{
	  result[pos] = ' ';
	}
    }
  while (pos != EOS);
  pos = result.find("]");
  if (pos != EOS)
    {
      result[pos] = ' ';
    }

  return result;
}

vector < string > MSP430::splitOperands(const string & operands)
{
  vector < string > result;

  if (operands.length() == 0) return result;

  string tmp = operands;
  int nb_operand = 1;

  for (size_t i = 0; i < tmp.length(); i++)
    {
      if (tmp[i] == ',')
	{
	  tmp[i] = ' ';
	  nb_operand++;
	}
    }

  string val;
  istringstream parse(tmp);
  for (int i = 0; i < nb_operand; i++)
    {
      parse >> val;
      result.push_back(val);
    }

  return result;
}







 
