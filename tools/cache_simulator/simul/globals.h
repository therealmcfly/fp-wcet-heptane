#ifndef GLOB_H
#define GLOB_H

#include "config.h"
#include "physMem.h"
#include "machine.h"
#include "mmu.h"
#include "icache.h"
#include "stats.h"

#include <stdlib.h> 
#include <stdio.h>
#include <string.h>

extern Config *cfg;
extern PhysicalMemManager *physicalMemManager;
extern Machine *machine;	// user program memory and registers
extern MMU *mmu;                               //!< the memory management unit
extern ICache *icache;
extern ICache *icacheL2;
extern Statistics *stats;

#endif
