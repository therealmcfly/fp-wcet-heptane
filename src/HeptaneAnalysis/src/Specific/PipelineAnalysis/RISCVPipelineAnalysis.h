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
                             RISCVPipelineAnalysis
 This is the entry point of the pipeline analysis for RISCV architecture.

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

#ifndef RISCVPIPELINEANALYSIS_H
#define RISCVPIPELINEANALYSIS_H

class RISCVPipelineAnalysis:public MIPSPipelineAnalysis
{

 public:

  /** Constructor */
  RISCVPipelineAnalysis (Program * p, int nbcache);

  /** Performs the analysis
      @return true if successful, false otherwise.
  */
  bool PerformAnalysis ();

};
#endif
