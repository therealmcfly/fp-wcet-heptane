/* ------------------------------------------------------------------------------------

	 Copyright IRISA, 2003-2017

	 This file is part of Heptane, a tool for Worst-Case Execution Time (WCET) estimation.
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

	 ------------------------------------------------------------------------------------ */

/*****************************************************************
														 FLEXPRETPipelineAnalysis
 This is the entry point of the pipeline analysis for FLEXPRET architecture.

*****************************************************************/

/* #include <ostream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <vector>
#include <set>
#include <map>

#include <errno.h>

#include "SharedAttributes/SharedAttributes.h"
*/

#include "MIPSPipelineAnalysis.h"

// using namespace std;

#ifndef FLEXPRETPIPELINEANALYSIS_H
#define FLEXPRETPIPELINEANALYSIS_H

class FLEXPRETPipelineAnalysis : public MIPSPipelineAnalysis
{

public:
	/** Constructor */
	FLEXPRETPipelineAnalysis(Program *p, int nbcache);

	/** Check input attributes and initialize CHMC attributes for scratchpad memory
			@return true if successful, false otherwise.
	*/
	bool CheckInputAttributes();

	/** Performs the analysis
			@return true if successful, false otherwise.
	*/
	bool PerformAnalysis();

	/** Override scheduleFirstInst for 5-stage pipeline */
	void scheduleFirstInst(Instruction &inst, vector<InstructionPipeline *> &IP, Context *context, bool first);

	/** Override scheduleNextInst for 5-stage pipeline with instruction-specific latencies */
	void scheduleNextInst(Instruction &inst, vector<InstructionPipeline *> &IP, Context *context, bool first);
};
#endif
