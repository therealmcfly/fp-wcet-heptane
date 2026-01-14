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

#include "Logger.h"
#include "AddressAnalysis.h"
#include "Generic/ContextHelper.h"
#include "arch.h"
#include "Utl.h"

#define LOCTRACE(s)  


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------


#define getRegMemNode(NODE, IDATTR) ((AbstractRegMemAttribute&)NODE->GetAttribute(IDATTR))
#define getRegMemContextualNode(ELEM, IDATTR) getRegMemNode(ELEM.node,IDATTR)
#define MyLocalAttribute "cfgStackArea"

bool AddressAnalysis::getCodeAddress( Instruction * vinstr, long *add)
{
  if (!vinstr->HasAttribute (AddressAttributeName)) return false;
  AddressAttribute attr = (AddressAttribute &) vinstr->GetAttribute (AddressAttributeName);
  *add = attr.getCodeAddress ();
  return true;
}


/**
   Assigns to a node (n), for each context of its cfg (c) 
   - an empty "Addr_in" attribute (AddressInName)
   - an empty "Addr_out" attribute (AddressOutName)
   The attributes are different.   
*/
bool static initAddressAnalysis(Cfg * c, Node * n, void *param)
{
  AddressAnalysis *ca = (AddressAnalysis *) param;
  int InternalSizeStack =  ca-> GetRequiredStackSize(); 
  string in = AddressInName;
  string out = AddressOutName;


  assert(c->HasAttribute(ContextListAttributeName));
  const ContextList & contexts = (ContextList &) c->GetAttribute(ContextListAttributeName);

  string NumBlock = n->getIdentifier();
  //  int nb = Utl::string2int(NumBlock);
  
  for (ContextList::const_iterator context = contexts.begin(); context != contexts.end(); context++)
    {

      // n->SetAttribute(in + currentContext, att);
      // n->SetAttribute(out + currentContext, att);
      // replaced by (October 1st 2019, LBesnard)

      string currentContext = (*context)->getStringId();
      AbstractRegMem regMem_empty1 = AbstractRegMem(ca->NewRegState(InternalSizeStack));
      AbstractRegMemAttribute att1(regMem_empty1);
      n->SetAttribute(in + currentContext, att1);

      AbstractRegMem regMem_empty2 = AbstractRegMem(ca->NewRegState(InternalSizeStack));
      AbstractRegMemAttribute att2(regMem_empty2);
      n->SetAttribute(out + currentContext, att2);
    }
  return true;
}

/** 
    @return true if n is a "endnode" of c.
*/
bool static isEndNode(Cfg *c, Node * n)
{
  vector<Node*> exitNodes= c->GetEndNodes();
  return (std::find(exitNodes.begin(), exitNodes.end(), n) !=  exitNodes.end());
}


/** 
    Reseting the sp register of a abstrat register (vRegMem) when a node (n) is the start or exit node of a cfg (c).
*/
void static resetSPRegister(Cfg *c, Node * n, AbstractRegMem &vRegMem)
{
  if ( n == c->GetStartNode() || isEndNode(c, n))
     vRegMem.reset();
}


/**
   Assigns to each load/store instruction and for all context the AddressAttributeName
*/
bool static finalAddressAnalysis(Cfg * c, Node * n, void *param)
{
  AddressAnalysis *ca = (AddressAnalysis *) param;

  string in = AddressInName;
  string out = AddressOutName;

  string NumBlock = n->getIdentifier();
  // int nb = Utl::string2int(NumBlock);
  
  assert(c->HasAttribute(ContextListAttributeName));
  const ContextList & contexts = (ContextList &) c->GetAttribute(ContextListAttributeName);
  for (ContextList::const_iterator context = contexts.begin(); context != contexts.end(); context++)
    {
      string currentContext = (*context)->getStringId();
      assert(n->HasAttribute(in + currentContext));
      AbstractRegMemAttribute& ca_attr_in = getRegMemNode(n, in + currentContext);
      AbstractRegMem v_in = ca_attr_in.getAbstractRegMem();
      RegState *state= v_in.getRegState();

      TRACE( cout << " Begin  finalAddressAnalysis for Node num = " << NumBlock << ", context = " << in + currentContext << " node = " << n << " state = " << state << endl; v_in.print(););

      vector < Instruction * >vi = n->GetAsm();
      for (size_t i = 0; i < vi.size(); i++)
	{
	  Instruction *vinstr = vi[i];
	  // Setting the address attribute for load and Store instructions.
	  if (ca->setLoadStoreAddressAttribute(c, vinstr, state, *context))
	    { 
	      LOCTRACE(cout << "   setLoadStoreAddressAttribute APPLIED " << endl); 
	    }
	  state->simulate (vinstr);
	}
      resetSPRegister(c, n, v_in);

      n->RemoveAttribute(in + currentContext);
      n->RemoveAttribute(out + currentContext); 
    }
  
  return true;
}


/** Set the adress attribute (info) to an instruction (vinstr). The attribute is NOT a contextual one. */
void AddressAnalysis::setAddressAttribute(Instruction* vinstr, AddressInfo & info)
{ 
  AddressAttribute attribute;

  if (vinstr->HasAttribute (AddressAttributeName))
    {
      attribute = (AddressAttribute &) vinstr->GetAttribute (AddressAttributeName);
    }
  attribute.addInfo (info);
  vinstr->SetAttribute (string (AddressAttributeName), attribute);
}

void AddressAnalysis::setContextualAddressAttribute(Instruction* Instr, Context *context, AddressInfo &contextual_info)
{
 // Attach the contextual address attribute
  string attribute_name = AnalysisHelper::mkContextAttrName(AddressAttributeName, context);
  if (Instr->HasAttribute (attribute_name)) 
    cout << " Instr " << Instr->GetCode () << " has already the attribute +++ ***++++ " << attribute_name << endl;
  assert (! Instr->HasAttribute (attribute_name));
  AddressAttribute contextual_address;
  contextual_address.addInfo (contextual_info);
  Instr->SetAttribute (attribute_name, contextual_address);
}


/**
   Check if a cfg is a leaf in the call graph (no dedicated stack frame in mips).
   @return true if a cfg (cfg) calls an another cfg, "false" otherwise.
 */
bool AddressAnalysis::isCfgWithCallNode (Cfg * cfg)
{
  vector < Node * >nodes = cfg->GetAllNodes ();
  for (size_t i = 0; i < nodes.size (); i++)
    {
      if (nodes[i]->IsCall ()) return true;
    }
  return false;
}

bool AddressAnalysis::CheckExternalCfg(Program *p)
{

  vector < Cfg * >listCfg = p->GetAllCfgs ();
  for (size_t cfg = 0; cfg < listCfg.size (); cfg++)
    {
      if (listCfg[cfg]->GetStartNode () == NULL) return true;
    }
  return false;
}

void AddressAnalysis::setPointerAccessInfo(Instruction* vinstr, string access)
{
  printPointerAccessInfos(vinstr);

  Logger::addWarning ("pointer analysis does not exist: stub all addresses can be accessed");
  long addr_begin = symbol_table.getCodeStartAddr ();
  long addr_end = spinit;
  mkAddressInfoAttribute(vinstr, access, false, "all", "pointer fallback", addr_begin, addr_end - addr_begin + 1);
}


void AddressAnalysis::mkAddressInfoAttribute(Instruction * Instr, string access, bool precision, string section_name, string var_name, long addr,  int size)
{
  AddressInfo info = mkAddressInfo( Instr, access, precision, section_name,  var_name,  addr,   size);
  setAddressAttribute(Instr, info);
}

AddressInfo AddressAnalysis::mkAddressInfo(Instruction * Instr, string access, bool precision, string section_name, string var_name, long addr,  int size)
{
  AddressInfo info;

  info.setType (access);
  info.setPrecision(precision);
  info.setSegment (section_name);
  info.setName (var_name);
  info.addAdrSize ( Utl::int2cstring (addr), Utl::int2cstring (size));
  LOCTRACE(info.print());
  return info;
}

//-----------Public -----------------------------------------------------------------

AddressAnalysis::AddressAnalysis (Program * prog, int sp):StackAnalysis (prog, sp)
{
  p = prog;
  spinit = sp;
  //initialization of sp value with 7FFF FFFF - virtual pages alignement
  //long spinit=2147483647 - ( 2147483647 % TAILLEPAGE);
}

bool
AddressAnalysis::PerformAnalysis ()
{
  if (CheckExternalCfg(p)) { Logger::addFatal ("AddressAnalysis: external Cfg detected in DataAddressAnalysis");}
  stackAnalysis ();
  int vs = GetRequiredStackSize();
  Logger::addInfo(" ++++ Stack size = " + Utl::int2cstring (vs) );
  Logger::print ();

  intraBlockDataAnalysis ();

  LOCTRACE( cout << " TRACE  finalAddressAnalysis " << endl;);
  AnalysisHelper::applyToAllNodesRecursive(p, finalAddressAnalysis, (void *)this);
  Logger::print ();

  return !(Logger::getErrorState ());
}

// Checks if all required attributes are in the CFG
// Returns true if successful, false otherwise
bool
AddressAnalysis::CheckInputAttributes ()
{
  return p->HasAttribute (SymbolTableAttributeName);
}

//nothig to remove
void
AddressAnalysis::RemovePrivateAttributes ()
{}


// -------------------------------

/*
  @return the AbstractRegMem of a ContextualNode (current). 
  The initial value is the given by the current analysis provided by inAnalysisName for the context.
*/
AbstractRegMem AddressAnalysis::compute_out(ContextualNode & current, string & inAnalysisName)
{
  Node * currentNode=current.getNode();
  Cfg* vCfg=currentNode->GetCfg();
  string idCurrentContext = current.context->getStringId();

  AbstractRegMemAttribute  &ca_attr_in = getRegMemContextualNode( current, inAnalysisName + idCurrentContext);
  AbstractRegMem v_out = ca_attr_in.getAbstractRegMem(); 
  RegState *state = v_out.getRegState();

  // string idAccessName = AnalysisHelper::mkContextAttrName(inAnalysisName, idCurrentContext);
  vector < Instruction * >vi = currentNode->GetAsm();

  for (size_t i = 0; i < vi.size(); i++)
    {
      LOCTRACE( 
	       cout << "-----------------------------------------------------------" << endl;
	       long vaddress; getCodeAddress( vi[i], &vaddress);
	       cout << " compute_out = " << vi[i] ->GetCode () << " at " << std::hex << vaddress << " " << std::dec << endl;
	       cout << " BEFORE = " << endl; state->printStates(); state->printStack(); 
	   	);
      
      state->simulate (vi[i]);

      LOCTRACE( 
	       cout << " AFTER = " << endl; state->printStates(); state->printStack(); 
	);
    }
  resetSPRegister(vCfg, currentNode, v_out);
  return v_out;
}

set < ContextualNode > AddressAnalysis::intraBlockDataAnalysis_out(set < ContextualNode > &work, set < ContextualNode > &visited)
{
  string in = AddressInName;
  string out = AddressOutName;

  LOCTRACE( cout << "intraBlockDataAnalysis_out , BEGIN " << endl;);
  set < ContextualNode > work_in;
  for (set < ContextualNode >::iterator it = work.begin(); it != work.end(); it++)
    {
      ContextualNode current = *it;
      string currentContext = current.context->getStringId();
      string currentContextOut=out + currentContext;

      AbstractRegMemAttribute &ca_attr_out = getRegMemContextualNode(current, currentContextOut);
      AbstractRegMem vAbstractRegMem_out = ca_attr_out.getAbstractRegMem();

      TRACE(  
	    string NumBlock = current.getNode()->getIdentifier();
	    cout << endl << " ++++  intraBlockDataAnalysis_out START BLOCK " << NumBlock << ", CA_ATTR_OUT context = " << currentContextOut << " address vAbstractRegMem_out = " << vAbstractRegMem_out.getRegState()<< endl;
	    vAbstractRegMem_out.print();
	    string currentContextIn = in + currentContext ;
	    AbstractRegMemAttribute  &ca_attr_in = getRegMemContextualNode(current, currentContextIn);
	    AbstractRegMem vAbstractRegMem_in = ca_attr_in.getAbstractRegMem();
	    cout << endl << " ++++  intraBlockDataAnalysis_out START BLOCK " << NumBlock << ", CA_ATTR_IN, context = " << currentContextIn << " address vAbstractRegMem_in = " << vAbstractRegMem_in.getRegState() << endl;
	    vAbstractRegMem_in.print();
	   );
      
      AbstractRegMem v_out = compute_out(current, in);

      bool b = ! vAbstractRegMem_out.EqualsRegisters(v_out);
      if (b) ca_attr_out.setAbstractRegMemRegisters(v_out);

      bool b1 = ! vAbstractRegMem_out.EqualsStacks(v_out);
      if (b1) ca_attr_out.setAbstractRegMemStack(v_out);

      b = b || b1;
      // To force the visit of all nodes : lbesnard.
      if (visited.find(current) == visited.end())
	{ 
	  visited.insert(current);   
	  b = true;
	}

      if (b) AnalysisHelper::insertContextualSuccessors(current, work_in);
      TRACE( 
	    cout << endl << " ++++  intraBlockDataAnalysis_out END BLOCK " << NumBlock << endl; 
	    vAbstractRegMem_out.print();
	    cout << " End of context intraBlockDataAnalysis_out, END BLOCK with CA_ATTR_IN " << endl; 
	    vAbstractRegMem_in.print(); 
	     );
    }
  
  LOCTRACE( cout << "intraBlockDataAnalysis_out , END " << endl; );
  return work_in;
}

set < ContextualNode > AddressAnalysis::intraBlockDataAnalysis_in(set < ContextualNode > &work_in, set < ContextualNode > &visited)
{
  set < ContextualNode > work;
  set < Edge * >backedges;  // empty set.

  LOCTRACE( cout << "intraBlockDataAnalysis_IN " << endl;);
  for (set < ContextualNode >::iterator it = work_in.begin(); it != work_in.end(); it++)
    {
      ContextualNode current = *it;
      // Force the visit of all the nodes
      if (updateRegistersAndStack_in(current, backedges) || (visited.find(current) == visited.end())) 
	work.insert(current);
    }
  LOCTRACE( cout << "intraBlockDataAnalysis_IN , END " << endl;);
  return work;
}


/* 
   FixPointStepInit_out analysis: Compute the "Stack_out" a set of nodes (work), without considering backedges.
   @return a set of nodes for which the Stack_in must be computed.
*/
set < ContextualNode > AddressAnalysis::FixPointStepInit_out(set < ContextualNode > &work, set < Edge * >&backedges, set < ContextualNode > &visited)
{
  string in = AddressInName;
  string out = AddressOutName;

  set < ContextualNode > work_in;
  for (set < ContextualNode >::iterator it = work.begin(); it != work.end(); it++)
    {
      ContextualNode current = *it;
      string NumBlock = current.node->getIdentifier();
      AbstractRegMem v_out = compute_out(current, in);
      AbstractRegMemAttribute &ca_attr_out = getRegMemContextualNode(current, out + current.context->getStringId());
      AbstractRegMem vAbstractRegMem_out = ca_attr_out.getAbstractRegMem();

      bool b = ! vAbstractRegMem_out.EqualsRegisters(v_out);
      if (b) ca_attr_out.setAbstractRegMemRegisters(v_out);
      bool b1 = ! vAbstractRegMem_out.EqualsStacks(v_out);
      if (b1)  ca_attr_out.setAbstractRegMemStack(v_out);
      if (visited.find(current) == visited.end())
	{
	  visited.insert(current);
	  b = true;
	} 
      if (b1 || b)
	AnalysisHelper::insertContextualSuccessorsExcludingBackEdges(current, work_in, backedges);
      
    }
  return work_in;
}


bool AddressAnalysis::updateRegistersAndStack_in(ContextualNode &current, set < Edge * >&backedges)
{
  string in = AddressInName;
  string out = AddressOutName;
  set < ContextualNode > work;
  ContextualNode pred;
  string idAttr;
  bool b, b1, first;
  AbstractRegMem vout, new_in;

  string NumBlock = current.node->getIdentifier();
  // printNodes(current.node);
  // int nb = Utl::string2int(NumBlock);

  string currentContext = current.context->getStringId();
  string currentContextIn = in + currentContext;
  AbstractRegMemAttribute &ca_attr_in = getRegMemContextualNode (current, currentContextIn);      
  AbstractRegMem vAbstractRegMem_in = ca_attr_in.getAbstractRegMem();

  //   cout << nb << endl;

  TRACE( cout << endl << " ++++  intraBlockDataAnalysis_in START BLOCK " << nb /*NumBlock*/ << ", CA_ATTR_IN, context = " << currentContextIn << " address vAbstractRegMem_in = " << vAbstractRegMem_in.getRegState() << endl;
	 vAbstractRegMem_in.print(); );

  // Registers
  const vector < ContextualNode > &predecessors = GetContextualPredecessors(current);
  assert(predecessors.size() != 0);	// not the program's entry node
  first = true;
  for (size_t i = 0; i < predecessors.size(); i++)
    {
      pred = predecessors[i];
      string NumBlock1 = pred.node->getIdentifier();
      // int nb1 = Utl::string2int(NumBlock1);
      
      // Ignoring the backedges
      b =  AnalysisHelper::FilterBackedge(current.node, pred.node, backedges);
      if (b)
	{
	  idAttr = out + pred.context->getStringId();
	  if (first)
	    {
	      new_in = getRegMemContextualNode(pred, idAttr).getAbstractRegMem();
	      first = false;  
	    }
	  else
	    {
	      vout =  getRegMemContextualNode(pred, idAttr).getAbstractRegMem();
	      new_in.JoinRegisters(vout);
	    }
	}
    }
  b = ! vAbstractRegMem_in.EqualsRegisters(new_in);
  if (b) ca_attr_in.setAbstractRegMemRegisters(new_in);
  
  // Stack
  b1 = false;
  first = true; 
  for (size_t i = 0; i < predecessors.size(); i++)
    {
      pred = predecessors[i];
      // Ignoring the backedges
      b =  AnalysisHelper::FilterBackedge(current.node, pred.node, backedges);
      if (b)
	{ 
	  idAttr = out + pred.context->getStringId();
	  if (first) 
	    {
	      new_in = getRegMemContextualNode(pred, out + pred.context->getStringId()).getAbstractRegMem();
	      first = false;
	    }
	  else
	    {
	      vout = getRegMemContextualNode(pred, out + pred.context->getStringId()).getAbstractRegMem();
	      new_in.JoinStacks(vout);
	    }
	}
    }
  vAbstractRegMem_in = ca_attr_in.getAbstractRegMem();
  b1 = ! vAbstractRegMem_in.EqualsStacks(new_in);
  if (b1) ca_attr_in.setAbstractRegMemStack(new_in);
  
  TRACE (cout << endl << " ++++  intraBlockDataAnalysis_in END BLOCK " << NumBlock << ", CA_ATTR_IN, context = " << 
	 currentContextIn << " address vAbstractRegMem_in = " << vAbstractRegMem_in.getRegState() << endl;
	 vAbstractRegMem_in.print();
	 );
  
  return  b1 || b;
}

/* FixPointStepInit_in analysis: Compute the "Stack_in" a set of nodes (work), without considering backedges.
   @return a set of nodes for which the Stack_out must be computed. */
set < ContextualNode > AddressAnalysis::FixPointStepInit_in(set < ContextualNode > &work_in, set < Edge * >&backedges, set < ContextualNode > &visited)
{
  set < ContextualNode > work;
  // cout << " >>> FixPointStepInit_in " << endl;
  for (set < ContextualNode >::iterator it = work_in.begin(); it != work_in.end(); it++)
    {
      ContextualNode current = *it;
      // Force the visit of all the nodes
      if (updateRegistersAndStack_in(current, backedges) || (visited.find(current) == visited.end())) 
	work.insert(current);
    }
  // cout << " <<< FixPointStepInit_in" << endl;
  return work;
}

/* blabla blabla blabla 
*/
bool AddressAnalysis::FixPointInit()
{
  set < ContextualNode > visited, work_in, work;
  set < Edge * >backedges = AnalysisHelper::compute_backedges(p, call_graph);
  work = AnalysisHelper::initWork();
  while (!work.empty())
    {
      work_in = FixPointStepInit_out(work, backedges, visited);
      work.clear();
      work = FixPointStepInit_in(work_in, backedges, visited);
      work_in.clear();
    }
  // cout << " >>>>>>>>>>>>>>>>>>>>>  End of FixPointInit " << endl;
  return true;
}

/*
  Address Analysis implemented as a data flow analysis 
  All nodes have to be visited at least once.
*/
bool AddressAnalysis::intraBlockDataAnalysis()
{
  set < ContextualNode > visited, work_in, work;

  this->call_graph = new CallGraph(p);

  // Initialising.
  symbol_table = (SymbolTableAttribute &) p->GetAttribute (SymbolTableAttributeName);
  AnalysisHelper::applyToAllNodesRecursive(p, initAddressAnalysis, (void *)this);

  FixPointInit();

  // fix point
  work = AnalysisHelper::initWork();
  while (!work.empty())
    {
      work_in = intraBlockDataAnalysis_out(work, visited);
      work.clear();
      work = intraBlockDataAnalysis_in(work_in, visited);
      work_in.clear();
    }
  return true;
}


void  AddressAnalysis::printNodes(Node *node)
{
  string NumBlock = node->getIdentifier();
  int nb = Utl::string2int(NumBlock);
  cout << " >>> printing NumBlock " << nb << endl;
  std::vector<Instruction*>  instructions = node->GetInstructions(); 
  for (size_t i=0; i<instructions.size(); i++)
    cout << "      " << instructions[i]->GetCode () << endl;
  cout << " <<< printing NumBlock " << nb << endl;
}
