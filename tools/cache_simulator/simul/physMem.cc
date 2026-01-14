//-----------------------------------------------------------------
/*! \file mem.cc
//  \brief Routines for the physical page management
*/
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
//-----------------------------------------------------------------

#include <unistd.h>
#include <assert.h>
#include "physMem.h"
#include "process.h"

//-----------------------------------------------------------------
// PhysicalMemManager::PhysicalMemManager
//
/*! Constructor. It simply clears all the page flags and inserts them in the
// free_page_list to indicate that the physical pages are free
*/
//-----------------------------------------------------------------
PhysicalMemManager::PhysicalMemManager() {

  int i;

  tpr = new struct tpr_c[cfg->NumPhysPages];

  for (i=0;i<cfg->NumPhysPages;i++) {
    tpr[i].free=true;
    tpr[i].system=false;
    free_page_list.Append((void*)i);
  }
  i_clock=-1;
}

PhysicalMemManager::~PhysicalMemManager() {

  delete[] tpr;
}

//-----------------------------------------------------------------
// PhysicalMemManager::release_page
//
/*! This method releases an unused physical page by clearing the
//  corresponding bit in the page_flags bitmap structure, and adding
//  it in the free_page_list
//
//  \param num_page is the number of the real page to free
*/
//-----------------------------------------------------------------
void PhysicalMemManager::release_page(int num_page) {
  // Update statistics
  //currentThread->getProcessOwner()->stat->incrMemoryAccess();
  
  // Check that the page is not already free 
  ASSERT(! tpr[num_page].free);

  // Update the physical page table entry
  tpr[num_page].free=true;
  tpr[num_page].system=false;

  // Insert the page in the free list
  free_page_list.Prepend((void*)num_page);
}

//-----------------------------------------------------------------
// PhysicalMemManager::get_new_page
//
/*! This method returns a new physical page number. If there is no
//  page available, it applies the clock algorithm to determinate
//  the page among the physical ones to put in the swapping area.
//
//  \param owner translation table of the owner (for backlink)
//  \param virtualPage is the number of virtualPage to link with physical page
//  \return A new physical page number.
*/
//-----------------------------------------------------------------
int PhysicalMemManager::get_new_page(TranslationTable* owner,int virtualPage) 
{
  int page;

  // First try to find a free page
  page=find_free_page();  

  // If there is no free page, call the replacement algorithm
  if (page==-1) {page=clock_algorithm();}  // no free page : clock
  assert(page!=-1);

  // Update the physical page table
  tpr[page].virtualPage=virtualPage;
  tpr[page].owner=owner;
  tpr[page].use=true;
  tpr[page].dirty=false;
  DEBUG('a',(const char*)"Get_new_page, virtual : %i, real : %i\n",virtualPage,page);  
  return page;
}

//-----------------------------------------------------------------
// PhysicalMemManager::find_free_page
//
/*! This method returns a new physical page number, if it finds one
//  free. If not, return -1. Does not run the clock algorithm.
//
//  \return A new free physical page number.
*/
//-----------------------------------------------------------------
int PhysicalMemManager::find_free_page() {
  long page;

  // Check that the free list is not empty
  if (free_page_list.IsEmpty())
    return -1;

  // Update statistics
  //currentThread->getProcessOwner()->stat->incrMemoryAccess();
  
  // Get a page from the free list
  page = (long)free_page_list.Remove();
  
  // Check that the page is really free
  ASSERT(tpr[page].free);
  
  // Update the physical page table
  tpr[page].free = false;

  return page;
}

//-----------------------------------------------------------------
// PhysicalMemManager::clock_algorithm
//
/*! This method returns a new physical page number, the using clock
//  algorithm.
//
//  \param owner thread who is calling
//  \return A new free physical page number.
*/
//-----------------------------------------------------------------
int PhysicalMemManager::clock_algorithm() {
  return -1;
}



//-----------------------------------------------------------------
// PhysicalMemManager::dirtyPage
//
/*! update the dirty bit of a page  
//
//  \param rpage number of real page
*/
//-----------------------------------------------------------------

void PhysicalMemManager::dirtyPage(int rpage) {
  //currentThread->getProcessOwner()->stat->incrMemoryAccess();
  tpr[rpage].dirty=true;

}

//-----------------------------------------------------------------
// PhysicalMemManager::usedPage
//
/*! update the used bit of a page  
//
//  \param rpage number of real page
*/
//-----------------------------------------------------------------

void PhysicalMemManager::usedPage(int rpage) {
  // currentThread->getProcessOwner()->stat->incrMemoryAccess();
  tpr[rpage].use=true;
}


//-----------------------------------------------------------------
// PhysicalMemManager::Print
//
/*! print the current status of the table of physical pages
//
//  \param rpage number of real page
*/
//-----------------------------------------------------------------

void PhysicalMemManager::Print(void) {
  int i;

  printf("Contents of TPR (%d pages)\n",cfg->NumPhysPages);
  for (i=0;i<cfg->NumPhysPages;i++) {
    printf("Page %d free=%d system=%d virtpage=%d owner=%lx use=%d dirty=%d\n",
	   i,tpr[i].free,tpr[i].system,tpr[i].virtualPage,(long)tpr[i].owner,
	   tpr[i].use,tpr[i].dirty);
  }
}
