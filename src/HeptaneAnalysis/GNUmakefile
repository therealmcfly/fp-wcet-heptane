

INCLS+=-Isrc -Isrc/Generic -Isrc/SharedAttributes -Isrc/Specific/CacheAnalysis -Isrc/Specific/CodeLine -Isrc/Specific/DataAddressAnalysis -Isrc/Specific/DotPrint
INCLS+=-Isrc/Specific/DummyAnalysis -Isrc/Specific/HtmlPrint -Isrc/Specific/IPETAnalysis -Isrc/Specific/PipelineAnalysis -Isrc/Specific/SimplePrint -Isrc/Specific/SESEAnalysis

CFGLIB_DIR_OBJ=../Common/cfglib/obj

OBJS= obj/Config.o obj/Analysis.o obj/AnalysisHelper.o obj/Timer.o obj/Context.o obj/ContextHelper.o \
obj/CodeLine.o obj/CodeLineAttribute.o  obj/HtmlPrint.o \
obj/SimplePrint.o obj/DotPrint.o obj/Cache.o obj/ICacheAnalysis.o obj/DCacheAnalysis.o obj/CacheStatistics.o obj/IPETAnalysis.o obj/Solver.o obj/RegState.o obj/MIPSRegState.o  obj/RISCVRegState.o \
obj/StackAnalysis.o obj/AddressAnalysis.o obj/MIPSAddressAnalysis.o obj/ARMAddressAnalysis.o obj/MSP430RegState.o obj/MSP430AddressAnalysis.o obj/RISCVAddressAnalysis.o \
obj/PipelineAnalysis.o obj/MIPSPipelineAnalysis.o obj/InstructionPipeline.o obj/ARMPipelineAnalysis.o obj/ARMRegState.o obj/MSP430PipelineAnalysis.o obj/RISCVPipelineAnalysis.o \
obj/StackInfoAttribute.o obj/DummyAnalysis.o \
obj/SESERegion.o obj/SESEAnalysis.o \
obj/main.o

vbin=../../bin/HeptaneAnalysis
all: $(vbin)

include ../makefile.common
include makefile.depends

# To build a new analysis
# - CFGLIBLIB (cfg management library)
# - Analysis.o (analysis template to be inherited from)
# - object file for your different analyses (here CacheAnalysis.o AddressAnalysis.o SimplePrint.o)
# - main.o (calls the different analyses)
$(vbin): $(OBJS) \
	$(CFGLIB) \
	$(ARCHDEP_DIR_OBJ)/MIPS.o\
	$(ARCHDEP_DIR_OBJ)/ARM.o\
	$(ARCHDEP_DIR_OBJ)/MSP430.o\
	$(ARCHDEP_DIR_OBJ)/RISCV.o\
	$(ARCHDEP_DIR_OBJ)/arch.o\
	$(ARCHDEP_DIR_OBJ)/InstructionType.o\
	$(ARCHDEP_DIR_OBJ)/InstructionFormat.o\
        $(ARCHDEP_DIR_OBJ)/DAAInstruction.o \
        $(ARCHDEP_DIR_OBJ)/DAAInstruction_MIPS.o \
        $(ARCHDEP_DIR_OBJ)/DAAInstruction_ARM.o \
        $(ARCHDEP_DIR_OBJ)/DAAInstruction_MSP430.o \
        $(ARCHDEP_DIR_OBJ)/DAAInstruction_RISCV.o \
	$(GLOB_ATTR_DIR_OBJ)/LoopTree.o\
	$(GLOB_ATTR_DIR_OBJ)/AddressAttribute.o\
	$(GLOB_ATTR_DIR_OBJ)/SymbolTableAttribute.o\
	$(GLOB_ATTR_DIR_OBJ)/ARMWordsAttribute.o \
	$(GLOB_ATTR_DIR_OBJ)/MetaInstructionAttribute.o \
	$(UTILITY_DIR_OBJ)/Logger.o $(UTILITY_DIR_OBJ)/Utl.o $(UTILITY_DIR_OBJ)/FileLoader.o $(UTILITY_DIR_OBJ)/InstructionARM.o $(UTILITY_DIR_OBJ)/UtlCfgLib.o  $(UTILITY_DIR_OBJ)/CallGraph.o\
	$(CFGLIB_DIR_OBJ)/Attributed.o $(CFGLIB_DIR_OBJ)/Factory.o \
	$(CFGLIB_DIR_OBJ)/Node.o $(CFGLIB_DIR_OBJ)/XmlExtra.o \
	$(CFGLIB_DIR_OBJ)/Cfg.o $(CFGLIB_DIR_OBJ)/Handle.o \
	$(CFGLIB_DIR_OBJ)/PointerAttributes.o $(CFGLIB_DIR_OBJ)/CloneHandle.o\
	$(CFGLIB_DIR_OBJ)/Instruction.o $(CFGLIB_DIR_OBJ)/Program.o \
	$(CFGLIB_DIR_OBJ)/Edge.o $(CFGLIB_DIR_OBJ)/Loop.o\
	$(CFGLIB_DIR_OBJ)/SerialisableAttributes.o

	$(CXX) $^ $(LINKSFLAGS) -o $@


clean:
	$(RM) $(vbin) $(OBJS) doc/generated-doc/html/*.html


force: clean all doc

