/*! \file translationtable.h 
    \brief Data structures for managing address translation  

   DO NOT CHANGE -- part of the machine emulation
  
    Copyright (c) 1999-2000 INSA de Rennes.
    All rights reserved.  
    See copyright_insa.h for copyright notice and limitation 
    of liability and disclaimer of warranty provisions.
*/

#ifndef TTABLE_H
#define TTABLE_H

#include "utility.h"

// Forward definitions
class TranslationTable;
class PageTableEntry;

/*! \brief Defines the data structures used for address translation
// 
*/

class TranslationTable {
 public:
  TranslationTable();  
  ~TranslationTable();

  int getMaxNumPages(); //!< Get the maximum number of pages
                        //!< that can be translated by this translation table

  PageTableEntry *getPageTableEntry(int virtualPage);
                        //!< Get the page table entry 
                        //!< corresponding to a virtual address
  void setPageTableEntry(int virtualPage, PageTableEntry entry);
                        //!< Update the page table entry 
                        //!< corresponding to a virtual address 
                        //!< (all fields altogether)
 
  // Methods to get/set specific fields of a page table
  // entry corresponding to a particular virtual page
  void setPhysicalPage(int virtualPage, short int physicalPage);
  short int getPhysicalPage(int virtualPage);
  
  void setAddrDisk(int virtualPage, short int addrDisk);
  short int getAddrDisk(int virtualPage);

  void setBitIo(int virtualPage);
  void clearBitIo(int virtualPage);
  bool getBitIo(int virtualPage);
  
  void setBitValid(int virtualPage);
  void clearBitValid(int virtualPage);
  bool getBitValid(int virtualPage);
  
  void setBitSwap(int virtualPage);
  void clearBitSwap(int virtualPage);
  bool getBitSwap(int virtualPage);
    
  void setBitReadAllowed(int virtualPage);
  void clearBitReadAllowed(int virtualPage);
  bool getBitReadAllowed(int virtualPage);

  void setBitWriteAllowed(int virtualPage);
  void clearBitWriteAllowed(int virtualPage);
  bool getBitWriteAllowed(int virtualPage);

 private:

  // Maximum number of pages that can be translated
  int maxNumPages;

  // Set when a linear translation table is used
  PageTableEntry *pageTable;
};

/*! \class PageTableEntry 
// \brief Defines an entry in a simple translation table 
//
// Each entry defines a mapping from one virtual page to one physical page.
// In addition, there are some extra bits for access control (valid and 
// read-only) and some bits for usage information (use and dirty).
*/

class PageTableEntry {
  public:
  /*! By default, a new page table entry refers
    to a page not on disk neither in swap or
    physical mem: page is considered unmapped */
  PageTableEntry();
  
  /*! If this bit isn't set, then the page is not in physical
    memory. */
  bool valid;
  
  /*! Access rights to the page. If some of these flags are set, the
     user is allowed to perform the corresponding operations
     (read/write) over the whole page. If none of these flags is set,
     then the page is considered not available at all, and any access
     to the page leads to an AddressErrorException */
  bool readAllowed;  /*!< Allows program to read the page contents */
  bool writeAllowed; /*!< Allows program to modify the page contents */
  
  /*! The page number in real memory (relative to the
    start of "mainMemory"). Relevant when valid is true only ! */
  short int physicalPage;

  /*! If this bit is set, the page must be load from swap.
    If not, the page must be load from executable file.*/
  bool swap;

  /*! Depending on the 'swap' bit:
    - swap == true : location, in terms of <b>PAGES</b>, in the swap
    - swap == false: location, in terms of <b>BYTES</b>, from the beginning
      of the executable file, or -1 for anonymous mapping */
  int addrDisk;

  /*! This bit is set by the system every time the
    page is occupied in a input-output.  */
  bool io;
};

/*! \class BookTableEntry
// \brief Defines an entry in a top-level translation table
//        Used only if a two-level translation table is selected
*/
class PageTableBook {
public:
  PageTableBook(); //!< Create the associated page table
  ~PageTableBook(); //!< Destroy the pageTable

  PageTableEntry *pageTable; //!< Pointer to a PageTable  
};
 
#endif // TTABLE_H
