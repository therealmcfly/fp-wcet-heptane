/*! \file  addrspace.h 
    \brief Data structures to keep track of the memory resources
           of executing user programs (address spaces).

           Don't look at this code in the first assignment.

    Copyright (c) 1992-1993 The Regents of the University of California.
    All rights reserved.  See copyright.h for copyright notice and limitation 
    of liability and disclaimer of warranty provisions.
*/

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "globals.h"
#include "msgerror.h"

// Forward references
class Process;

#define USERSTACKSIZE 8*1024 	//!< Size of thread size (increase it if required)

/*! \brief Defines the data structures to keep track of  memory resources
        of executing user programs (address spaces).
 */
class AddrSpace {

 private:
  unsigned long gp_value;

public:

  unsigned long getGP_value(){return gp_value;}


  AddrSpace(FILE *exec_file, Process *p, int * err);
               //!< Constructor. Create an address space,
               //!< initializing it with the program
               //!< stored in the file "executable"

  ~AddrSpace();	
	       //!< De-allocate an address space

  int StackAllocate();                  
               //!< Allocate a new stack (will be changed
               //!< in the virtual memory assignment)

  void SaveState();			
               //!< Save address space-specific
               //!< info on a context switch: to be filled-in in the
               //!< virtual memory assignment
  void RestoreState();		
               //!< Restore address space-specific
               //!< info on a context switch: to be filled-in in the
               //!< virtual memory assignment

  //! Address of the default first instruction to execute
  int32_t getCodeStartAddress()
  { return CodeStartAddress; }

  TranslationTable *translationTable;   /*!< page table translation */

private:
  int32_t CodeStartAddress; //!< Code start address

  int Alloc(int numPages); /*!< allocate numPages virtual pages in the
                                current address space */

  int freePageId; /*!< Number of the next virtual page allocated.
                    Virtual addresses are simply allocated : an
		    allocation will simply increment this address by
		    the size of the allocated object */

  /* The corresponding process (for debug only) */
  Process *process;

};

#endif // ADDRSPACE_H


