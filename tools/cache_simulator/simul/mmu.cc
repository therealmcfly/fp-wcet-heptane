/*! \file mmu.cc 
//  \brief Routines to translate virtual addresses to physical addresses
//
//	Software sets up a table of legal translations.  We look up
//	in the table on every memory reference to find the true physical
//	memory location.
//
// Two types of translation are supported here.
//
//	Linear page table -- the virtual page # is used as an index
//	into the table, to find the physical page #.
//
//	Translation lookaside buffer -- associative lookup in the table
//	to find an entry with the same virtual page #.  If found,
//	this entry is used for the translation.
//	If not, it traps to software with an exception. 
//
//	In practice, the TLB is much smaller than the amount of physical
//	memory (16 entries is common on a machine that has 1000's of
//	pages).  Thus, there must also be a backup translation scheme
//	(such as page tables), but the hardware doesn't need to know
//	anything at all about that.
//
*/
// DO NOT CHANGE -- part of the machine emulation
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "icache.h"
#include <assert.h>

#include "machine.h"
#include "addrspace.h"
#include "physMem.h"
#include "mmu.h"

//----------------------------------------------------------------------
// MMU::MMU()
/*! Construction. Empty for now  
*/
//----------------------------------------------------------------------
MMU::MMU() {
  translationTable = NULL;
}

//----------------------------------------------------------------------
// MMU::~MMU()
/*! Destructor. Empty for now
*/
//----------------------------------------------------------------------
MMU::~MMU() {
  translationTable = NULL;
}

//----------------------------------------------------------------------
// MMU::ReadMem
/*!     Read "size" (1, 2, 4) bytes of virtual memory at "addr" into 
//	the location pointed to by "value".
//
//
//	\param addr the virtual address to read from
//	\param size the number of bytes to read (1, 2, 4)
//	\param value the place to write the result
//      \return Returns false if the translation step from 
//              virtual to physical memory failed, true otherwise.
*/
//----------------------------------------------------------------------
bool
MMU::ReadMem(int virtAddr, int size, int *value, bool is_instruction)
{
    int data;
    ExceptionType exc;
    int physAddr;
  
    DEBUG('a', "Reading VA 0x%x, size %d\n", virtAddr, size);

    // Update statistics
    //currentThread->getProcessOwner()->stat->incrMemoryAccess();

    // Perform address translation
    exc = Translate(virtAddr, &physAddr, size, false);

    // Raise an exception if one has been detected during address translation
    if (exc != NoException) {
	machine->RaiseException(exc, virtAddr);
	return false;
    }
    
    // Look if the data is in the cache
    //int Machine::ReadIntRegister(int num)
    int virtualAddrOfCurrentInstruction = machine->ReadIntRegister(PCReg);
    if (is_instruction) {
      if (icache->LoadLine(physAddr)==CACHE_HIT){ stats->incrNbHits(virtAddr/*virtualAddrOfCurrentInstruction*/);}
      else {
	stats->incrNbMisses(virtAddr/*virtualAddrOfCurrentInstruction*/);
	if(icacheL2->LoadLine(physAddr)==CACHE_HIT){stats->incrNbHitsL2(virtAddr/*virtualAddrOfCurrentInstruction*/);}
	else{stats->incrNbMissesL2(virtAddr/*virtualAddrOfCurrentInstruction*/);}
      }
    }

    // Read data from main memory
    switch (size) {
      case 1:
	data = machine->mainMemory[physAddr];
	*value = data;
	break;
	
      case 2:
	data = *(unsigned short *) &machine->mainMemory[physAddr];
	*value = ShortToHost(data);
	break;
	
      case 4:
	data = *(unsigned int *) &machine->mainMemory[physAddr];
	*value = WordToHost(data);
	break;

    default: ASSERT(false);
    }

    DEBUG('a', "\tValue read = %8.8x\n", *value);

    return (true);
}

//----------------------------------------------------------------------
// MMU::WriteMem
/*!      Write "size" (1, 2, 4) bytes of the contents of "value" into
//	virtual memory at location "addr".
//
//   	Returns false if the translation step from virtual to physical memory
//   	failed.
//
//	\param addr the virtual address to write to
//	\param size the number of bytes to be written (1, 2, or 4)
//	\param value the data to be written
*/
//----------------------------------------------------------------------
bool
MMU::WriteMem(int addr, int size, int value)
{
    ExceptionType exc;
    int physicalAddress;
     
    DEBUG('a', "Writing VA 0x%x, size %d, value 0x%x\n", addr, size, value);

    // Update statistics
    //currentThread->getProcessOwner()->stat->incrMemoryAccess();

    // Perform address translation
    exc = Translate(addr, &physicalAddress, size, true);
    if (exc != NoException) {
	machine->RaiseException(exc, addr);
	return false;
    }

    // Write into the machine main memory
    switch (size) {
      case 1:
	machine->mainMemory[physicalAddress] = (unsigned char) (value & 0xff);
	break;

      case 2:
	*(unsigned short *) &machine->mainMemory[physicalAddress]
		= ShortToMachine((unsigned short) (value & 0xffff));
	break;
      
      case 4:
	*(unsigned int *) &machine->mainMemory[physicalAddress]
		= WordToMachine((unsigned int) value);
	break;
      default: ASSERT(false);
    }
      
    DEBUG('a', "\tValue written");

    return true;
}

//----------------------------------------------------------------------
// MMU::Translate(int virtAddr, int* physAddr, int size, bool writing)
/*! 	Translate a virtual address into a physical address, using 
//	either a page table or a TLB.
//         - check for alignment constraints
//         - Look for a translation of the virtual page in the TLB
//             - if found, check access rights and physical address
//                correctness, returns the physical page
//         - else Look for a translation of the virtual page in the
//           translation pages
//             - make sure the entry corresponds to a correct entry,
//               ie it maps something (phys mem or disk) <=> at least one
//               of the readAllowed or writeAllowed bits is set ?
//             - check access rights
//	       - If bit valid=true : physical page already known,
//               update the TLB
//	       - Else if bit valid=false : call PageFaultManager and update
//  	         the TLB
//             - returns the physical page
//
//      If everything is ok, set the use/dirty bits in
//	the translation table entry, and store the translated physical 
//	address in "physAddr".  If there was an error, returns the type
//	of the exception.
//
//	\param virtAddr the virtual address to translate
//	\param physAddr pointer to the place to store the physical address
//	\param size the amount of memory being read or written (1, 2
//	  or 4 Bytes)
// 	\param writing if true, check the writeAllowed bit
//      \return Number of the exception raised during address translation
*/
//----------------------------------------------------------------------
ExceptionType
MMU::Translate(int virtAddr, int* physAddr, int size, bool writing)
{
  DEBUG('a', "\tTranslate 0x%x, %s: \n",
	virtAddr, writing ? "write" : "read");
  
  // check for alignment errors
  if (((size == 4) && (virtAddr & 0x3))
      || ((size == 2) && (virtAddr & 0x1))){
    DEBUG('a', "alignment problem at %d, size %d!\n", virtAddr, size);
    return BusErrorException;
  }

  // Compute virtual page number and offset in the page
  int vpn = virtAddr / cfg->PageSize;
  int offset = virtAddr % cfg->PageSize;
  
  /*
   * Otherwise, use the translation table
   */

  // check the virtual page number
  if (vpn >= translationTable->getMaxNumPages()) {
    DEBUG('a', "virtual page # %d too large for page table size %d!\n",
	  vpn, translationTable->getMaxNumPages());
    return AddressErrorException;
  }

  // get the entry of the virtual page in the translation table
  assert(vpn>=0);
  PageTableEntry * entry = translationTable->getPageTableEntry(vpn);
  assert(entry!=NULL);
  
  // is the page correctly mapped ?
  if (!entry->readAllowed && !entry->writeAllowed) {
    DEBUG('a', "virtual page # %d not mapped !\n", vpn);
    return AddressErrorException;
  }

  // Check access rights
  if (writing && !entry->writeAllowed) {
    DEBUG('a', "write access on read-only virtual page # %d !\n",
	  vpn);
    return ReadOnlyException;
  }

  assert(entry->valid);

  // Make sure physical address is correct
  if ((entry->physicalPage < 0)
      || (entry->physicalPage >= cfg->NumPhysPages))
    {
      DEBUG('a', "MMU: Translated physical page out of bounds (0x%x)\n",
	    entry->physicalPage);
      return BusErrorException;
    }
  
  // signal that the page is used / modified
  if (writing)
    physicalMemManager->dirtyPage(entry->physicalPage);
  physicalMemManager->usedPage(entry->physicalPage);

  *physAddr = entry->physicalPage*cfg->PageSize + offset;
  DEBUG('a', "phys addr = 0x%x\n", *physAddr);
  return NoException;
}
