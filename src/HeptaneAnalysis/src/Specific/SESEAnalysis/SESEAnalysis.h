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

This file contains the code to extract SESE (Single-Entry Single-Exit) 
regions, according to the algorithm of Johnson et al:
"The Program Structure Tree: Computing Control Regions in Linear Time", 
published in PLDI 1994.

------------------------------------------------------------------------ */

#ifndef SESE_ANALYSIS_H
#define SESE_ANALYSIS_H
#include "Analysis.h"
#include "Edge.h"
#include "SESERegion.h"
#include "assert.h"
#include <unordered_map>

// Global variables for flat list of regions and tree of regions
// -------------------------------------------------------------
extern vector<SESERegion *> allSESERegions;
class Tree;
extern vector<Tree *> allPST;
extern std::unordered_map<Cfg *, Tree *> SESETree;

// -------------------------------------------------------------
// TreeEdge

// Class defining an edge in the tree of region for a given CFG
//
// -------------------------------------------------------------
class TreeEdge {
private:
  SESERegion *source;
  SESERegion *target;

public:
  // Constructor
  TreeEdge(SESERegion *source, SESERegion *target) {
    this->source = source;
    this->target = target;
  }
  // Simple accessors
  SESERegion *getSource() { return this->source; }
  SESERegion *getTarget() { return this->target; }
  // Dump of tree edge
  void dump() {
    cout << "Tree edge ";
    if (source)
      this->source->dump();
    else
      cout << "None ";
    cout << " --> " << endl;

    if (target)
      this->target->dump();
    else
      cout << "None ";
  }
};

// -------------------------------------------------------------
// Tree

// Class defining a tree of SESE region for a given CFG
//
// -------------------------------------------------------------
class Tree {
private:
  vector<SESERegion *> nodes;
  vector<TreeEdge> edges;
  std::unordered_map<SESERegion *, int> order;

public:
  // Constructor
  Tree() { allPST.push_back(this); }
  Tree(vector<SESERegion *> nodes) { this->nodes = nodes; }
  Tree(vector<SESERegion *> nodes, vector<TreeEdge> edges) {
    this->nodes = nodes;
    this->edges = edges;
  }
  // Getters
  vector<SESERegion *> getNodes() { return this->nodes; }
  vector<TreeEdge> getEdges() { return this->edges; }

  // Addition of nodes and edges
  void addNode(SESERegion *node) {
    this->nodes.push_back(node);
    allSESERegions.push_back(node);
  }
  void addEdge(SESERegion *source, SESERegion *target) {
    bool isSourceNode = false;
    bool isTargetNode = false;
    for (size_t i = 0; i < this->nodes.size(); i++) {
      if (nodes[i] == source)
        isSourceNode = true;
      if (nodes[i] == target)
        isTargetNode = true;
    }
    if (isSourceNode && isTargetNode) {
      this->edges.push_back(TreeEdge(source, target));
    } else {
      cerr << "Tree : Adding an edge with unknown Target or Source" << endl;
    }
  }

  // Node and region filling methods
  void fillNode(Cfg *cfg, SESERegion *curr_reg, Node *startNode, Edge *exit) {
    vector<Node *> succ = cfg->GetSuccessors(startNode);
    for (unsigned int i = 0; i < succ.size(); i++) {
      if (cfg->FindEdge(startNode, succ[i]) == exit)
        break;
      if (!curr_reg->isIn(succ[i])) {
        curr_reg->addNode(succ[i]);
        fillNode(cfg, curr_reg, succ[i], exit);
      }
    }
  }
  void fillRegion(Cfg *cfg) {
    Edge *entry, *exit;
    for (unsigned int i = 0; i < this->nodes.size(); i++) {
      entry = nodes[i]->getEntry();
      exit = nodes[i]->getExit();
      if (!entry) {
        for (unsigned int k = 0; k < cfg->GetAllNodes().size(); k++) {
          nodes[i]->addNode(cfg->GetAllNodes()[k]);
        }
      } else { // the region is a sub region of the program
        for (unsigned int k = 0; k < cfg->GetAllNodes().size(); k++) {
          if (cfg->GetAllNodes()[k] == entry->GetTarget()) {
            nodes[i]->addNode(entry->GetTarget());
            fillNode(cfg, nodes[i], entry->GetTarget(), exit);
          }
        }
      }
    }
  }

  // Get ancestor and children in tree
  SESERegion *getAncestor(SESERegion *s) {
    for (size_t i = 0; i < this->edges.size(); i++)
      if (this->edges[i].getTarget() == s)
        return this->edges[i].getSource();
    return nullptr;
  }
  vector<SESERegion *> getChildren(SESERegion *s) {
    vector<SESERegion *> succ;
    for (size_t i = 0; i < this->edges.size(); i++) {
      if (this->edges[i].getSource() == s) {
        succ.push_back(this->edges[i].getTarget());
      }
    }
    return succ;
  }

  // Get root
  SESERegion *getRoot() {
    for (unsigned int i = 0; i < this->nodes.size(); i++) {
      if (this->getAncestor(nodes[i]) == nullptr)
        return nodes[i];
    }
    return nullptr;
  }

  // Dump functions (for debug)
  void dump() {
    cout << "    Dumping tree with " << nodes.size() << " region" << endl;
    for (unsigned int i = 0; i < this->nodes.size(); i++) {
      cout << "    Region SESE : " << i << endl;
      this->nodes[i]->dump();
    }
  }

  void dumpEdges() {
    cout << "    Dumping tree with " << nodes.size() << " region" << endl;
    for (unsigned int i = 0; i < this->edges.size(); i++) {
      cout << "    Region SESE : " << i << endl;
      this->edges[i].dump();
    }
  }

};

// Default value for DFS level
#define DFS_NUM_DEFAULT -1

// --------------------------------------------------------
//
// Static functions used for the implementation of SESE
// analysis
//
// Remark: why are these function static? They should
//         probably be in SESE analysis
// ---------------------------------------------------------


/**
 * This analysis is defined for developers only
 * in order to avoid the definition of a new analysis.
 *
 * That one can be used directly
 *
 * Used in:
 *  - GNUmakefile
 *  - Generic/Config.h
 *  - Generic/Config.cc
 *
 */
class SESEAnalysis : public Analysis {
private:
  std::vector<cfglib::Cfg *> listCfg;

public:
  /* Constructor */
  SESEAnalysis(Program *p);

  /** Checks if all required attributes are in the CFG
      @return always true.
  */
  bool CheckInputAttributes();

  /** Performs the analysis
      @return always true.
  */
  bool PerformAnalysis();

  /** Remove all private attributes */
  void RemovePrivateAttributes();

  /* Process the cfg to find the SESE regions */
  Tree *processCFG(Cfg *cfg);

  // Traversal of DFS to assign DFS levels and back edge information
  void depthFirstTraversal(Cfg *cfg, Node *startNode,
			   unordered_map<Node *, int> &dfs_num_list,
			   unordered_map<Edge *, bool> &backedge, int &dfs_num);
  
  // Assign equivalence classes to edges (SESE region for each edge)
  std::unordered_map<Edge *, CEClass *>
  assignClasses(Cfg *cfg, unordered_map<Node *, int> &dfs_num_list,
		unordered_map<Edge *, bool> &backedge);
  
  // Build SESE tree per function
  void buildTree(Cfg *cfg, Node *startNode,
	    std::unordered_map<Edge *, CEClass *> classe,
	    SESERegion *context, Tree *pst,
	    std::unordered_map<Node *, bool> *visited);
};


#endif
