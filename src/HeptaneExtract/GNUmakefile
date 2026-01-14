
INCLS+=-Isrc 
OBJS=obj/ConfigExtract.o obj/dominatorData.o obj/dominatorAnalysis.o obj/loopAnalysis.o obj/Annotations.o  obj/switchAnalysis.o obj/HeptaneExtract.o
CFGLIB_DIR_OBJ=../Common/cfglib/obj
CFGLIB_DIR_OBJS=$(CFGLIB_DIR_OBJ)/Attributed.o $(CFGLIB_DIR_OBJ)/Factory.o \
	$(CFGLIB_DIR_OBJ)/Node.o $(CFGLIB_DIR_OBJ)/XmlExtra.o \
	$(CFGLIB_DIR_OBJ)/Cfg.o $(CFGLIB_DIR_OBJ)/Handle.o \
	$(CFGLIB_DIR_OBJ)/PointerAttributes.o $(CFGLIB_DIR_OBJ)/CloneHandle.o\
	$(CFGLIB_DIR_OBJ)/Instruction.o $(CFGLIB_DIR_OBJ)/Program.o \
	$(CFGLIB_DIR_OBJ)/Edge.o $(CFGLIB_DIR_OBJ)/Loop.o\
	$(CFGLIB_DIR_OBJ)/SerialisableAttributes.o

vbin=../../bin/HeptaneExtract
all: $(vbin)

include ../makefile.common
include makefile.depends


$(vbin): $(ARCHDEP_DIR_OBJ)/MIPS.o $(ARCHDEP_DIR_OBJ)/ARM.o $(ARCHDEP_DIR_OBJ)/MSP430.o  $(ARCHDEP_DIR_OBJ)/RISCV.o $(ARCHDEP_DIR_OBJ)/arch.o $(ARCHDEP_DIR_OBJ)/InstructionFormat.o $(ARCHDEP_DIR_OBJ)/InstructionType.o \
        $(ARCHDEP_DIR_OBJ)/DAAInstruction.o $(ARCHDEP_DIR_OBJ)/DAAInstruction_MIPS.o $(ARCHDEP_DIR_OBJ)/DAAInstruction_MSP430.o $(ARCHDEP_DIR_OBJ)/DAAInstruction_ARM.o $(ARCHDEP_DIR_OBJ)/DAAInstruction_RISCV.o \
	$(OBJS)\
	$(CFGLIB_DIR_OBJS)\
	$(UTILITY_DIR_OBJ)/Logger.o $(UTILITY_DIR_OBJ)/Utl.o $(UTILITY_DIR_OBJ)/FileLoader.o  $(UTILITY_DIR_OBJ)/UtlCfgLib.o $(UTILITY_DIR_OBJ)/CallGraph.o \
        $(GLOB_ATTR_DIR_OBJ)/AddressAttribute.o $(GLOB_ATTR_DIR_OBJ)/LoopTree.o  $(GLOB_ATTR_DIR_OBJ)/SymbolTableAttribute.o $(GLOB_ATTR_DIR_OBJ)/ARMWordsAttribute.o $(GLOB_ATTR_DIR_OBJ)/MetaInstructionAttribute.o
	$(CXX) $^ $(LINKSFLAGS) -o $@

clean:
	$(RM) $(vbin) $(OBJS) doc/generated-doc/html/*.html

force: clean all doc

