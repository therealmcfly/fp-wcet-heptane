/*! \file translationtable..cc
// \brief Data structures for address translation
//
// DO NOT CHANGE -- part of the machine emulation
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
//
*/

#include "config.h"
#include "translationtable.h"
#include "globals.h"

//----------------------------------------------------------------------
// TranslationTable::TranslationTable
/*!  Constructor. Create the memory management structures
//   \param _mode : the type of the memory management structures
//   - Single Level : use simple table page
//   - Dual Level : use book table associated with table pages
*/
//----------------------------------------------------------------------
TranslationTable::TranslationTable() {

  // Init private fields
  maxNumPages = cfg->MaxVirtPages;
 
  pageTable = new PageTableEntry[maxNumPages];

  DEBUG('p',"One-level translation table created with at most %d pages\n",
	maxNumPages);

}

//----------------------------------------------------------------------
// TranslationTable::~TranslationTable()
/*!  Destructor. De-allocate the memory management structures
*/
//----------------------------------------------------------------------
TranslationTable::~TranslationTable() {
 
  delete [] pageTable;
  
  maxNumPages = -1;
}

//----------------------------------------------------------------------
// TranslationTable::getMaxNumPages()
/*! Get the number of pages that can be translated using the
//  translation table.
*/
//----------------------------------------------------------------------
int TranslationTable::getMaxNumPages() {
  return maxNumPages;
}


//----------------------------------------------------------------------
//  TranslationTable::getPageTableEntry
/*!  Get the entry of a virtual page
//   \param virtualPage : the virtual page (supposed to be between 0 and the
//     size of the address space)
//   \return a pointer to the entry
*/
//----------------------------------------------------------------------
PageTableEntry *TranslationTable::getPageTableEntry(int virtualPage) {

  ASSERT ((virtualPage >= 0) && (virtualPage < maxNumPages));

  return &pageTable[virtualPage];
}


//----------------------------------------------------------------------
//  TranslationTable::setPageTableEntry
/*!  Set the entry of a virtual page
//   \param virtualPage : the virtual page
//   \param entry : the entry
*/
//----------------------------------------------------------------------
void TranslationTable::setPageTableEntry(int virtualPage, PageTableEntry model) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  *e = model;
}


//----------------------------------------------------------------------
// TranslationTable::setPhysicalPage
/*!  Set the physical page of a virtual page
//   \param virtualPage : the virtual page
//   \param physicalPage : the physical page
*/
//----------------------------------------------------------------------
void TranslationTable::setPhysicalPage(int virtualPage,short int physicalPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  e->physicalPage = physicalPage;
}

//----------------------------------------------------------------------
//  TranslationTable::getPhysicalPage
/*!  Get the physical page of a virtual page
//   \param virtualPage : the virtual page
//   \return the physical page
*/
//----------------------------------------------------------------------
short int TranslationTable::getPhysicalPage(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  return e->physicalPage;
}

//----------------------------------------------------------------------
//  TranslationTable::setAddrDisk
/*!  Set the disk address of a virtual page
//   \param virtualPage : the virtual page
//   \param addrDisk : the address on disk (page number in swap, bytes in
//                     executable file)
*/
//----------------------------------------------------------------------
void TranslationTable::setAddrDisk(int virtualPage,short int addrDisk) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  e->addrDisk = addrDisk;
}

//----------------------------------------------------------------------
//  TranslationTable::getAddrDisk
/*!  Get the disk address of a virtual page
//   \param virtualPage : the virtual page
//   \return the disk address (page number in swap, or bytes in exec file)
*/
//----------------------------------------------------------------------
short int TranslationTable::getAddrDisk(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  return e->addrDisk;
}

//----------------------------------------------------------------------
//  TranslationTable::setBitValid
/*!  Set the bit valid of a virtual page
//   \param virtualPage : the virtual page
*/
//----------------------------------------------------------------------
void TranslationTable::setBitValid(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  e->valid = true;
}

//----------------------------------------------------------------------
//  TranslationTable::clearBitValid
/*!  Clear the bit valid of a virtual page
//   \param virtualPage : the virtual page
*/
//----------------------------------------------------------------------
void TranslationTable::clearBitValid(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  e->valid = false;
}

//----------------------------------------------------------------------
//   TranslationTable::getBitValid
/*!  Get the bit valid of a virtual page
//   \param virtualPage : the virtual page
//   \return value of the bit valid
*/
//----------------------------------------------------------------------
bool TranslationTable::getBitValid(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  return e->valid;
}

//----------------------------------------------------------------------
//  TranslationTable::setBitIo
/*!  Set the bit io of a virtual page
//   \param virtualPage : the virtual page
*/
//----------------------------------------------------------------------
void TranslationTable::setBitIo(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  e->io = true;
}

//----------------------------------------------------------------------
//  TranslationTable::clearBitIo
/*!  Clear the bit io of a virtual page
//   \param virtualPage : the virtual page
*/
//----------------------------------------------------------------------
void TranslationTable::clearBitIo(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  e->io = false;
}

//----------------------------------------------------------------------
//   TranslationTable::getBitIo
/*!  Get the bit io of a virtual page
//   \param virtualPage : the virtual page
//   \return value of the bit io
*/
//----------------------------------------------------------------------
bool TranslationTable::getBitIo(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  return e->io;
}

//----------------------------------------------------------------------
//  TranslationTable::setBitSwap
/*!  Set the bit swap of a virtual page
//   \param virtualPage : the virtual page
*/
//----------------------------------------------------------------------
void TranslationTable::setBitSwap(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  e->swap = true;
}

//----------------------------------------------------------------------
//  TranslationTable::clearBitSwap
/*!  Clear the bit swap of a virtual page
//   \param virtualPage : the virtual page
*/
//----------------------------------------------------------------------
void TranslationTable::clearBitSwap(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  e->swap = false;
}

//----------------------------------------------------------------------
//   TranslationTable::getBitSwap
/*!  Get the bit swap of a virtual page
//   \param virtualPage : the virtual page
//   \return value of the bit swap
*/
//----------------------------------------------------------------------
bool TranslationTable::getBitSwap(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  return e->swap;
}

//----------------------------------------------------------------------
//  TranslationTable::setBitReadAllowed
/*!  Set the bit readAllowed of a virtual page
//   \param virtualPage : the virtual page
*/
//----------------------------------------------------------------------
void TranslationTable::setBitReadAllowed(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  e->readAllowed = true;
}

//----------------------------------------------------------------------
//  TranslationTable::clearBitReadAllowed
/*!  Clear the bit readAllowed of a virtual page
//   \param virtualPage : the virtual page
*/
//----------------------------------------------------------------------
void TranslationTable::clearBitReadAllowed(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  e->readAllowed = false;
}

//----------------------------------------------------------------------
//   TranslationTable::getBitReadAllowed
/*!  Get the bit readAllowed of a virtual page
//   \param virtualPage : the virtual page
//   \return value of the bit readAllowed
*/
//----------------------------------------------------------------------
bool TranslationTable::getBitReadAllowed(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  return e->readAllowed;
}


//----------------------------------------------------------------------
//  TranslationTable::setBitWriteAllowed
/*!  Set the bit writeAllowed of a virtual page
//   \param virtualPage : the virtual page
*/
//----------------------------------------------------------------------
void TranslationTable::setBitWriteAllowed(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  e->writeAllowed = true;
}

//----------------------------------------------------------------------
//  TranslationTable::clearBitWriteAllowed
/*!  Clear the bit writeAllowed of a virtual page
//   \param virtualPage : the virtual page
*/
//----------------------------------------------------------------------
void TranslationTable::clearBitWriteAllowed(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  e->writeAllowed = false;
}

//----------------------------------------------------------------------
//   TranslationTable::getBitWriteAllowed
/*!  Get the bit writeAllowed of a virtual page
//   \param virtualPage : the virtual page
//   \return value of the bit writeAllowed
*/
//----------------------------------------------------------------------
bool TranslationTable::getBitWriteAllowed(int virtualPage) {
  PageTableEntry * e = getPageTableEntry(virtualPage);
  return e->writeAllowed;
}

//----------------------------------------------------------------------
//   PageTableEntry::PageTableEntry
/*!  Constructor. Defaut initialization of a page table entry
*/
//----------------------------------------------------------------------
PageTableEntry::PageTableEntry()
{
  valid=false;
  swap=false;
  addrDisk = -1;
  readAllowed=false;
  writeAllowed=false;
}

