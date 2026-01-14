/*! \file  process.cc
//  \brief Routines to manage processes
*/

#include <assert.h>

#include "msgerror.h"
#include "process.h"

//----------------------------------------------------------------------
// Process::Process
/*! 	Constructor. Create the environment to run a user program
//      (address space, statistics, ...)
//
//	\param executable is the file containing the object code 
//             to load into memory
//      \param err: error code 0 if OK, -1 otherwise
*/
//----------------------------------------------------------------------
Process::Process(char *filename, int *err)
{
  *err = NoError;

  assert(filename!=NULL);
  DEBUG('t', (const char *)"Create named process %s\n", filename);

  // Open executable
  exec_file = fopen(filename,"r");
  if (exec_file == NULL) {
    // NB : don't delete the stat object, so that statistics can
    // be displayed after the end of the process
    *err = InexistFileError;
    return;
  }
 
  // Create the new address space associated with this file
  addrspace = new AddrSpace(exec_file, this, err);
  if (*err != NoError) {
    delete addrspace;
    // NB : don't delete the stat object, so that statistics can
    // be displayed after the end of the process
    return;
  }
  
}

//----------------------------------------------------------------------
// Process::~Process
//!   Destructor. De-alloate a process and all its components
//      (address space, ...)
//----------------------------------------------------------------------
Process::~Process()
{
    if (exec_file != NULL) {
      delete addrspace;
    } 
}
