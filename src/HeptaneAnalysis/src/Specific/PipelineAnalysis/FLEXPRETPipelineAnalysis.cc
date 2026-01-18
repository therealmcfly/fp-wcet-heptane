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
	PIPELINEDEPTH = 5; // FLEXPRET uses a 5-stage pipeline: IF, ID, EX, MEM, WB
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

/**
 * FLEXPRET 5-stage pipeline: IF, ID, EX, MEM, WB
 * Schedule the first instruction of a basic block.
 */
void FLEXPRETPipelineAnalysis::scheduleFirstInst(Instruction &inst, vector<InstructionPipeline *> &IP, Context *context, bool first)
{
	string codeInstr = inst.GetCode();
	InstructionPipeline *instTmp = new InstructionPipeline(PIPELINEDEPTH);
	pipeStage *pipeStageTmp;

	TRACE_PIPELINEANALYSIS(cout << " -- begin scheduleFirstInst() instr = " << codeInstr << endl);

	unsigned int fetchAt = getFetchLatency(inst, context, first);

	// Stage 1: IF (Instruction Fetch)
	instTmp->insertInstruction(fetchAt);

	// Stage 2: ID (Instruction Decode)
	instTmp->propagateInstruction(1);

	// Stage 3: EX (Execute) - use instruction-specific latency
	unsigned int lat = Arch::getLatency(codeInstr);
	pipeStageTmp = instTmp->propagateInstruction(lat);
	pipeStageTmp->FU = Arch::getResourceFunctionalUnits(codeInstr);
	pipeStageTmp->in = Arch::getResourceInputs(codeInstr);
	assert(pipeStageTmp->FU.size() == 1);

	// Stage 4: MEM (Memory Access)
	instTmp->propagateInstruction(1);

	// Stage 5: WB (Write Back)
	pipeStageTmp = instTmp->propagateInstruction(1);
	pipeStageTmp->out = Arch::getResourceOutputs(codeInstr);

	IP.push_back(instTmp);
	TRACE_PIPELINEANALYSIS(instTmp->Print());
	TRACE_PIPELINEANALYSIS(cout << " -- end scheduleFirstInst " << endl);
}

/**
 * FLEXPRET 5-stage pipeline: IF, ID, EX, MEM, WB
 * Schedule subsequent instructions of a basic block.
 */
void FLEXPRETPipelineAnalysis::scheduleNextInst(Instruction &inst, vector<InstructionPipeline *> &IP, Context *context, bool first)
{
	string codeInstr = inst.GetCode();
	InstructionPipeline *instTmp = new InstructionPipeline(PIPELINEDEPTH);

	TRACE_PIPELINEANALYSIS(cout << " -- begin scheduleNextInst() instr = " << codeInstr << endl);

	unsigned int fetchAt = IP[IP.size() - 1]->getPipeStage(0)->tick + getFetchLatency(inst, context, first);

	// Stage 1: IF (Instruction Fetch)
	instTmp->insertInstruction(fetchAt);

	// Stage 2: ID (Instruction Decode)
	instTmp->propagateInstruction(1);

	// Check for data dependencies
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

	// Calculate execution stage delay based on dependencies/hazards
	// For 5-stage pipeline: IF(0), ID(1), EX(2), MEM(3), WB(4)
	// EX starts at fetchAt + 2
	int execLat = 0;
	if (depTick > FUTick)
		execLat = depTick - (fetchAt + 2);
	else if (FUTick > depTick)
		execLat = FUTick - (fetchAt + 2);

	if (execLat < 0)
		execLat = 0;

	// Stage 3: EX (Execute) - use instruction-specific latency
	unsigned int lat = Arch::getLatency(codeInstr);
	int totalExecLat = (lat > (unsigned int)(execLat + 1)) ? lat : (execLat + 1);

	pipeStage *pipeStageTmp = instTmp->propagateInstruction(totalExecLat);
	pipeStageTmp->FU = FUs;
	pipeStageTmp->in = Arch::getResourceInputs(codeInstr);
	if (lat <= 1) // allow bypass for single-cycle instructions
		pipeStageTmp->out = Arch::getResourceOutputs(codeInstr);

	// Stage 4: MEM (Memory Access)
	instTmp->propagateInstruction(1);

	// Stage 5: WB (Write Back) - must wait for previous instruction's WB
	unsigned int WBAt = IP[IP.size() - 1]->getPipeStage(PIPELINEDEPTH - 1)->tick + 1;
	pipeStageTmp = instTmp->getPipeStage(PIPELINEDEPTH - 2); // MEM stage
	int WBLat = WBAt - pipeStageTmp->tick;
	if (WBLat < 1)
		WBLat = 1;
	pipeStageTmp = instTmp->propagateInstruction(WBLat);
	pipeStageTmp->out = Arch::getResourceOutputs(codeInstr);

	IP.push_back(instTmp);
	TRACE_PIPELINEANALYSIS(instTmp->Print());
	TRACE_PIPELINEANALYSIS(cout << " -- end scheduleNextInst()" << endl);
}
