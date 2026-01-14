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

This file generates a pdf drawing of all called Cfgs in a program
(dead functions are not printed for readability). In case the analysis
of SESE regions has been performed before, displays the SESE regions and SESE
tree.

Important note: dot is "a bit" picky on displaying subgraphs
(loops, SESE regions). If the subgraphs to be displayed are properly nested,
which is the case for irreduciable loops and SESE regions, it works properly.
In case the subgraphs are not properly nested, dot never complains but
the generated pdf is somewhat random.

------------------------------------------------------------------------ */

#include "Specific/DotPrint/DotPrint.h"
#include "SharedAttributes/SharedAttributes.h"
#include "Specific/SESEAnalysis/SESEAnalysis.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// -----------------------------------------------
// BackEdge: helper function to determine if an
// edge is a back edge
// NB: this function is not efficiently implemented
//     because the library does not allow to know
//     quickly is an edge is a back-edge or not
// -----------------------------------------------
bool DotPrint::BackEdge(Cfg *c, Node *s, Node *d) {
  Edge *e = c->FindEdge(s, d);
  assert(e != NULL);
  vector<Loop *> vl = c->GetAllLoops();

  for (unsigned int l = 0; l < vl.size(); l++) {
    vector<Edge *> vbe = vl[l]->GetBackedges();
    for (unsigned int be = 0; be < vbe.size(); be++) {
      if (vbe[be] == e) {
        return true;
      }
    }
  }
  return false;
}

// -------------------------------------------
// Displays fequencies
// -------------------------------------------
void DotPrint::displayFrequencies(Cfg * c, Node * n, ofstream & os)
{
  if (! c->HasAttribute(ExternalWCETAttributeName)) // LBesnard May 2019
    {
      const ContextList& contexts = (ContextList&) c->GetAttribute(ContextListAttributeName);
      unsigned int nc = contexts.size();
      os << "\\n";
      for (unsigned int ic=0; ic<nc; ic++) 
	{
	  Context *ctx = contexts[ic];
	  string contextName = ctx->getStringId();
	  string AttName = AnalysisHelper::getContextAttrFrequencyName(contextName);
	  if (n->HasAttribute (AttName))
	    {
	      SerialisableUnsignedLongAttribute freq_attr = (SerialisableUnsignedLongAttribute &) n->GetAttribute (AttName);
	      long frequency = freq_attr.GetValue ();
	      os << " frequency_c" << contextName << "=" << frequency << "\\n";
	    }
	}
    }
}

// -------------------------------------------
// Displays the contents of one node and
// prints the name of its successors in the CFG
// -------------------------------------------
bool DotPrint::displayNode (Cfg * c, Node * n, ofstream & os)
{

  if (n->HasAttribute (InternalAttributeNameOK))
    {
      os << "node" << n << ";" << endl;
      return true;		// Node already treated
    }

  os << "node" << n << " [label = \"";

  // Node address and type
  t_address add = AnalysisHelper::getStartAddress (n);
  if (add != INVALID_ADDRESS)
    {
      os << "@0x" << std::hex << add << " " << std::dec;
    }
  if (n->IsBB ())
    {
      os << "(BB";
      if(n->HasAttribute(InternalAttributeId)) {
        NonSerialisableIntegerAttribute &ai = (NonSerialisableIntegerAttribute&) n->GetAttribute(InternalAttributeId);
        os << ": " << to_string(ai.GetValue());
      }
      os << ")";
    }
  else
    {
      os << "(Call ";
      Cfg *callee = n->GetCallee ();
      assert (callee != NULL);
      os << callee->getStringName/*GetName*/ ();
      if (callee->IsExternal ())
	os << ": external";
      
      if(n->HasAttribute(InternalAttributeId)) {
        NonSerialisableIntegerAttribute &ai = (NonSerialisableIntegerAttribute&) n->GetAttribute(InternalAttributeId);
        os << ": " << to_string(ai.GetValue());
      }
      os << ")";
    }
  // Add-on: display CRPD information
  /* { 
      displayCRPD(c,n,os);
      os <<"\\n";
      } */

  NonSerialisableIntegerAttribute OK (0);
  n->SetAttribute (InternalAttributeNameOK, OK);

  displayFrequencies(c, n, os);

  if(detailed) {
    for(Instruction *i : n->GetInstructions()) 
      {
	os << "\\n" << i->GetCode();
      }
  }
    os << "\"";

  Node *entry = c->GetStartNode ();
  if (entry == n)
    os << " ,color=\"blue\"";

  // Display loops heads in green
  vector < Loop * >vl = c->GetAllLoops ();
  for (unsigned int i = 0; i < vl.size (); i++)
    {
      Node *head = vl[i]->GetHead ();
      if (head == n)
	{
	  os << " ,color=\"green\"";
	  break;
	}
    }

  os << "];" << endl;

  return true;
}

// -------------------------------------------
// Displays the successors f a node in the CFG
// -------------------------------------------
bool DotPrint::displaySucs(Cfg *c, Node *n, ofstream &os) {

  // Print its successors in the CFG
  vector<Node *> sucs = c->GetSuccessors(n);
  for (unsigned int i = 0; i < sucs.size(); i++) {
    os << "node" << n << " -> "
       << "node" << sucs[i];
    if (BackEdge(c, n, sucs[i]))
      os << "[color=\"red\"]";
    os << ";" << endl;
  }
  return true;
}

// -------------------------------------------
// Displays a loop
// -------------------------------------------
bool DotPrint::displayLoop(Cfg *c, Loop *l, ofstream &os, vector<Loop *> &vl) {

  // Print the loop if there is no loop in the list above the loop in the list
  if (l != NULL) {
    for (unsigned int nl = 0; nl < vl.size(); nl++) {
      if (vl[nl] != l) {
        if (l->IsNestedIn(vl[nl])) {
          return true;
        }
      }
    }
  }

  // Remove the loop from the list of loops to be printed
  for (std::vector<Loop *>::iterator it = vl.begin(); it != vl.end(); it++) {
    if ((*it) == l) {
      vl.erase(it);
      break;
    }
  }

  // Print the loop nodes
  if (l != NULL) {
    vector<Node *> vn = l->GetAllNodes();
    os << "subgraph "
       << "cluster_loop" << l << " {" << endl;
    int maxiter;
    SerialisableIntegerAttribute ai =
        (SerialisableIntegerAttribute &)l->GetAttribute(MaxiterAttributeName);
    maxiter = ai.GetValue();
    // Print loop information 
    os << "graph [label = \"loop [" << dec << maxiter << "]";
    os << "\"];" << endl;
    for (unsigned int i = 0; i < vn.size(); i++) {
      displayNode(c, vn[i], os);
    }
  }

  // Display subloops of loop l
  vector<Loop *> viter = vl;

  for (unsigned int nl = 0; nl < viter.size(); nl++) {
    bool stillthere = false;
    for (unsigned int tmp = 0; tmp < vl.size(); tmp++)
      if (vl[tmp] == viter[nl])
        stillthere = true;
    if (stillthere && viter[nl]->IsNestedIn(l)) {
      displayLoop(c, viter[nl], os, vl);
    }
  }

  if (l != NULL)
    os << "}" << endl;

  return true;
}

// -------------------------------------------
// Displays a SESE region
// -------------------------------------------
void DotPrint::displaySESERegion(Cfg *c, ofstream &os, SESERegion *r) {
  if (r==NULL) return;
  Tree *pst = SESETree[c];
  // Test of the region is also a loop anf if so, also print its maxiter
  vector<Loop *> vl = c->GetAllLoops();
  bool is_loop = false;
  int maxiter=0;
  for (unsigned int l=0;l<vl.size();l++) {
    if (r->getEntryNode() == vl[l]->GetHead()) {
      SerialisableIntegerAttribute ai =
        (SerialisableIntegerAttribute &)vl[l]->GetAttribute(MaxiterAttributeName);
      maxiter = ai.GetValue();
      is_loop = true;
      break;
    }
  }
  
  // Print the region nodes the cluster for the region + region nodes
  vector<Node *> vn = r->GetAllNodes();
  os << "subgraph " << "cluster_sese_region" << r << " {" << endl;
  if (is_loop == false) {
    os << "graph [label = \"SESE " << dec << r->getId() << "\"];" << endl;
  }
  else {
    os << "graph [label = \"loop[" << dec << maxiter << "] (SESE " << dec << r->getId() << ")" << "\"];" << endl;
  }
  // Displyaing nodes of SESE
  for (unsigned int i = 0; i < vn.size(); i++) {
    // cout << "Displaying a node" << endl;
    displayNode(c, vn[i], os);
  }
  // Recursive call for sub regions
  vector <SESERegion *> subr = pst->getChildren(r);
  for (unsigned int r=0;r<subr.size(); r++) {
    displaySESERegion(c, os, subr[r]);
  }
  os << "};" << endl;
}

void DotPrint::displaySESERegions(Cfg *c, ofstream &os) {
  Tree *pst = SESETree[c];
  SESERegion *root = pst->getRoot();
  if (root==NULL) {
    return;
  }
  vector <SESERegion *> subr = pst->getChildren(root);  
  for (unsigned int r=0;r<subr.size(); r++) {
    displaySESERegion(c, os, subr[r]);
  }
}

// -------------------------------------------
// Displays a SESE region tree
// -------------------------------------------
void DotPrint::displaySESETree(Cfg *c, Tree *pst, ofstream &os) {

  if (pst==NULL) {
    // cout << "No tree to display, returning" << endl;
    return;
  }

  os << "subgraph cluster_SESERegion_of_" << c->getStringName() << " {" << endl;
  os << "graph [label = \"Region tree of " << c->getStringName() << "\"];"
     << endl;

  vector<SESERegion *> vn = pst->getNodes();
  
  t_address add1, add2;

  for (unsigned int i = 0; i < vn.size(); i++) {
    
    bool entire_cfg_region = false;

    if (!vn[i]->getEntry()) {
      entire_cfg_region = true;
    }

    if (!vn[i]->getExit()) {
      entire_cfg_region = true;
    }

    if (!entire_cfg_region) {
      os << "region" << vn[i] << " [label = \"";
      os << "SESE " << dec << vn[i]->getId() << endl;
      add1 = AnalysisHelper::getStartAddress(vn[i]->getEntry()->GetSource());
      add2 = AnalysisHelper::getStartAddress(vn[i]->getEntry()->GetTarget());
      os << "entry = " << hex << add1 << "-->" << hex << add2;
      add1 = AnalysisHelper::getStartAddress(vn[i]->getExit()->GetSource());
      add2 = AnalysisHelper::getStartAddress(vn[i]->getExit()->GetTarget());
      os << "\nexit = " << hex << add1 << "-->" << hex << add2 << "\"];" << endl;
    }
    else {
      os << "region" << vn[i] << " [label = \"";
      os << "SESE " << dec << vn[i]->getId() << endl;
      add1 = AnalysisHelper::getStartAddress(vn[i]->getEntryNode());
      os << "CFG region, entry = " << hex << add1;
      os << "\"];" << endl;
    }
  }

  vector<TreeEdge> edges = pst->getEdges();
  for (unsigned int i = 0; i < edges.size(); i++) {
    os << "region" << edges[i].getSource() << " -> region"
       << edges[i].getTarget() << ";" << endl;
  }

  os << "}" << endl;
}

// -------------------------------------------
// Displays a Cfg
// -------------------------------------------
void DotPrint::displayCfg(Cfg *c, ofstream &os) {  
  os << "subgraph cluster_" << c->getStringName() << " {" << endl;
  bool isSESE = false;
  int num_sese = 0;
  Tree *pst = SESETree[c];

  if (allSESERegions.size() > 0) {
    vector<SESERegion *>vr = pst->getNodes();
    for (unsigned int r=0;r<vr.size();r++) {
      if (vr[r]->getEntryNode() == c->GetStartNode()) {
	num_sese = vr[r]->getId();
	isSESE=true;
	break;
      }
    }
  }

  if (isSESE) {
    os << "graph [label = \"" << c->getStringName() << " (SESE " << dec << num_sese << ")" << "\"];" << endl;
  } else {
    os << "graph [label = \"" << c->getStringName() << "\"];" << endl;

  }

  // Dot prints clusters if and only if one cluster is completely embedded into another
  // Use a different way to traverse the cfg recursively weuther of not the cfg has SESE regions
  if (allSESERegions.size() > 0) {
      displaySESERegions(c, os);
  } else {
    vector<Loop *> vl = c->GetAllLoops();
    displayLoop(c, NULL, os, vl);
    assert(vl.size() == 0);
  }

  // Generate the display of individual nodes
  vector<Node *> vn = c->GetAllNodes();
  for (unsigned int i = 0; i < vn.size(); i++) {
    displayNode(c, vn[i], os);
  }

  os << "}" << endl;

  // Display SESE tree (if non empty)
  displaySESETree(c, pst, os);
}

void DotPrint::displayAllSucs(ofstream &os, Program *p) {
  vector<Cfg *> lc = p->GetAllCfgs();
  for (unsigned int c = 0; c < lc.size(); c++) {
    vector<Node *> vn = lc[c]->GetAllNodes();
    for (unsigned int i = 0; i < vn.size(); i++) {
      displaySucs(lc[c], vn[i], os);
    }
  }
}

// ----------------------
// DotPrint class
// ----------------------
DotPrint::DotPrint (Program * p, string dir, bool d):Analysis (p), directory(dir), detailed(d)
  {
  };

// ----------------------------------------------------------------
// Checks if all required attributes are in the CFG
// Returns true if successful, false otherwise
// Here, nothing specific to do
// ----------------------------------------------------------------
bool DotPrint::CheckInputAttributes() { return true; }

// ----------------------------------------------
// Performs the analysis
// Returns true if successful, false otherwise
// ----------------------------------------------
bool DotPrint::PerformAnalysis() {
  CallGraph *call_graph = new CallGraph(p);
  Cfg *currentCfg;
  
  string filename = this->directory + "/" + p->GetName() + ".dot";
  ofstream os(filename.c_str());
  os << "digraph G {" << endl;
  vector<Cfg *> lc = p->GetAllCfgs();

  for (unsigned int c = 0; c < lc.size(); c++) {
    currentCfg = lc[c];
    if (!call_graph->isDeadCode(currentCfg)) {
      displayCfg(currentCfg, os);
    }
  }
  displayAllSucs(os, p);
  os << "}" << endl;
  os.close();
    
  // Isabelle: changed format to a pdf output, was not managing colors
  // properly with jpg export on version 2.32 (default color seemed to be white,
  // ...)
  string command = "dot -Tpdf " + filename + " -o " + this->directory + "/" +
                   p->GetName() + ".pdf";
  system(command.c_str());
  return true;
}

// Remove all private attributes
void DotPrint::RemovePrivateAttributes() {
  vector<Cfg *> lc = p->GetAllCfgs();
  for (unsigned int c = 0; c < lc.size(); c++) {
    vector<Node *> ln = lc[c]->GetAllNodes();
    for (unsigned int n = 0; n < ln.size(); n++)
      if (ln[n]->HasAttribute(InternalAttributeNameOK))
        ln[n]->RemoveAttribute(InternalAttributeNameOK);
  }
}
