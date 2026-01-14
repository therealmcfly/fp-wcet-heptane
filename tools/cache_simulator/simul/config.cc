/*! \file config.cc
    \brief Routines for setting up the Nachos hardware and software confuguration
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
*/
#ifndef CONFIG_CPP
#define CONFIG_CPP

#include <stdio.h>
#include "config.h"
#include "utility.h"
#include "globals.h"

#define LINE_LENGTH     256
#define COMMAND_LENGTH  80

#define power_of_two(size) (((size) & ((size)-1)) == 0)

void
fail(int numligne,char *name,char *ligne)
{
  ligne[strlen(ligne)-1] = '\0';
  printf("Config Error : File %s line %d ---> \"%s\"\n",name,numligne,ligne);
  exit(-1);
}

Config::Config() {
  PageSize=/*1024*/8192;
  NumPhysPages=4096;
  MaxVirtPages=10000;
}
  
Config::~Config() {
}

#endif // CONFIG_CPP
