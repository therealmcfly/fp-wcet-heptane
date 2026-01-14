/*! \file config.h
    \brief Data structures for setting up the Nachos hardware and
	software configuration
  
    Copyright (c) 1999-2000 INSA de Rennes.
    All rights reserved.  
    See copyright_insa.h for copyright notice and limitation 
    of liability and disclaimer of warranty provisions.
*/
#ifndef CONFIG_H
#define CONFIG_H

#include "translationtable.h"

#define MAXSTRLEN 100

/*! \brief Defines Nachos hardware and software configuration */
class Config {

 public:

  int PageSize;            //!< the memory page size (do not modify)
  int NumPhysPages;        //!< the number of physical memory page
  int MaxVirtPages;        //!< the maximum number of virtual pages in each address space

  Config();
  ~Config();
};

#endif // CONFIG_H
