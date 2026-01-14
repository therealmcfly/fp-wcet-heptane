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
#include "SESERegion.h"
#include "Cfg.h"
#include "Handle.h"
#include "Helper.h"
#include "Node.h"
#include "SESEAnalysis.h"
#include <cassert>
#include <string>
#include <vector>

// Global variables for flat vector of SESE regions and tree of SESE regions
// ----------------------------------------------------------------------
vector<SESERegion *> allSESERegions;
vector<Tree *> allPST;

// ----------------------------------------------------------------
// dump
//
// Dump of a SESE region (for debug)
//
// ----------------------------------------------------------------
void SESERegion::dump() {
  t_address add_source;
  t_address add_target;
  t_address add;
  cout << "SESE region of id " << dec << id << ", Cfg " << c->getStringName() << endl;
  if (entry) {
    add_source = AnalysisHelper::getStartAddress(this->entry->GetSource());
    add_target = AnalysisHelper::getStartAddress(this->entry->GetTarget());
    
    cout << "        entry : @0x" << hex << add_source << " --> @0x" << hex
	 << add_target << endl;
  } else
    cout << "        entry : None " << endl;
  
  if (exit) {
    add_source = AnalysisHelper::getStartAddress(this->exit->GetSource());
    add_target = AnalysisHelper::getStartAddress(this->exit->GetTarget());
    
    cout << "        exit : @0x" << hex << add_source << " --> @0x" << hex
	 << add_target << endl;
  } else
    cout << "        exit : None " << endl;
  
  cout << "        Nodes in the region are : " << endl;
  for (long unsigned int i = 0; i < nodes.size(); i++) {
    add = AnalysisHelper::getStartAddress(nodes[i]);
    cout << "         @0x" << hex << add << endl;
  }
}

// ----------------------------------------------------------------
// IsNestedIn
//
// Region nesting test. Returns true if all of in l are
// nested in the region.
//
// ----------------------------------------------------------------
bool SESERegion::IsNestedIn(SESERegion *l) {

  if (l == NULL) {
    return true;
  }

  if (l == nullptr) {
    return false;
  }

  vector<cfglib::Node *> vnl = l->GetAllNodes();
  for (unsigned int i = 0; i < this->nodes.size(); i++) {
    bool found = false;
    for (unsigned int il = 0; il < vnl.size(); il++) {
      if ((this->nodes)[i] == vnl[il]) {
        found = true;
        break;
      }
    }
    if (!found)
      return false;
  }
  return true;
}

// ----------------------------------------------------------------
// WriteXml
//
// Dump region to xml (serialize)
//
// ----------------------------------------------------------------
std::ostream &SESERegion::WriteXml(std::ostream &os, cfglib::Handle &hand) {
  cout << "************* dumping to xml " << endl;
  os << "<SESEREGION "
     << " id=\"" << hand.identify((Serialisable *)this) << "\" ";
  os << " entry=\"" << hand.identify((Serialisable *)(this->entry)) << "\" ";
  os << " exit=\"" << hand.identify((Serialisable *)(this->exit)) << "\" ";
  os << " nodes=\"";
  // List of nodes
  for (std::vector<cfglib::Node *>::const_iterator it(this->nodes.begin());
       it != this->nodes.end(); ++it) {
    os << hand.identify((Serialisable *)*it);
    os << ", ";
  }
  os << "\" ";
  os << ">" << std::endl;

  this->WriteXmlAttributes(os, hand);

  os << "</SESEREGION>" << std::endl;
  return os;
}

// ----------------------------------------------------------------
// Readxml
//
// Deserialisation function.
//
// The deserialisation is in two phase : first the creation of the
// object (in several ways depending on the object) and second
// call to O->ReadXml() which initialize the object with correct
// values.
// ----------------------------------------------------------------
void SESERegion::ReadXml(XmlTag const *tag, cfglib::Handle &hand) {
  assert(tag->getName() == string("SESEREGION"));
  string id = tag->getAttributeString("id");
  assert(id != "");
  hand.addID_serialisable(id, this);

  // Read loop entry and exit
  string entry = tag->getAttributeString("entry");
  assert(entry != "");
  hand.addID_handle(entry, (Serialisable **)&(this->entry));

  string exit = tag->getAttributeString("exit");
  assert(exit != "");
  hand.addID_handle(exit, (Serialisable **)&(this->exit));

  // Read loop nodes
  string attr_nodes = tag->getAttributeString("nodes");
  assert(attr_nodes != "");
  std::vector<std::string> nodes_vector(
      cfglib::helper::split_string(attr_nodes, ", "));
  this->nodes.resize(nodes_vector.size());
  for (unsigned int i = 0; i < nodes_vector.size(); i++) {
    (this->nodes)[i] = NULL;
    hand.addID_handle(nodes_vector[i], (Serialisable **)&(this->nodes[i]));
  }

  // the only Loop child element should be `ATTRS_LIST`
  ListXmlTag children = tag->getAllChildren();
  if (children.size() != 0) {
    assert(children.size() == 1);
    assert(children[0].getName() == string("ATTRS_LIST"));
    this->ReadXmlAttributes(tag, hand);
  }
}
