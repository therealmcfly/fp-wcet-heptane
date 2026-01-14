/*! \file  process.h
    \brief A process keeps track of the resources used by a running
           Nachos program
  
        The resources considered are the memory (addrspace.h), the
        processor (thread.h), the open files, the statistics, ...
*/

#ifndef PROCESS_H
#define PROCESS_H

#include "addrspace.h"

class AddrSpace;
class Thread;
class Semaphore;

/*! \brief Defines the data structures to keep track of the execution
 environment of a user program */
class Process {
public:
  /*!
   * Create an address space, initializing it with the program stored
   * in the file "executable", without any thread in it.
   */
  Process(char *filename, int *err);

  ~Process();	

  /*! Pointer on the executable file (may be NULL) */
  FILE *exec_file;
  
  AddrSpace * addrspace;              /*!< Pointer to the address space */


};

#endif // PROCESS_H
