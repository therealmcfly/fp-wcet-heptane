//-----------------------------------------------------------------
/*! \file mem.h 
    \brief Data structures for the physical page management
  
    Copyright (c) 1999-2000 INSA de Rennes.
    All rights reserved.  
    See copyright_insa.h for copyright notice and limitation 
    of liability and disclaimer of warranty provisions.
*/
//-----------------------------------------------------------------

#ifndef __MEM_H
#define __MEM_H

class PhysicalMemManager;

#include "machine.h"
#include "translationtable.h"
#include "list.h"

//-----------------------------------------------------------------
/*! \brief Implements the physical page management.

   This class manages the physical pages of the programs which run on the
   top of the Nachos kernel. It keeps track of which physical pages are used
   and which are free. 
   
   It processes a new page demand by applying the clock algorithm when 
   there is no page available. It requires an access to the thread list
   in order to choose which page will be swapped using the SwapManager
   class.
*/
//-----------------------------------------------------------------

class PhysicalMemManager {
public:
  PhysicalMemManager();   //!< initialize the memory manager
  ~PhysicalMemManager();  //!< de-allocate the page_flags bitmap

  int get_new_page(TranslationTable* owner,int virtualPage); //!< return the number of a new page
  void release_page(int numPage); //!< release the page which number is num_page
  void dirtyPage(int rpage); //!< Set the dirty bit of a page
  void usedPage(int rpage); //!< Set the used bit of a page       
  void Print(void); //!< Print the contents of a page
 
private:
  int find_free_page();                  // Return a free page if there is one
  int clock_algorithm();                 // Return a free page when there is none

  /*! \brief Describes the allocation of physical pages */
  struct tpr_c {
      bool free;  			//!< true if page is free
      bool system;              //!< true if page is reserved by the system
      int virtualPage;		//!< Number of virtualPage which reference this real page
      TranslationTable* owner;			//!< The owner !
      bool use;				//!< If page is readed or written                            	
      bool dirty;				//!< If page is modified
  }; 

  struct tpr_c *tpr;	//!< RealPAge Array to know the state of each real page

  Listint free_page_list; //!< List of available (unused) real page numbers

  int i_clock;          //!< Index for clock_algorithm
};

#endif // __MEM_H
