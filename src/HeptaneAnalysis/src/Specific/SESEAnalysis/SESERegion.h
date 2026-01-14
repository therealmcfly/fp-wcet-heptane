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

This file defines the data structures for SESE (Single-Entry Single-Exit) 
regions, according to the algorithm of Johnson et al:
"The Program Structure Tree: Computing Control Regions in Linear Time", 
published in PLDI 1994. The code for SESE analysis is in file SESEAnalysis.cc/h

------------------------------------------------------------------------ */
  
#ifndef __SESEREGION_H_
#define __SESEREGION_H_
#include "Analysis.h"
#include "Attributed.h"
#include "Edge.h"
#include "Node.h"
#include "assert.h"
#include <iostream>
#include <unordered_map>
#include <vector>

// Use of class Tree
// -----------------
class Tree;

// Export of global variable allPST
// containing the tree of regions for all Cfgs
// -------------------------------------------
extern vector<Tree *> allPST;

// ----------------------------------------------------
//
// Equivalence class as defined in the
// algorithm. Each class corresponds to a canonical SESE
// region. Equivalence classes are associated to Cfg edges
//
// ----------------------------------------------------
class CEClass {
  int count;              // number of edges in this class
  bool first;
  cfglib::Edge *backEdge; // backEdge associated with the class, if any. There
                          // is 0 or 1 backEdge per class
public:
  // Constructor
  inline CEClass(void) : count(0), first(true), backEdge(NULL) {}

  // Account for tree edges (that are not backedges) in this class
  inline void inc(void) { count++; }

  // Account for the backedge of this class, verifying that it is counted only
  // once
  inline void inc(cfglib::Edge *bracket) {
    // Two back-edges cannot be cycle-equivalent.
    assert((backEdge == NULL) || (backEdge == bracket));
    if (backEdge == NULL)
      count++;
    backEdge = bracket;
  }

  // Decrement number of edges in class
  inline void dec(void) {
    count--;
    this->first = false;
  }
  // Basic methods in equivalence classes
  inline bool isLast(void) { return (count == 1); }
  inline bool isFirst(void) { return first; }
  inline int getCount(void) { return (count); }
};

// ----------------------------------------------------
//
// Class corresponding to a SESE region
//
// ----------------------------------------------------
static int last_id=0; // Static number for region numbering (debug)

class SESERegion : public cfglib::Attributed {
private:
  /* SESE Regions are defined by an entry edge and an exit edge.
  There must be one, and only one, entry edge for the region. Same for the exit
  edge.  */
  Cfg *c;
  cfglib::Edge *entry;
  cfglib::Edge *exit;
  bool first, last;

  /* Two edgeq make a SESE Region if they are cycle equivalent. */
  CEClass *classe;

  /* Informations for the DotPrint analysis */
  vector<cfglib::Node *> nodes;
  int id;

public:
  // Constructor
  SESERegion(Cfg *_c, cfglib::Edge *_entry = NULL, cfglib::Edge *_exit = NULL,
             bool _first = false, bool _last = false, CEClass *_classe = NULL)
      : c(_c), entry(_entry), exit(_exit), first(_first), last(_last),
        classe(_classe) {
    id = last_id;
    last_id++;
  }
  // Destructor
  virtual ~SESERegion(){};

  // Setters, for info not always known at region creation time
  void setExit(cfglib::Edge *exit) { this->exit = exit; }
  void setEntry(cfglib::Edge *entry) { this->entry = entry; }
  void setLast(bool last) { this->last = last; }
  void setFirst(bool first) { this->first = first; }
  void setClass(CEClass *classe) { this->classe = classe; }
  void addNode(cfglib::Node *n) { this->nodes.push_back(n); }

  // Getters
  cfglib::Edge *getExit() { return this->exit; }
  cfglib::Edge *getEntry() { return this->entry; }
  Node *getEntryNode() {
    if (this->entry)
      return this->entry->GetTarget();
    return c->GetStartNode();
  }
  bool isLast() { return this->last; }
  bool isFirst() { return this->first; }
  CEClass *getClass() { return this->classe; }
  std::vector<cfglib::Node *> GetAllNodes() { return this->nodes; }
  std::vector<cfglib::Node *> getNodes() { return this->nodes; }

  // Serialisation and deserialisation (needed from Attributed)
  std::ostream &WriteXml(std::ostream &os, cfglib::Handle &hand);
  virtual void ReadXml(XmlTag const *tag, cfglib::Handle &hand);

  // Find if a region is nested in another comparing their nodes
  bool IsNestedIn(SESERegion *l);

  // Dump of region
  void dump();

  // Tests if a node is in a SESE region
  bool isIn(Node *n) {
    for (long unsigned int i = 0; i < this->nodes.size(); i++) {
      if (nodes[i] == n)
        return true;
    }
    return false;
  }

  // Get region id
  int getId() {return id;}
};
#endif // __SESEREGION_H_
