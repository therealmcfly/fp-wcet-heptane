/* ---------------------------------------------------------------------

   Copyright IRISA, 2003-2014

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
#include "RISCV.h"
#include "Logger.h"
#include "DAAInstruction_RISCV.h"
#include "Utl.h"
#include "FileLoader.h"

#define BTRACE 0

#define GetLatencyDataValue(s)  F->GetLatencyDataValue(s)


RISCV::RISCV(const bool is_big_endian_p, const string &dataPath)
{
   FileLoader *F = new FileLoader();
   F->loadDataLatency("RISCV", dataPath);

  is_big_endian = is_big_endian_p;
  zero_register_num = 0;

  //------------------------------------------
  // markers for ReadELF and Objdump files
  //---------------------------------------
  readelf_sections_markers.push_back("Section Headers:");
  readelf_sections_markers.push_back("En-têtes de section:");

  readelf_flags_markers.push_back("Key to Flags:");
  readelf_flags_markers.push_back("Clé des fanions:");

  objdump_text_markers.push_back("Disassembly of section .text:");
  objdump_text_markers.push_back("Déassemblage de la section .text:");

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
  formats["rd_rs_rs"].insert(new rd_rs_rs());
  formats["hex"].insert(new Hex());
  formats["addr"].insert(new Addr());
  formats["rd_rs_int"].insert(new rd_rs_int());
  formats["rd_rs_hex"].insert(new rd_rs_hex());
  formats["rd_hex"].insert(new rd_hex());
  formats["rd_hi"].insert(new rd("HI"));
  formats["rd_lo"].insert(new rd("LO"));
  formats["rd_rs"].insert(new rd_rs());
  formats["rs_rd"].insert(new rs_rd());
  formats["rs_rs_lo_hi"].insert(new rs_rs("LO, HI"));
  formats["rs_rs"].insert(new rs_rs(""));
  formats["rs_addr"].insert(new rs_addr());
  formats["rd_addr"].insert(new rd_addr());
  formats["rs_rs_addr"].insert(new rs_rs_addr());
  formats["empty"].insert(new Empty());
  formats["rs"].insert(new rs());
  formats["rs_mem"].insert(new rs_mem());
  formats["rd_mem"].insert(new rd_mem());
  formats["rs_zero_hex"].insert(new rs_zero_hex());
  formats["jalr"].insert(new rs());
  formats["jalr"].insert(new rd_rs());
  formats["div"].insert(new zero_rs_rs("LO, HI"));
  formats["div"].insert(new rs_rs("LO, HI"));
  formats["rd_int"].insert(new rd_int());
  //formats["li"].insert(new rd_int());
  //formats["li"].insert(new rd_hex());

  formats["rd_rs_rounding"].insert(new rd_rs_rounding("rne,rtz,rdn,rup,rmm,dyn"));

  //------------------------------------------
  // mnemonics declaration
  //------------------------------------------

    //pseudo instruction -> translation needed for the pipeline
    mnemonicToInstructionTypes["li"] = new Basic("alu", formats["rd_int"], new RISCV_LI(), GetLatencyDataValue("li"));     // Load immediate
    mnemonicToInstructionTypes["ret"] = new Return("alu", formats["empty"], new RISCV_NOP(), GetLatencyDataValue("ret"));
    mnemonicToInstructionTypes["nop"] = new Basic("alu", formats["empty"], new RISCV_NOP(), GetLatencyDataValue("nop"));
    mnemonicToInstructionTypes["j"] = new UnconditionalJump("alu", formats["addr"], new RISCV_NOP(), GetLatencyDataValue("j"));

    mnemonicToInstructionTypes["sext.w"] = new Basic("alu", formats["rd_rs"], new RISCV_SIGN_EXTENDED_WORD, GetLatencyDataValue("sext.w")); // Sign extend word  equivalent  to addiw rd, rs, 0 ?
    
    //DH voir comment ne pas en générer car après il y a un jalr
    mnemonicToInstructionTypes["auipc"] = new Basic("alu", formats["rd_hex"], NULL, GetLatencyDataValue("auipc"));

    
    // Arithmetic operation
    mnemonicToInstructionTypes["add"]  = new Basic("alu",formats["rd_rs_rs"], new RISCV_ADD(), GetLatencyDataValue("add"));
    mnemonicToInstructionTypes["addw"]  = new Basic("alu",formats["rd_rs_rs"],new RISCV_ADD(), GetLatencyDataValue("addw"));
    mnemonicToInstructionTypes["addi"] = new Basic("alu", formats["rd_rs_int"], new RISCV_ADDIU(), GetLatencyDataValue("addi"));
    mnemonicToInstructionTypes["addiw"] = new Basic("alu", formats["rd_rs_int"], new RISCV_ADDIU(), GetLatencyDataValue("addiw"));
    mnemonicToInstructionTypes["sub"]  = new Basic("alu",formats["rd_rs_rs"], new RISCV_SUBTRACT(), GetLatencyDataValue("sub"));
    mnemonicToInstructionTypes["subw"]  = new Basic("alu",formats["rd_rs_rs"], new RISCV_SUBTRACT(), GetLatencyDataValue("subw"));

    mnemonicToInstructionTypes["rem"]  = new Basic("alu",formats["rd_rs_rs"], new RISCV_REMAINDER(), GetLatencyDataValue("rem"));
    mnemonicToInstructionTypes["remu"]  = new Basic("alu",formats["rd_rs_rs"], new RISCV_REMAINDER(), GetLatencyDataValue("remu"));
    mnemonicToInstructionTypes["remw"]  = new Basic("alu",formats["rd_rs_rs"], new RISCV_REMAINDER(), GetLatencyDataValue("remw"));
    mnemonicToInstructionTypes["remuw"]  = new Basic("alu",formats["rd_rs_rs"], new RISCV_REMAINDER(), GetLatencyDataValue("remuw"));

    mnemonicToInstructionTypes["mul"]  = new Basic("alu",formats["rd_rs_rs"], new RISCV_MUL(), GetLatencyDataValue("mul"));
    mnemonicToInstructionTypes["mulw"]  = new Basic("alu",formats["rd_rs_rs"], new RISCV_MUL(), GetLatencyDataValue("mulw"));

    mnemonicToInstructionTypes["div"]  = new Basic("alu",formats["rd_rs_rs"], new RISCV_DIV(), GetLatencyDataValue("div"));
    mnemonicToInstructionTypes["divw"]  = new Basic("alu",formats["rd_rs_rs"], new RISCV_DIV(), GetLatencyDataValue("divw"));

    // Store
    mnemonicToInstructionTypes["sd"] = new Store(8, "alu", formats["rs_mem"], new RISCV_DSTORE(), GetLatencyDataValue("sd"));
    mnemonicToInstructionTypes["sw"] = new Store(4, "alu", formats["rs_mem"], new RISCV_DSTORE(), GetLatencyDataValue("sw"));
    mnemonicToInstructionTypes["sh"] = new Store(2, "alu", formats["rs_mem"], new RISCV_DSTORE(), GetLatencyDataValue("sh"));
    mnemonicToInstructionTypes["sb"] = new Store(1, "alu", formats["rs_mem"], new RISCV_DSTORE(), GetLatencyDataValue("sb"));
    mnemonicToInstructionTypes["fsw"] = new Store(4, "alu", formats["rs_mem"], new RISCV_DSTORE(), GetLatencyDataValue("fsw"));//TODO: check size
    mnemonicToInstructionTypes["fsd"] = new Store(8, "alu", formats["rs_mem"], new RISCV_DSTORE(), GetLatencyDataValue("fsd"));//TODO: check size

    
    // Load
    mnemonicToInstructionTypes["ld"] = new Load(8, "alu", formats["rd_mem"],  new RISCV_DLOAD(), GetLatencyDataValue("ld"));
    mnemonicToInstructionTypes["lw"] = new Load(4, "alu", formats["rd_mem"],  new RISCV_DLOAD(), GetLatencyDataValue("lw"));
    mnemonicToInstructionTypes["lwu"] = new Load(4, "alu", formats["rd_mem"], new RISCV_DLOAD(), GetLatencyDataValue("lwu"));
    mnemonicToInstructionTypes["lh"] = new Load(2, "alu", formats["rd_mem"],  new RISCV_DLOAD(), GetLatencyDataValue("lh"));
    mnemonicToInstructionTypes["lhu"] = new Load(2, "alu", formats["rd_mem"], new RISCV_DLOAD(), GetLatencyDataValue("lhu"));
    mnemonicToInstructionTypes["lbu"] = new Load(1, "alu", formats["rd_mem"], new RISCV_DLOAD(), GetLatencyDataValue("lbu"));
    mnemonicToInstructionTypes["fld"] = new Load(8, "alu", formats["rd_mem"], new RISCV_DLOAD(), GetLatencyDataValue("fld"));//TODO: check size (double precision)
    mnemonicToInstructionTypes["flw"] = new Load(4, "alu", formats["rd_mem"], new RISCV_DLOAD(), GetLatencyDataValue("flw"));//TODO: check size (double precision)

    
    // Branch
    mnemonicToInstructionTypes["jal"] = new Call("alu", formats["rd_addr"], new RISCV_DCALL(), GetLatencyDataValue("jal"));
    mnemonicToInstructionTypes["ble"] = new ConditionalJump("alu", formats["rs_rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("ble"));
    mnemonicToInstructionTypes["bge"] = new ConditionalJump("alu", formats["rs_rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("bge"));
    mnemonicToInstructionTypes["bgeu"] = new ConditionalJump("alu", formats["rs_rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("bgeu"));
    mnemonicToInstructionTypes["blt"] = new ConditionalJump("alu", formats["rs_rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("blt"));
    mnemonicToInstructionTypes["bltu"] = new ConditionalJump("alu", formats["rs_rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("bltu"));
    mnemonicToInstructionTypes["bltz"] = new ConditionalJump("alu", formats["rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("bltz"));
    mnemonicToInstructionTypes["bgtz"] = new ConditionalJump("alu", formats["rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("bgtz"));
    mnemonicToInstructionTypes["bgez"] = new ConditionalJump("alu", formats["rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("bgez"));
    mnemonicToInstructionTypes["bnez"] = new ConditionalJump("alu", formats["rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("bnez"));
    mnemonicToInstructionTypes["bne"] = new ConditionalJump("alu", formats["rs_rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("bne"));
    mnemonicToInstructionTypes["beqz"] = new ConditionalJump("alu", formats["rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("beqz"));
    mnemonicToInstructionTypes["blez"] = new ConditionalJump("alu", formats["rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("blez"));
    mnemonicToInstructionTypes["bltu"] = new ConditionalJump("alu", formats["rs_rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("bltu"));
    mnemonicToInstructionTypes["bleu"] = new ConditionalJump("alu", formats["rs_rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("bleu"));
    mnemonicToInstructionTypes["beq"] = new ConditionalJump("alu", formats["rs_rs_addr"], new RISCV_BRANCH(), GetLatencyDataValue("beq"));
    
    mnemonicToInstructionTypes["jr"] = new Return("alu", formats["rs"], new RISCV_BRANCH(), GetLatencyDataValue("jr"));//TODO check (cover with switch)

    // Load immediate
    mnemonicToInstructionTypes["lui"] = new Basic("alu", formats["rd_hex"], new RISCV_LUI(), GetLatencyDataValue("lui"));

        // Move
    mnemonicToInstructionTypes["mv"] = new Basic("alu", formats["rd_rs"], new RISCV_MOVE(), GetLatencyDataValue("mv"));

    // others (mostly logic)
    mnemonicToInstructionTypes["or"] = new Basic("alu", formats["rd_rs_rs"],  new RISCV_LOGICAL(), GetLatencyDataValue("or"));
    mnemonicToInstructionTypes["xor"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_LOGICAL(), GetLatencyDataValue("xor"));
    mnemonicToInstructionTypes["and"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_LOGICAL(), GetLatencyDataValue("and"));
    mnemonicToInstructionTypes["not"] = new Basic("alu", formats["rd_rs"],    new RISCV_LOGICAL(), GetLatencyDataValue("not"));

    mnemonicToInstructionTypes["xori"] = new Basic("alu", formats["rd_rs_int"], new RISCV_LOGICAL_I(), GetLatencyDataValue("xori"));
    mnemonicToInstructionTypes["ori"] = new Basic("alu", formats["rd_rs_int"],  new RISCV_LOGICAL_I(), GetLatencyDataValue("ori"));
    mnemonicToInstructionTypes["andi"] = new Basic("alu", formats["rd_rs_int"], new RISCV_LOGICAL_I(), GetLatencyDataValue("andi"));

    mnemonicToInstructionTypes["neg"] = new Basic("alu", formats["rd_rs"],  new RISCV_NEGATE(), GetLatencyDataValue("neg"));
    mnemonicToInstructionTypes["negw"] = new Basic("alu", formats["rd_rs"], new RISCV_NEGATE(), GetLatencyDataValue("negw"));

    mnemonicToInstructionTypes["sll"] = new Basic("alu", formats["rd_rs_rs"],    new RISCV_SHIFT(), GetLatencyDataValue("sll"));
    mnemonicToInstructionTypes["slli"] = new Basic("alu", formats["rd_rs_hex"],  new RISCV_SHIFT(), GetLatencyDataValue("slli"));
    mnemonicToInstructionTypes["slliw"] = new Basic("alu", formats["rd_rs_hex"], new RISCV_SHIFT(), GetLatencyDataValue("slliw"));
    //mnemonicToInstructionTypes["sll"] = new Basic("alu", formats["rd_rs_rs"],  new RISCV_SHIFT(), GetLatencyDataValue("sll"));
    mnemonicToInstructionTypes["sllw"] = new Basic("alu", formats["rd_rs_rs"],   new RISCV_SHIFT(), GetLatencyDataValue("sllw"));

    mnemonicToInstructionTypes["srai"] = new Basic("alu", formats["rd_rs_hex"],  new RISCV_SHIFT(), GetLatencyDataValue("srai"));
    mnemonicToInstructionTypes["sraiw"] = new Basic("alu", formats["rd_rs_hex"], new RISCV_SHIFT(), GetLatencyDataValue("sraiw"));
    mnemonicToInstructionTypes["srli"] = new Basic("alu", formats["rd_rs_hex"],  new RISCV_SHIFT(), GetLatencyDataValue("srli"));
    mnemonicToInstructionTypes["srliw"] = new Basic("alu", formats["rd_rs_hex"], new RISCV_SHIFT(), GetLatencyDataValue("srliw"));
    mnemonicToInstructionTypes["sraw"] = new Basic("alu", formats["rd_rs_rs"],   new RISCV_SHIFT(), GetLatencyDataValue("sraw"));
    mnemonicToInstructionTypes["sra"] = new Basic("alu", formats["rd_rs_rs"],    new RISCV_SHIFT(), GetLatencyDataValue("sra"));

    mnemonicToInstructionTypes["sltu"] = new Basic("alu", formats["rd_rs_rs"],   new RISCV_SETIF(), GetLatencyDataValue("sltu")); // set to 1 if less than value
    mnemonicToInstructionTypes["sltiu"] = new Basic("alu", formats["rd_rs_int"], new RISCV_SETIF(), GetLatencyDataValue("sltiu"));
    mnemonicToInstructionTypes["seqz"] = new Basic("alu", formats["rd_rs"],      new RISCV_SETIF(), GetLatencyDataValue("seqz")); // set to 1 if eq 0
    
    // FLOATING POINT INSTRUCTIONS
    // Conversions
    mnemonicToInstructionTypes["fcvt.d.l"] = new Basic("alu", formats["rd_rs"],  new RISCV_CONVERSION(), GetLatencyDataValue("fcvt.d.l"));
    mnemonicToInstructionTypes["fcvt.d.lu"] = new Basic("alu", formats["rd_rs"], new RISCV_CONVERSION(), GetLatencyDataValue("fcvt.d.lu"));
    mnemonicToInstructionTypes["fcvt.d.w"] = new Basic("alu", formats["rd_rs"],  new RISCV_CONVERSION(), GetLatencyDataValue("fcvt.d.w"));
    mnemonicToInstructionTypes["fcvt.d.s"] = new Basic("alu", formats["rd_rs"],  new RISCV_CONVERSION(), GetLatencyDataValue("fcvt.d.s"));
    mnemonicToInstructionTypes["fcvt.s.w"] = new Basic("alu", formats["rd_rs"],  new RISCV_CONVERSION(), GetLatencyDataValue("fcvt.s.w"));

    mnemonicToInstructionTypes["fcvt.w.d"] = new Basic("alu", formats["rd_rs_rounding"], new RISCV_CONVERSION(), GetLatencyDataValue("fcvt.w.d"));
    mnemonicToInstructionTypes["fcvt.l.d"] = new Basic("alu", formats["rd_rs_rounding"], new RISCV_CONVERSION(), GetLatencyDataValue("fcvt.l.d"));
    mnemonicToInstructionTypes["fcvt.s.d"] = new Basic("alu", formats["rd_rs_rounding"], new RISCV_CONVERSION(), GetLatencyDataValue("fcvt.s.d"));
    mnemonicToInstructionTypes["fcvt.s.d"] = new Basic("alu", formats["rd_rs"], new RISCV_CONVERSION(), GetLatencyDataValue("fcvt.s.d"));

    // move
    mnemonicToInstructionTypes["fmv.d.x"] = new Basic("alu", formats["rd_rs"], new RISCV_MOVE(), GetLatencyDataValue("fmv.d.x"));
    mnemonicToInstructionTypes["fmv.x.d"] = new Basic("alu", formats["rd_rs"], new RISCV_MOVE(), GetLatencyDataValue("fmv.x.d"));
    mnemonicToInstructionTypes["fmv.d"] = new Basic("alu", formats["rd_rs"],   new RISCV_MOVE(), GetLatencyDataValue("fmv.d"));
    mnemonicToInstructionTypes["fmv.s"] = new Basic("alu", formats["rd_rs"],   new RISCV_MOVE(), GetLatencyDataValue("fmv.s"));
    mnemonicToInstructionTypes["fmv.w.x"] = new Basic("alu", formats["rd_rs"], new RISCV_MOVE(), GetLatencyDataValue("fmv.w.x"));

    // Division
    mnemonicToInstructionTypes["fdiv.d"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_DIV(), GetLatencyDataValue("fdiv.d"));
    mnemonicToInstructionTypes["fdiv.s"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_DIV(), GetLatencyDataValue("fdiv.s"));

    // multiplication
    mnemonicToInstructionTypes["fmul.d"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_MUL(), GetLatencyDataValue("fmul.d"));
    mnemonicToInstructionTypes["fmul.s"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_MUL(), GetLatencyDataValue("fmul.s"));

    // Addition
    mnemonicToInstructionTypes["fadd.d"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_ADD(), GetLatencyDataValue("fadd.d"));
    mnemonicToInstructionTypes["fadd.s"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_ADD(), GetLatencyDataValue("fadd.s"));

    // Substraction
    mnemonicToInstructionTypes["fsub.d"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_SUBTRACT(), GetLatencyDataValue("fsub.d"));
    mnemonicToInstructionTypes["fsub.s"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_SUBTRACT(), GetLatencyDataValue("fsub.s"));
    
    // negate (Two's complement)
    mnemonicToInstructionTypes["fneg.d"] = new Basic("alu", formats["rd_rs"], new RISCV_NEGATE(), GetLatencyDataValue("fneg.d"));
    mnemonicToInstructionTypes["fneg.s"] = new Basic("alu", formats["rd_rs"], new RISCV_NEGATE(), GetLatencyDataValue("fneg.s"));

    // fp comparing.
    mnemonicToInstructionTypes["feq.s"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_FP_COMPARE(), GetLatencyDataValue("feq.s"));
    mnemonicToInstructionTypes["feq.d"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_FP_COMPARE(), GetLatencyDataValue("feq.d"));//check format (fft)
    mnemonicToInstructionTypes["fle.s"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_FP_COMPARE(), GetLatencyDataValue("fle.s"));
    mnemonicToInstructionTypes["fle.d"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_FP_COMPARE(), GetLatencyDataValue("fle.d"));//check format (fft)
    mnemonicToInstructionTypes["flt.d"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_FP_COMPARE(), GetLatencyDataValue("flt.d"));
    mnemonicToInstructionTypes["flt.s"] = new Basic("alu", formats["rd_rs_rs"], new RISCV_FP_COMPARE(), GetLatencyDataValue("flt.s"));

  //------------------------------------------
  // regs declaration
  //------------------------------------------
  //GPR
    
    regs["zero"] = 0;
    regs["ra"] = 1;
    regs["sp"] = 2;
    regs["gp"] = 3;
    regs["tp"] = 4;
    regs["t0"] = 5;
    regs["t1"] = 6;
    regs["t2"] = 7;
    regs["fp"] = 8;
    regs["s0"] = 8;
    regs["s1"] = 9;
    regs["a0"] = 10;
    regs["a1"] = 11;
    regs["a2"] = 12;
    regs["a3"] = 13;
    regs["a4"] = 14;
    regs["a5"] = 15;
    regs["a6"] = 16;
    regs["a7"] = 17;
    regs["s2"] = 18;
    regs["s3"] = 19;
    regs["s4"] = 20;
    regs["s5"] = 21;
    regs["s6"] = 22;
    regs["s7"] = 23;
    regs["s8"] = 24;
    regs["s9"] = 25;
    regs["s10"] = 26;
    regs["s11"] = 27;
    regs["t3"] = 28;
    regs["t4"] = 29;
    regs["t5"] = 30;
    regs["t6"] = 31;
    
    //FP
    regs["ft0"] = 32;
    regs["ft1"] = 33;
    regs["ft2"] = 34;
    regs["ft3"] = 35;
    regs["ft4"] = 36;
    regs["ft5"] = 37;
    regs["ft6"] = 38;
    regs["ft7"] = 39;
    regs["fs0"] = 40;
    regs["fs1"] = 41;
    regs["fa0"] = 42;
    regs["fa1"] = 43;
    regs["fa2"] = 44;
    regs["fa3"] = 45;
    regs["fa4"] = 46;
    regs["fa5"] = 47;
    regs["fa6"] = 48;
    regs["fa7"] = 49;
    regs["fs2"] = 50;
    regs["fs3"] = 51;
    regs["fs4"] = 52;
    regs["fs5"] = 53;
    regs["fs6"] = 54;
    regs["fs7"] = 55;
    regs["fs8"] = 56;
    regs["fs9"] = 57;
    regs["fs10"] = 58;
    regs["fs11"] = 59;
    regs["ft8"] = 60;
    regs["ft9"] = 61;
    regs["ft10"] = 62;
    regs["ft11"] = 63;

/** TODO: check if it exist for RISCV

  //FP Code Condition
 */
}

//-----------------------------------------------------
//
//  RISCV class functions
//
//-----------------------------------------------------

/** aaaaaaaaaaaaaaaaaaaa   */
RISCV::~RISCV()
{
  //delete formats map
  for (map < string, set < InstructionFormat * > >::iterator it1 = formats.begin(); it1 != formats.end(); it1++)
    {
      set < InstructionFormat * >current = (*it1).second;
      for (set < InstructionFormat * >::iterator it2 = current.begin(); it2 != current.end(); it2++)
	{
	  delete(*it2);
	}
    }

  //delete mnemonicToInstructionTypes map
  for (map < string, InstructionType * >::iterator it = mnemonicToInstructionTypes.begin(); it != mnemonicToInstructionTypes.end(); it++)
    {
      delete((*it).second);
    }
}

string RISCV::removeUselessCharacters(const string & line)
{
  string result = line;
  for (size_t i = 0; i < line.length(); i++)	//remove useless char
    {
      if (result[i] == ':')
	{
	  result[i] = ' ';
	}
      else if (result[i] == '<')
	{
	  result[i] = ' ';
	}
      else if (result[i] == '>')
	{
	  result[i] = ' ';
	}
    }
  return result;
}

vector < string > RISCV::splitOperands(const string & operands)
{
  vector < string > result;

  if (operands.length() == 0)
    {
      return result;
    }

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

bool RISCV::isFunction(const string & line)
{
  return (line[0] != ' ');
}

bool RISCV::isInstruction(const string & line)
{
  return (line[0] == ' ');
}

ObjdumpFunction RISCV::parseFunction(const string & line)
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

t_address RISCV::getJumpDestination(const ObjdumpInstruction & instr)
{
  //Get the last operand
  string s_addr = instr.operands[instr.operands.size() - 1];

  //Convert the string in hexa to an addr
  t_address result = strtoul(s_addr.c_str(), NULL, 16);
  assert(result != 0 && result != ULONG_MAX);

  return result;
}

bool RISCV::isMemPattern(const string & operand)
{
  if (operand.size() == 0)
    {
      return false;
    }

  string op_local = operand;

  //Remove "("
  size_t pos = op_local.find("(");
  if (pos == EOS)
    {
      return false;
    }
  op_local[pos] = ' ';

  //Remove ")"
  pos = op_local.find(")");
  if (pos == EOS)
    {
      return false;
    }
  if (pos != op_local.size() - 1)
    {
      return false;
    }				//if not the last char
  op_local[pos] = ' ';

  //get the offset and the base register
  string offset, reg_name;
  istringstream parse(op_local);
  parse >> offset >> reg_name;

  //Check if reg_name is defined
  if (isRegisterName(reg_name) == false)
    {
      return false;
    }

  //Check if offset is a positive or negative integer
  if (Utl::isDecNumber(offset) == false)
    {
      return false;
    }

  return true;
}

/***************************************************************

	    MODIFICATIONS FOR ARM

**************************************************************/

vector < string > RISCV::extractInputRegistersFromMem(const string & operand)
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

vector < string > RISCV::extractOutputRegistersFromMem(const string & operand)
{
  //There are no output registers in memory access with RISCV
  vector < string > result;
  assert(result.empty());
  return result;
}

bool RISCV::isLoadMultiple(const string & instr)
{
  //There are no multiple load instructions in RISCV
  assert(false);
  return false;
}

int RISCV::getNumberOfLoads(const string & instr)
{
  int result = 1;		//only 1 in RISCV
  assert(getInstructionTypeFromAsm(instr)->isLoad());
  return result;
}

bool RISCV::isStoreMultiple(const string & instr)
{
  //There are no multiple store instructions in RISCV
  assert(false);
  return false;
}

int RISCV::getNumberOfStores(const string & instr)
{
  int result = 1;		//only 1 in RISCV
  assert(getInstructionTypeFromAsm(instr)->isStore());
  return result;
}

bool RISCV::isReturn(const ObjdumpInstruction & instr)
{
  return getInstructionTypeFromMnemonic(instr.mnemonic)->isReturn();
}

ObjdumpInstruction RISCV::parseInstruction(const string & line)
{
  ObjdumpInstruction result;
  result.line = line;

  string clean_line = removeUselessCharacters(line);

  string operands;
  string s_addr;
  string binary_code;

  istringstream parse(clean_line);
  parse >> s_addr >> binary_code >> result.mnemonic >> operands >> result.extra;

  result.addr = strtoul(s_addr.c_str(), NULL, 16);
  assert(result.addr != 0 && result.addr != ULONG_MAX);

  result.operands = splitOperands(operands);

  /* check that the mnemonic is defined and the operand format is correct */
  if (!getInstructionTypeFromMnemonic(result.mnemonic)->checkFormat(result.operands))
    {
      Logger::addFatal("Error: instruction asm \"" + result.mnemonic + "\" format not valid");
    }
  //--

  if (operands != "")
    {
      result.asm_code = result.mnemonic + " " + operands;
    }
  else
    {
      result.asm_code = result.mnemonic;
    }

  return result;
}

string RISCV::rebuiltObjdumpInstruction(const string & vcode, t_address addrinstr)
{
  ostringstream oss;

  oss << "0x" << hex << addrinstr;
  return oss.str () + " " + oss.str () + " " + vcode;
}

string RISCV::getCalleeName(const ObjdumpInstruction & instr)
{
  assert(getInstructionTypeFromMnemonic(instr.mnemonic)->isCall());

  //We assume that the extra field is in the format "FunctionName"
  assert(instr.extra != "");
  string result = instr.extra;

  return result;
}



bool RISCV::getSection(ObjdumpSymbolTable & table, string &section,   vector < ObjdumpSection >::const_iterator &it)
{
  for (it = table.sections.begin(); it != table.sections.end(); it++)
    if ((*it).name == section) return true;
  return false;
}



void RISCV::parseSymbolTableLine(const string & line, ObjdumpSymbolTable & table)
{
  /* We assume that the input line is in the format : Value  Location  Type  Section  Size  Name
     for some lines, the field Type is empty */

  //Extracting each field of the line
  string value, location, type, section, size, name;
  istringstream parse(line);
  parse >> value >> location >> type >> section >> size >> name;

  //Function
  if (type == "F" && section == ".text")
    {
      assert(name != "");

      //Converting the string to Unsigned Long
      t_address addr = strtoul(value.c_str(), NULL, 16);
      assert(addr != 0 && addr != ULONG_MAX);

      //Adding the function in the map
      table.functions[addr].push_front(name);
      table.userFunctions[addr].push_front(name);
    }
  else
    if (section == ".rodata") // constant
      {
	//Convert the string to Unsigned Long
	t_address address = strtoul(value.c_str(), NULL, 16);
	assert(address != 0 && address != ULONG_MAX);
	vector < ObjdumpSection >::const_iterator it;	
	assert(getSection(table, section, it));
      }
  //Variable
    else 
    if (type == "O")
      {
	assert(section != "");
	assert(size != "");
	assert(name != "");
	
	//Convert the string to Unsigned Long
	t_address address = strtoul(value.c_str(), NULL, 16);
	assert(address != 0 && address != ULONG_MAX);
	
      //Convert the string to Integer
	int size_var = (int)strtol(size.c_str(), NULL, 16);
	assert(size_var != 0 && size_var != INT_MAX);
	
	//Search for the section of the variable in the vector of ObjdumpSymbolTable
	//to check if the section of the variable is a section we extracted
	vector < ObjdumpSection >::const_iterator it;
	assert(getSection(table, section, it));
	
	//Checking if the address of the variable is really in the section
	unsigned long end_of_section = (*it).addr + (unsigned long)(*it).size;
	assert(address >= (*it).addr && address < end_of_section);
	
	//create the ObjdumpVariable
	ObjdumpVariable var;
	var.name = name;
	var.addr = address;
	var.size = size_var;	//in bytes
	var.section_name = section;
	
	//Add the variable in the vector
	table.variables.push_back(var);
      }
  //global pointer
  //In this case, Size is the Name because the field Type is empty so there is a shift in the parsing
  //like this  : 0000000000011c1c g       .text	0000000000000000 __global_pointer$
    else if (size == "__global_pointer$")
      {
	//Convert the string to unsigned Long
	t_address address = strtoul(value.c_str(), NULL, 16);
	assert(address != 0 && address != ULONG_MAX);
	table.gp = address;
    }
}

ObjdumpWord RISCV::readWordInstruction(const ObjdumpInstruction & instr, ObjdumpSymbolTable & table)
{
  assert(false);
  ObjdumpWord result;
  return result;
}

bool RISCV::isPcInInputResources(const ObjdumpInstruction & instr)
{
  assert(false);
  return false;
}


bool RISCV::isPcInOutputResources(const ObjdumpInstruction & instr)
{
  assert(false);
  return false;
}


vector < ObjdumpWord > RISCV::getWordsFromInstr(const ObjdumpInstruction & instr1, const ObjdumpInstruction & instr2, vector < ObjdumpWord > words,
					       bool & is_instr2_consumed)
{
  assert(false);
  vector < ObjdumpWord > result;
  return result;
}

bool RISCV::isInputWrittenRegister(const string & operand)
{
  assert(false);
  return false;
}

string RISCV::extractRegisterFromInputWrittenOperand(const string & operand)
{
  assert(false);
  return "";
}

bool RISCV::isRegisterList(const string & operand)
{
  assert(false);
  return "";
}

vector < string > RISCV::extractRegistersFromRegisterList(const string & operand)
{
  assert(false);
  vector < string > result;
  return result;
}

int RISCV::getSizeRegisterList(const string & operand)
{
  assert(false);
  return -1;
}

bool RISCV::isShifterOperand(const string & operand)
{
  assert(false);
  return false;
}

string RISCV::extractRegisterFromShifterOperand(const string & operand)
{
  assert(false);
  return "";
}

bool RISCV::getLoadStoreARMInfos(bool strongContext, string & instr, string & codeinstr, string & oregister, AddressingMode * vaddrmode,
				offsetType * TypeOperand, string & operand1, string & operand2, string & operand3)
{
  assert(false);
  return false;
}

bool RISCV::isPcLoadInstruction(string & instr)
{
  // assert(false);
  return false;
}

bool RISCV::getInstr1ARMInfos(string & instr, string & codeinstr, string & oregister, offsetType * TypeOperand, string & operand1, string & operand2)
{
  assert(false);
  return false;
}

bool RISCV::getInstr2ARMInfos(string & instr, string & codeinstr, string & oregister, offsetType * TypeOperand, string & operand1, string & operand2, string & operand3)
{
  assert(false);
  return false;
}

bool RISCV::getMultipleLoadStoreARMInfos(string & instr, string & codeinstr, string & oregister, vector < string > &regList, bool *writeBack)
{
  assert(false);
  return false;
}

bool RISCV::isARMClassInstr(const string &codeop, const string &prefix)
{
  assert(false);
  return false;
}

bool RISCV::isConditionnedARMInstr(const string &codeop, const string &prefix)
{
  assert(false);
  return false;
}


bool RISCV::getRegisterAndIndexStack(const string &instructionAsm, string &reg, int *i)
{
  string val, sp;

  if (instructionAsm.find ("sp") == EOS) return false;
  vector < string > v_instr = Arch::splitInstruction (instructionAsm);
  reg = v_instr[1]; 
  Utl::extractRegVal( v_instr[2], sp, val);
  // instruction references "sp" such that sw ra,20(sp)
  *i = Utl::string2int(val);
  return true;
}
