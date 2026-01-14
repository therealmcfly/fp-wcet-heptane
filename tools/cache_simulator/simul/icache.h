/*! \file icache.h
   \brief Data structures for managing an instruction cache
  
    DO NOT CHANGE -- part of the machine emulation
  
    Copyright (c) 1999-2000 INSA de Rennes.
    All rights reserved.  
    See copyright_insa.h for copyright notice and limitation 
    of liability and disclaimer of warranty provisions.
*/

#ifndef ICACHE_H
#define ICACHE_H

#include <stdio.h>

#define ICACHE 0
#define DCACHE 1

#define CACHE_HIT 1
#define CACHE_MISS 0

typedef struct {
  int physaddress;
  int valid;
  unsigned long int lastUse;
} ICacheLine;

/*!
// \brief Defines a simple instruction cache
//
// The instruction cache is virtually addressed and physically tagged.
// Its size and associativity can be set (parameters to the constructor).
// The cache has a LRU replacement policy.
// It has a cache locking capability to freeze its contents with its
//    current contents
*/

class ICache {
 public:
  ICache(int lineSize, int nbLines, int nbWays);  /*!< Create a new empty instruction cache
						     with the specified dimensions */
  ~ICache();
  int LoadLine(int physaddress);   /*!< Load a cache line with instructions from given address
				   Return CACHE_HIT if there was a cache hit, CACHE_MISS if there was a miss. */
 
  void Print(void);               /*!< Display the cache contents */

 private:
  ICacheLine **cacheLines;       /*!< Cache contents */
  int nbLines;                  /*!< Number of cache lines */
  int lineSize;                 /*!< Line size in bytes */
  int nbWays;                   /*!< Set associativity of the cache (1 for direct-mapped) */
  int nbSets;                   /*!< Number of sets (= nbLines/nbWays) */
  int locked;                   /*!< if non-zero, allow replacement of any cache line */
  unsigned long int lastUse;    /*!< Used for the LRU cache replacement policy */
  int lastBaseAddress;          /*!< Last address accessed (simulation of the prefetch buffer) */


};

#endif // ICACHE_H

