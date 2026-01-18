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

#include "FLEXPRETPipelineAnalysis.h"
#include "arch.h"
#include "SharedAttributes/SharedAttributes.h"

//-----------Public -----------------------------------------------------------------

FLEXPRETPipelineAnalysis::FLEXPRETPipelineAnalysis(Program *p, int nbcache) : MIPSPipelineAnalysis(p, nbcache)
{
	cout << "[FP DEBUG] FLEXPRET PipelineAnalysis Constructor Entered" << endl;
}

// Initialize CHMC attributes to "AH" for scratchpad memory simulation
static bool InitializeCHMCAttributesToAH(Cfg *c, Node *n, void *param)
{
	int nbCacheLevel = *((int *)param);
	SerialisableStringAttribute AHatt("AH"); // Always-hit for scratchpad memory

	if (!c->HasAttribute(ContextListAttributeName))
		return true;

	const ContextList &contexts = (ContextList &)c->GetAttribute(ContextListAttributeName);

	for (ContextList::const_iterator context = contexts.begin(); context != contexts.end(); context++)
	{
		string currentContext = (*context)->getStringId();

		// Set CHMC attribute for each cache level
		for (int level = 1; level <= nbCacheLevel; level++)
		{
			string CHMCAttName = CHMCAttributeNameCode(level);
			string attrName = AnalysisHelper::mkContextAttrName(CHMCAttName, currentContext);

			vector<Instruction *> vi = n->GetInstructions();
			for (size_t i = 0; i < vi.size(); i++)
			{
				if (vi[i]->IsCode())
				{
					vi[i]->SetAttribute(attrName, AHatt);
				}
			}
		}
	}
	return true;
}

bool FLEXPRETPipelineAnalysis::CheckInputAttributes()
{
	// Initialize all CHMC attributes to "AH" for scratchpad memory simulation
	cout << "[FP DEBUG] Initializing CHMC attributes to AH for scratchpad memory" << endl;
	int nbCache = 1; // For scratchpad memory, we only need L1 cache level
	AnalysisHelper::applyToAllNodesRecursive(p, InitializeCHMCAttributesToAH, (void *)&nbCache);

	// Now call the parent class's CheckInputAttributes
	return PipelineAnalysis::CheckInputAttributes();
}

bool FLEXPRETPipelineAnalysis::PerformAnalysis()
{
	return MIPSPipelineAnalysis::PerformAnalysis();
}

// Override scheduleNextInst to properly incorporate instruction-specific latencies
void FLEXPRETPipelineAnalysis::scheduleNextInst(Instruction &inst, vector<InstructionPipeline *> &IP, Context *context, bool first)
{
	string codeInstr = inst.GetCode();
	InstructionPipeline *instTmp = new InstructionPipeline(PIPELINEDEPTH);

	TRACE_PIPELINEANALYSIS(cout << " -- begin scheduleNextInst() instr = " << codeInstr << endl);

	unsigned int fetchAt = IP[IP.size() - 1]->getPipeStage(0)->tick + getFetchLatency(inst, context, first);

	// Fetch stage
	instTmp->insertInstruction(fetchAt);
	// Decode stage
	instTmp->propagateInstruction(1);

	// Check for dependencies
	vector<string> inputs = Arch::getResourceInputs(codeInstr);
	unsigned int i = IP.size() - 1;
	unsigned int depTick = IP[i]->getDependencies(inputs);
	while (depTick == 0 && i > 0)
	{
		i--;
		depTick = IP[i]->getDependencies(inputs);
	}

	// Check FU availability
	vector<string> FUs = Arch::getResourceFunctionalUnits(codeInstr);
	assert(FUs.size() == 1);
	unsigned int FUTick = 0;
	for (i = 0; i < IP.size(); i++)
	{
		unsigned int t = IP[i]->checkAvaliability(FUs[0]);
		if (t > FUTick)
		{
			FUTick = t;
		}
	}

	// Get the max of depTick and FUTick
	int execLat = 0;
	if (depTick > FUTick)
		execLat = depTick - (fetchAt + 3);
	else if (FUTick > depTick)
		execLat = FUTick - (fetchAt + 3);

	if (execLat < 0)
		execLat = 0;

	// FLEXPRET-specific: Get instruction latency and use it for execution stage
	unsigned int lat = Arch::getLatency(codeInstr);
	cout << "[LATENCY DEBUG] Instruction: " << codeInstr << " -> Latency: " << lat << " cycles" << endl;

	// Use the maximum of dependency-based delay and instruction latency
	// This ensures multi-cycle instructions are properly accounted for
	int totalExecLat = (lat > (unsigned int)(execLat + 1)) ? lat : (execLat + 1);

	// Execution stage
	pipeStage *pipeStageTmp = instTmp->propagateInstruction(totalExecLat);
	pipeStageTmp->FU = FUs;
	pipeStageTmp->in = Arch::getResourceInputs(codeInstr);
	if (lat <= 1) // allow bypass
		pipeStageTmp->out = Arch::getResourceOutputs(codeInstr);

	// WB stage, do WB only after preceding inst WB
	unsigned int WBAt = IP[IP.size() - 1]->getPipeStage(PIPELINEDEPTH - 1)->tick + 1;
	int WBLat = WBAt - pipeStageTmp->tick;
	if (WBLat < 0)
		WBLat = 1;
	pipeStageTmp = instTmp->propagateInstruction(WBLat);
	pipeStageTmp->out = Arch::getResourceOutputs(codeInstr);

	IP.push_back(instTmp);
	TRACE_PIPELINEANALYSIS(instTmp->Print());
	TRACE_PIPELINEANALYSIS(cout << " -- end scheduleNextInst()" << endl);
}
