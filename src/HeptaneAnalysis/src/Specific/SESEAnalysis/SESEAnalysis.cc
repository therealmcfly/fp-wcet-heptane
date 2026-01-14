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

#include "Attributed.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "SESEAnalysis.h"
#include "SESERegion.h"
#include "SharedAttributes/SharedAttributes.h"
#include "Specific/SESEAnalysis/SESEAnalysis.h"
#include "assert.h"
#include <cstring>

// Global variable containing the constructed SESE tree
// ----------------------------------------------------
std::unordered_map<Cfg *, Tree *> SESETree;

// -----------------------------------------------------------------------
// SESEAnalysis utility functions
// -----------------------------------------------------------------------

// Search object in list vector (perhaps this function should be somewhere else)
template <typename T> int search(vector<T> list, T object) {
  for (int i = 0; i < list.size(); i++) {
    if (list[i] == object)
      return i;
  }
  return -1;
}

// Dump cfg edge on stout (same, perhaps this function should be somewhere else)
/* static void print_edge(Edge *e) {
  if (e==NULL) {
    cout << "NULL" << endl;
    return;
  }
  Node *n1 = e->GetSource();
  Node *n2 = e->GetTarget();     
  t_address add1 = AnalysisHelper::getStartAddress(n1);
  t_address add2 = AnalysisHelper::getStartAddress(n2);
  cout << "@0x" << hex << add1 << ",@0x" << hex << add2 << endl;
  }*/

// ----------------------------------------------------------------
// depthFirstTraversal
//
// DFS traversal of a Cfg considered as an non directed graph,
// starting at a given node passed as parameter.
// This function has two effects
// - fills in the DFS level of each traversed node
// - indicates if edges in the Cfg are backedges
//
// Parameters :
// - cfg:          Cfg to be traversed
// - startNode:    first node to be traversed
// - dfs_num_list: reference to map of dfs ordering of Nodes, filled-in by the function
// - backedge:     reference to map <edge,bool> that indicate which edge is a back edge
//                 by default, backedge set to true for all edges, this function
//                 sets backedge to false when nodes are traversed by DFS
// - dfs_num:      current value in DFS traversal, for recursive calls to the function
//
// Remarks:
// - implicitely assumes that startNode belongs to the Cfg. No check is done for
//   complexity considerations (requires a sequential search in node list at all calls)
// - by construction, assigns a DFS level only to node that are reachable from
//   the entry point of the CFG, else (dead basic blocks) the DFS is kept to the initial
//   value DFS_NUM_DEFAULT
// 
// ----------------------------------------------------------------
void SESEAnalysis::depthFirstTraversal(Cfg *cfg, Node *startNode,
                      unordered_map<Node *, int> &dfs_num_list,
                      unordered_map<Edge *, bool> &backedge, int &dfs_num) {

  // Set DFS level of start node
  dfs_num_list[startNode] = dfs_num;
  dfs_num++;

  // Get preds and sucs
  vector<Node *> succ = cfg->GetSuccessors(startNode);
  vector<Node *> pred = cfg->GetPredecessors(startNode);

  /* The DFS is supposed to be applied on an undirected graph
   * Our Cfg is oriented so each time we consider a sucessor we do the same with
   * predecessor */
  for (long unsigned int j = 0; j < succ.size(); j++) {
    if (dfs_num_list[succ[j]] == DFS_NUM_DEFAULT) {
      backedge[cfg->FindEdge(startNode, succ[j])] = false;
      depthFirstTraversal(cfg, succ[j], dfs_num_list, backedge, dfs_num);
    }
  }
  for (long unsigned int j = 0; j < pred.size(); j++) {
    if (dfs_num_list[pred[j]] ==  DFS_NUM_DEFAULT) {
      backedge[cfg->FindEdge(pred[j], startNode)] = false;
      depthFirstTraversal(cfg, pred[j], dfs_num_list, backedge, dfs_num);
    }
  }
}

// ----------------------------------------------------------------
// assignClasses
//
// Assign equivalence classes to edges (SESE region for each edge)
//
// Parameters :
// - cfg:          Cfg under study
// - dfs_num_list: reference to map of dfs ordering of Nodes
// - backedge:     reference to map <edge,bool> that indicate which edge is a back edge
// Result :
// - Map assigning an equivalence class (CEClass) to each edge
//
// Remarks:
// - the code is largely inspired of SESE detection in Otawa
//   (https://sourcesup.renater.fr/projects/otawa/)
//   thanks to their authors!
// 
// ----------------------------------------------------------------
std::unordered_map<Edge *, CEClass *>
SESEAnalysis::assignClasses(Cfg *cfg, unordered_map<Node *, int> &dfs_num_list,
              unordered_map<Edge *, bool> &backedge) {
  std::vector<Edge *> added;
  std::vector<Edge *> removed;
  std::vector<Node *> nodes = cfg->GetAllNodes(); // All the nodes of the CFG

  Node *n;                        // current node of the loop
  std::vector<Node *> succ, pred; // the successors and predecesors of n
  std::vector<Edge *> outEdges,
      inEdges; // the outgoing  and incoming edges of n

  unordered_map<Node *, int> hi; // store the n.hi for all n
  int hi0, hi1, hi2;             // Intermediary variables for n.hi

  unordered_map<Node *, std::vector<Edge *> >
      bracketlist; // bracketlist of all the nodes

  std::unordered_map<Edge *, int> recent_size;
  std::unordered_map<Edge *, CEClass *> recent_class, ce_class;
  std::unordered_map<Edge *, bool> capping;
  vector<Edge *> blist; // tempory variable to store a bracketlist
  Edge *edge, *top_bracket;
  int curr_size;
  
  vector<Edge *> toDel;
  for (int i = nodes.size() - 1; i > -1; i--) {

    hi0 = nodes.size();
    hi1 = nodes.size();
    hi2 = nodes.size();

    // Find the node n such qs n.dfsnum = i (ie traverse nodes in reverse DFS ordering)
    n = NULL;
    for (long unsigned int j = 0; j < nodes.size(); j++) {
      if (dfs_num_list[nodes[j]] == i) {
        n = nodes[j];
        break;
      }
    }
    
    // Skip dead BBs if any (strangely, it occurs on Arm, one BB detected that is not really one, following return
    // node of function. Dead BBs have no DFS numbers, reducing the total number of nodes to scan
    if (n==NULL) {
      continue;
    }

    /* hi0 = min{t.dfsnum | (n,t) is a backedge} */
    outEdges = cfg->GetOutgoingEdges(n);
    inEdges = cfg->GetIncomingEdges(n);
    for (long unsigned int k = 0; k < outEdges.size(); k++) {
      if ((backedge[outEdges[k]]) &&
          (dfs_num_list[outEdges[k]->GetTarget()] < i)) {
        hi0 = min(hi0, dfs_num_list[outEdges[k]->GetTarget()]);
      }
    }
    for (long unsigned int k = 0; k < inEdges.size(); k++) {
      if ((backedge[inEdges[k]]) &&
          (dfs_num_list[inEdges[k]->GetSource()] < i)) {
        hi0 = min(hi0, dfs_num_list[inEdges[k]->GetSource()]);
      }
    }

    /* hi1 = min { c.hi | c is a child of n } */
    // The cfg is oriented but we want to work on the non-oriented version,
    // For that reason we consider successors and predecessors in the same way
    Node *hichild = nullptr;
    succ = cfg->GetSuccessors(n);
    pred = cfg->GetPredecessors(n);
    for (long unsigned int k = 0; k < succ.size(); k++) {
      if (!(backedge[cfg->FindEdge(n, succ[k])]) && (hi[succ[k]] < i) &&
          (dfs_num_list[succ[k]] > i) && (hi1 > hi[succ[k]])) {
        hi1 = hi[succ[k]];
        hichild = succ[k];
      }
    }
    for (long unsigned int k = 0; k < pred.size(); k++) {
      if (!(backedge[cfg->FindEdge(pred[k], n)]) && (hi[pred[k]] < i) &&
          (dfs_num_list[pred[k]] > i) && (hi1 > hi[pred[k]])) {
        hi1 = hi[pred[k]];
        hichild = pred[k];
      }
    }

    /*n.hi = min(hi0,hi1)*/
    hi[n] = min(hi0, hi1);
    
    if (hichild) {
      /* hi2 = min {c.hi | c is a child of n and c.hi != hi1} */
      for (long unsigned int k = 0; k < succ.size(); k++) {
        if ((hi[succ[k]] < i) && !(backedge[cfg->FindEdge(n, succ[k])]) &&
            (hi[succ[k]] < hi2) && (dfs_num_list[succ[k]] > i) &&
            (succ[k] != hichild)) {
          hi2 = hi[succ[k]];
        }
      }
      for (long unsigned int k = 0; k < pred.size(); k++) {
        if ((hi[pred[k]] < i) && !(backedge[cfg->FindEdge(pred[k], n)]) &&
            (hi[pred[k]] < hi2) && (dfs_num_list[pred[k]] > i) &&
            (pred[k] != hichild)) {
          hi2 = hi[pred[k]];
        }
      }
    }

    /* Compute the bracket-list for n */
    /* Merge the bracketlist of successors and predecessors*/
    for (long unsigned int k = 0; k < succ.size(); k++) {
      if ((!backedge[cfg->FindEdge(n, succ[k])]) // if (n,c) is a backedge
          && (dfs_num_list[succ[k]] > i)) {
        blist = bracketlist[succ[k]];
        for (long unsigned int j = 0; j < blist.size(); j++) {
          bracketlist[n].push_back(blist[j]);
        }
      }
    }
    for (long unsigned int k = 0; k < pred.size(); k++) {
      if ((!backedge[cfg->FindEdge(pred[k], n)]) // if (n,c) is a backedge
          && (dfs_num_list[pred[k]] > i)) {
        blist = bracketlist[pred[k]];
        for (long unsigned int j = 0; j < blist.size(); j++) {
          bracketlist[n].push_back(blist[j]);
        }
      }
    }

    /* Remove the edge that are from n to an ancestor of n
     * If the edge is capping, take it out of the cfg also */
    /* Remove the edge from a successor of n to n  */
    toDel.clear();

    for (unsigned int k = 0; k < bracketlist[n].size(); k++) {
      if ((bracketlist[n][k]->GetSource() == n) &&
          (dfs_num_list[bracketlist[n][k]->GetTarget()] > i)) {
        toDel.push_back(bracketlist[n][k]);
      } else if ((bracketlist[n][k]->GetTarget() == n) &&
                 (dfs_num_list[bracketlist[n][k]->GetSource()] > i)) {
        toDel.push_back(bracketlist[n][k]);
      }
    }

    for (unsigned int k = 0; k < toDel.size(); k++) {
      for (unsigned int j = 0; j < bracketlist[n].size(); j++) {
        if (bracketlist[n][j] == toDel[k]) {
          bracketlist[n].erase(bracketlist[n].begin() + j);
	}
      }
      if (capping[toDel[k]]) {
        capping[toDel[k]] = false;
        cfg->RemoveEdgeNoDelete(toDel[k]);
      }
    }

    for (long unsigned int k = 0; k < succ.size(); k++) {
      if (backedge[cfg->FindEdge(n, succ[k])] // if (n,c) is a backedge
          && dfs_num_list[succ[k]] < i) {
        bracketlist[n].push_back(cfg->FindEdge(n, succ[k]));
      }
    }

    for (long unsigned int k = 0; k < pred.size(); k++) {
      if (backedge[cfg->FindEdge(pred[k], n)] // if (n,c) is a backedge
          && dfs_num_list[pred[k]] < i) {
        bracketlist[n].push_back(cfg->FindEdge(pred[k], n));
      }
    }
    
    /* If hi2 < hi0 add the edge from n to the first node of the section  */
    if (hi2 < hi0) {
      for (long unsigned int k = 0; k < nodes.size(); k++) {
        if (dfs_num_list[nodes[k]] == hi2) {
          Edge *e = cfg->CreateNewEdge(n, nodes[k]);
	  backedge[e] = true;
          added.push_back(e);
          capping[e] = true;
          bracketlist[n].push_back(e);
        }
      }
    }
    
    /* Determine the class for edge from ancestor of n to n */
    if (i != 0) {
      edge = NULL;
      for (long unsigned int k = 0; k < succ.size(); k++) {
        if ((!backedge[cfg->FindEdge(n, succ[k])]) &&
            (dfs_num_list[succ[k]] < i)) {
          edge = cfg->FindEdge(n, succ[k]);
        }
      }
      for (long unsigned int k = 0; k < pred.size(); k++) {
        if ((!backedge[cfg->FindEdge(pred[k], n)]) &&
            (dfs_num_list[pred[k]] < i)) {
	  edge = cfg->FindEdge(pred[k], n);
        }
      }

      top_bracket = bracketlist[n][bracketlist[n].size() - 1];
      curr_size = bracketlist[n].size();

      if (recent_size[top_bracket] != curr_size) {
        recent_size[top_bracket] = curr_size;
        recent_class[top_bracket] = new CEClass();
      }

      ce_class[edge] = recent_class[top_bracket];
      
      ce_class[edge]->inc();

      if (recent_size[top_bracket] == 1) {
        ce_class[top_bracket] = ce_class[edge];
        if (top_bracket->GetSource() != cfg->GetEndNodes()[0] &&
            top_bracket->GetTarget() != cfg->GetStartNode()) {
          ce_class[top_bracket]->inc(top_bracket);
        }
      }
    }
  }

  /* Restore the CFG as it was */
  for (size_t i = 0; i < removed.size(); i++) {
    cfg->putEdge(removed[i]);
  }
  for (size_t i = 0; i < added.size(); i++) {
    cfg->RemoveEdgeNoDelete(added[i]);
  }

  return ce_class;
}

// ----------------------------------------------------------------
// buildTree
//
// Construction of region tree
//
// Parameters :
// - cfg:          Cfg under study
// - startNode:    start node for tree construction
// - classe:       map assigning an equivalence class (CEClass) to each edge
// - context:      ??
// - pst:          tree under construction
// - visited:      map indicating if a node was visited
//
// Remarks:
// - this function is recursive, called with Cfg entry point,
//   and then recursively on its non yet visited successors in the Cfg
//
// ----------------------------------------------------------------
void SESEAnalysis::buildTree(Cfg *cfg, Node *startNode,
               std::unordered_map<Edge *, CEClass *> classe,
               SESERegion *context, Tree *pst,
               std::unordered_map<Node *, bool> *visited) {

  std::vector<Node *> succ;
  CEClass *cl;
  SESERegion *newcontext;
  Edge *entry, *exit;
  bool first, last;

  succ = cfg->GetSuccessors(startNode);

  // Traverse all successors
  for (long unsigned int i = 0; i < succ.size(); i++) {
    
    newcontext = context;

    // Find CE class for edge startNode->succ and create corresponding
    // SESE region
    cl = classe[cfg->FindEdge(startNode, succ[i])];
    if (cl) {
      if ((!cl->isFirst() || !cl->isLast())) {
        if (cl->isFirst()) {	  
          entry = cfg->FindEdge(startNode, succ[i]);
          exit = nullptr;
          first = true;
          last = false;
          newcontext = new SESERegion(cfg, entry, exit, first, last, cl);
          pst->addNode(newcontext);
          pst->addEdge(context, newcontext);	  
        } else if (!cl->isLast()) {
          context->setExit(cfg->FindEdge(startNode, succ[i]));
          entry = cfg->FindEdge(startNode, succ[i]);
          exit = nullptr;
          first = true;
          last = false;
          newcontext = new SESERegion(cfg, entry, exit, first, last, cl);
          pst->addNode(newcontext);
          pst->addEdge(pst->getAncestor(context), newcontext);
        } else {
          context->setExit(cfg->FindEdge(startNode, succ[i]));
          newcontext = pst->getAncestor(context);
        }
        cl->dec();
      }
    }

    // Recursive call for tree construction on successors of startNode
    if (!(*visited)[succ[i]]) {
      (*visited)[succ[i]] = true;
      buildTree(cfg, succ[i], classe, newcontext, pst, visited);
    }
  }
}

// ----------------------------------------------------------------
// processCFG
//
// Compute SESE regions for a Cfg
//
// Parameters :
// - cfg:          Cfg under study
// Result :
// - calculated region tree
//
// ----------------------------------------------------------------
Tree *SESEAnalysis::processCFG(Cfg *cfg) {

  // Get end node of cfg (assumes there is only one node)
  vector<Node *> endNodes = cfg->GetEndNodes();
  assert(endNodes.size() == 1);

  // Fake edge to compute CEclasses (cf paper)
  Edge *fakeEdge;

  // DFS numbering of Cfg nodes + identification of backedges
  std::unordered_map<Node *, int> dfs_num_list;
  std::unordered_map<Edge *, bool> backedge;

  // Calculated CE classes
  std::unordered_map<Edge *, CEClass *> classe;

  // Root region and SESE tree
  SESERegion *rootRegion;
  Tree *pst;

  // Visited marker for nodes
  std::unordered_map<Node *, bool> visited;
  int counter = 0;

  // Vectore of node + vector of edges of the CFG
  vector<Node *> nodes;
  vector<Edge *> edges;


  // Step 0 of the algo. Add the fake edge
  // --------------------------------------
  fakeEdge = cfg->CreateNewEdge(cfg->GetEndNodes()[0], cfg->GetStartNode());
  
  // Step 1. calculate DFS order in Cfg + backedge information
  // ----------------------------------------------------------
  nodes = cfg->GetAllNodes();
  for (size_t i = 0; i < nodes.size(); i++) {
    dfs_num_list[nodes[i]] = DFS_NUM_DEFAULT;
  }
  edges = cfg->GetAllEdges();
  for (size_t k = 0; k < edges.size(); k++) {
    backedge[edges[k]] = true;
  }
  depthFirstTraversal(cfg, cfg->GetStartNode(), dfs_num_list, backedge, counter);

  // Step 2. Assign equivalence classes to edges
  // -------------------------------------------
  classe = assignClasses(cfg, dfs_num_list, backedge);

  // Remove fake edge
  // -----------------
  cfg->RemoveEdge(fakeEdge);

  // 3. Creation of root SESE region, region tree + filling of tree
  // --------------------------------------------------------------
  rootRegion = new SESERegion(cfg);
  rootRegion->setFirst(true);
  pst = new Tree();
  pst->addNode(rootRegion);
  visited[cfg->GetStartNode()] = true;
  buildTree(cfg, cfg->GetStartNode(), classe, rootRegion, pst, &visited);
  pst->fillRegion(cfg);
  
  return pst;
}

SESEAnalysis::SESEAnalysis(Program *p) : Analysis(p) {
  this->listCfg = p->GetAllCfgs();
}

// ----------------------------------------------------------------
// Checks if all required attributes are in the CFG
// Returns true if successful, false otherwise
// Here, we simply check that there is a cfg to print
// ----------------------------------------------------------------
bool SESEAnalysis::CheckInputAttributes() {
  Cfg *c = config->getEntryPoint();
  assert(c != NULL);
  assert(!(c->IsEmpty()));
  return true;
}

// ----------------------------------------------
// Performs the analysis
// Returns true if successful, false otherwise
// ----------------------------------------------
bool SESEAnalysis::PerformAnalysis() {
  Tree *pst;
  for (size_t i = 0; i < listCfg.size(); i++) {
    pst = processCFG(listCfg[i]);
    SESETree[listCfg[i]] = pst;
  }
  return true;
}

/* Remove all private attributes */
void SESEAnalysis::RemovePrivateAttributes() {}
