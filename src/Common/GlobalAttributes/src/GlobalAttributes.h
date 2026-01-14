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

/*

  This file contains type definitions and naming conventions
  for all attributes shared by the different analysis binaries
  (CFG extractor and WCETanalysis).

  Other attributes (used locally in the CFG extractor and
  WCET analyzer) should NOT use the same attribute names.

  This file is included in both the CFG extractor and the
  WCET analyzer.

*/

#ifndef GLOBAL_ATTRIBUTES_H
#define GLOBAL_ATTRIBUTES_H

//////////////////////////////////////////////////////////////

/*! Loop bound attribute
 * ---------------------
 *
 * Maximum number of iterations for loops
 * This attribute is set by the CFG extractor
 * (annotations embedded in source code or external xml file)
 * and is retrieved by any subsequent analysis step
*/

// Attribute name
#define MaxiterAttributeName "maxiter"

/** WCET attribute
 * ---------------------
 *
 * WCET of entry point
 */
#define WCETAttributeName "WCET"
// External WCET is given as input by the user
#define ExternalWCETAttributeName "ExWCET"
#define ExternalWCETAttributeNameType SerialisableIntegerAttribute 
#define WCET_type long

/*
 * Address Attributes
 * ------------------
 * Set of addresses acceded by each instruction
 * Attached by the CFG extractor, used by the
 * WCET analyzer
 */

#define AddressAttributeName "address"
#include "AddressAttribute.h"


/*
 * SymbolTable Attribute
 * ------------------
 * Symbol Table information
 * Attached by the CFG extractor, used by the
 * WCET analyzer
 */
#define SymbolTableAttributeName "symbolTable"
#include "SymbolTableAttribute.h"


/*
 * ARMWords Attribute
 * ------------------
 * ARM .word information
 * Attached by the CFG extractor, used by the
 * WCET analyzer
 */
#define ARMWordsAttributeName "ARM_WORDS"
#include "ARMWordsAttribute.h"

/** Attached to ARM to added instruction for the translation of pusp, pop, ldm, stm instructions.
    Attached by the CFG extractor */
#define MetaInstructionAttributeName "MetaInstruction"
#include "MetaInstructionAttribute.h"

/*
 * Instruction Attribute
 */
#define InstructionPredicateAttributeName "predicate"
#define InstructionIsConditionalAttributeName "conditional"
#define InstructionTypeAttributeName "type"
#define InstructionTypeLoad "load"
#define InstructionTypeStore "store"
#define InstructionTypeFwJump "fwjump"
#define InstructionTypeBwJump "bwjump"
#define InstructionTypeOther "other"

/*
 * Determine the shape of the loop
 *    pre: the conditional branch is a the beginning of the loop
 *    post: the conditional branch is a the end of the loop
 * 
 * Attribute added in loopAnalysis.cc when extracting/creating the loop
 * Attribute used at least with the branch predictor analysis
 */
#define LoopShapeAttributeName "LoopShape"
#define LoopScopeNBDescendantBranchesInstr "LoopDescendantBranches"
#define BRPRED_FREQ_LOOPENTER "BrPred_Freq_FirstIter"
#define BRPRED_FIRSTITER_MISS_BTB "BrPred_FirstIter_MissBTB"

#define JumpTargetAddressAttributeName "jump_target"

#endif // GLOBAL_ATTRIBUTES_H
