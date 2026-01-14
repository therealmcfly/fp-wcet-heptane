/*! \file msgerror.cc
// \brief Data structure to store the last syscall error message.
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
*/

#include "msgerror.h"

//-----------------------------------------------------------------
// SyscallError::SyscallError
/*!      Constructor. Initialize the data structures
*/
//-----------------------------------------------------------------
SyscallError::SyscallError() {
  lastError = NoError;
  errorAbout = NULL;

  msgs = new char*[NUMMSGERROR];
  msgs[NoError] =  "no error %s \n";
  msgs[IncError] = "incorrect error type %s \n";

  msgs[OpenFileError] = "unable to open file %s \n";
  msgs[ExecFileFormatError]
    = "file %s is not a valid executable file (not in ELF format)\n";
  msgs[OutOfMemory] = "out of memory %s\n";

  msgs[OutOfDisk] = "out of disk space %s\n";
  msgs[AlreadyInDirectory] = "file or directory %s already exists\n";
  msgs[InexistFileError] = "file %s does not exist or is a directory\n";
  msgs[InexistDirectoryError] = "directory %s does not exist or is a file\n";
  msgs[NoSpaceInDirectory]
    = "maximum number of entries in directory %s reached\n";
  msgs[NotAFile] = "%s is not a file\n";
  msgs[NotADirectory] = "%s is not a directory\n";
  msgs[DirectoryNotEmpty] = "directory %s is not empty\n";

  msgs[InvalidSemaphoreId] = "invalid semaphore identifier %s\n";
  msgs[InvalidLockId] = "invalid lock identifier %s\n";
  msgs[InvalidConditionId] = "invalid condition identifier %s\n";
  msgs[InvalidFileId] = "invalid file identifier %s\n";
  msgs[InvalidThreadId] = "invalid thread identifier %s\n";

  msgs[NoACIA] = "no ACIA driver installed %s\n";
}


//-----------------------------------------------------------------
// SyscallError::~SyscallError
/*!      Destructor. De-allocate the structures
*/
//-----------------------------------------------------------------
SyscallError::~SyscallError() { 
  if (errorAbout != NULL) delete[] errorAbout;
  delete[] msgs;
}


//-----------------------------------------------------------------
// SyscallError::SetMsg
/*!      Set the current error message defined by its index and
//       the related context string.
//
//       \param about is the context string
//       \param num is the number associated with the error msg
*/
//-----------------------------------------------------------------
void SyscallError::SetMsg(char *about,int num) {

  // Delete old "about" string
  if (errorAbout != NULL) delete errorAbout;

  // Allocate a new one if the argument is not NULL
  if (about != NULL) {
    int size = strlen(about)+1;
    errorAbout = new char[size];
    strcpy(errorAbout,about);
  } else errorAbout = NULL;

  // Remember the error code of the last system call
  if ((num < 0) || (num >= NUMMSGERROR))
    lastError = IncError;
  if (msgs[num] == NULL)
    lastError = IncError;
  else
    lastError = num;
}


//-----------------------------------------------------------------
// SyscallError::GetFormat
/*! Get the error message corresponding to a given error number
//
// \param num error number
// \return error message
//       
*/
//-----------------------------------------------------------------
const char *SyscallError::GetFormat(int num)
{
  if ((num < 0) || (num >= NUMMSGERROR))
    num = IncError;
  if (msgs[num] == NULL)
    num = IncError;
  return msgs[num];
}
