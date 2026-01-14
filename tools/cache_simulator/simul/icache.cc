/*! \file icache.cc
// \brief Routines implementing an instruction cache
//
//  DO NOT CHANGE -- part of the machine emulation
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
*/

#include "config.h"
#include "icache.h"

//----------------------------------------------------------------------
// ICache::ICache(int lsize, int nbl, int nbw)
/*! Constructor. Creates an instruction cache. 
//      Its size, size of cache lines (unit of
//      transfer from memory) and associativity degree as taken as paremeters
//      of the constructor.	
//
//      //--DH old representation
//	\param lsize size of a cache block (or line) in bytes
//      \param nbl total number of cache lines
//      \param nbw associativity degree
//
//      New representation to have the same with static cache analysis
//      \param nbs : number of set
//      \param nbw : number of way (associativity degree)
//      \param lsize : size of a cache line
//
//----------------------------------------------------------------------*/
ICache::ICache(int nbs, int nbw,int lsize /*int lsize, int nbl, int nbw*/)
{
  //int nbsets,int nbways,
  //int cachelinesize,
  lineSize = lsize;
  nbLines = nbw * nbs ; /*nbl;*/
  nbWays = nbw;
  nbSets = nbs; /*nbl / nbw;*/
  lastUse = 0;
  lastBaseAddress = -1;
  cacheLines = new ICacheLine *[nbSets];
  for(int i = 0; i < nbSets; i++)
    {
      cacheLines[i] = new ICacheLine[nbWays];
      for(int j = 0; j < nbWays; j++)
	cacheLines[i][j].valid = 0;
    }
}

//----------------------------------------------------------------------
// ICache::~ICache()
/*! Deletes the instruction cache (frees its contents)
//
//----------------------------------------------------------------------*/
ICache::~ICache()
{
  delete cacheLines;
}


//----------------------------------------------------------------------
// int ICache::LoadLine(int physaddress)
/*! Load a given instruction, whose physical address is passed
//      as paremeter, in the instruction cache.
//      Does not actually load the code in the instruction cache,
//      just remember that it is loaded.
//
//	\param physaddress physical address of the instruction (not necesarily of a cache line boundary)
//      \return returns CACHE_HIT or CACHE_MISS depending on 
//              whether or not the instruction is already loaded in the icache.
//
//  Note that we simulate an instruction prefetch buffer those size equals
//  the one of a cache line. CACHE_HIT is returned if the instruction
//  is in the prefetch buffer, even if it is not loaded in the instruction
//  cache.
//----------------------------------------------------------------------
*/
int ICache::LoadLine(int physaddress)
{
  int lineNb = physaddress / lineSize;
  int setNb = lineNb % nbSets;
  int baseAddress = lineNb * lineSize;
  int j;

  // Check if the instruction is in the prefetch buffer
  // (if so, return directly without looking into the icache)
  //if(lastBaseAddress == baseAddress)
  //  {
  //    return CACHE_HIT;  /* Cache line loaded in the processor
  //			    (even when the icache is disabled) */
  //  }
  //else
  //  {
  //    lastBaseAddress = baseAddress;
  //  }


  // Then, we have to look into the cache
  // ------------------------------------
  /* is the line already in cache ? */
  for(j = 0; j < nbWays; j++)
    if(cacheLines[setNb][j].valid && (cacheLines[setNb][j].physaddress == baseAddress))
      {
	cacheLines[setNb][j].lastUse = lastUse++;
	// If the instruction is in the cache, return  CACHE_HIT
	return CACHE_HIT;
      }


  /* The instruction is not in the cache, we have to look for
     one line in the set to place the instruction */

  /* First, we look for an invalid line */
  for(j = 0; j < nbWays; j++) {

    // Find an invalid cache block
    if(!cacheLines[setNb][j].valid) {
      cacheLines[setNb][j].valid = 1;
      cacheLines[setNb][j].physaddress = baseAddress;
      cacheLines[setNb][j].lastUse = lastUse++;
      return CACHE_MISS;
    }
  }

  /* No invalid line found */
 
  /* No invalid line found, replace a valid line */
  // (LRU cache replacement policy) */
 
  /* LRU policy for replacement */
  unsigned long int minUse = cacheLines[setNb][0].lastUse;
  int j_min = 0;
  for(j = 1; j < nbWays; j++) {
    if(cacheLines[setNb][j].lastUse< minUse)
      {
	minUse = cacheLines[setNb][j].lastUse;
	j_min = j;
      }
  }
  cacheLines[setNb][j_min].physaddress = baseAddress;
  cacheLines[setNb][j_min].lastUse = lastUse++;
      
  return CACHE_MISS;

}

//----------------------------------------------------------------------
// ICache::Print(void)
/*! Display the cache contents
//
//	\param none
//----------------------------------------------------------------------
*/
void ICache::Print(void)
{
  printf("Cache contents and configuration\n");
  printf("--------------------------------\n");
  printf("  Line size : %d\n",lineSize);
  printf("  nbLines   : %d\n",nbLines);
  printf("  nbWays    : %d\n",nbWays);
 
  for(int i = 0; i < nbSets; i++) {
    printf("    Set %d : ",i);
    for(int j = 0; j < nbWays; j++) {
      printf("(%d,%d) ",cacheLines[i][j].physaddress,
	     cacheLines[i][j].valid);
    }
    printf("\n");
  }
}
