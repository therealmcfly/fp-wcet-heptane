/*! \file  main.cc 
//  \brief Bootstrap code to initialize the operating system kernel.
//
// Usage: nachos -d <debugflags> -rs <random seed #>
//		-s -x <nachos file>
//              -z -f <configfile> 
//
//    -d causes certain debugging messages to be printed (cf. utility.h)
//    -s causes user programs to be executed in single-step mode
//    -z prints the copyright message
//    -f <configfile> gives the name of a configuration file for Nachos
//    -x runs a user program
//
//  NOTE -- flags are ignored until the relevant assignment.
*/
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include <assert.h>
#include <iostream>

#include "utility.h"
#include "msgerror.h"
#include "config.h"
#include "mmu.h"
#include "physMem.h"
#include "process.h"

using namespace std;

/*!  This defines *all* of the global data structures used by Nachos.
// These are all initialized and de-allocated by this file.
*/
Config *cfg;
Listint *Alive;                         //!< list of existing thread
Listint *UserObj;                       //!< list of existing objects (semaphore, ...)
MMU *mmu;                               //!< the memory management unit
ICache *icache;
ICache *icacheL2;
PhysicalMemManager *physicalMemManager; //!< the real memory manager

Machine *machine;	// user program memory and registers

Statistics *stats;

//----------------------------------------------------------------------
// main
/*! 	Bootstrap the operating system kernel.  
//	
//	Check command line arguments
//	Initialize data structures
//	(optionally) Call test procedure
//
//	\param argc is the number of command line arguments (including the name
//		of the command) -- ex: "nachos -d +" -> argc = 3 
//	\param argv is an array of strings, one for each command line argument
//		ex: "nachos -d +" -> argv = {"nachos", "-d", "+"}
*/
//----------------------------------------------------------------------

int
main(int argc, char **argv)
{

  int err;

  if (argc!=9) {
    printf("Usage: %s elf_file <nb L1 set> <nb L1 way> <L1 line size> <nb L2 set> <nb L2 way> <L2 line size>  <exit stat file> \n",argv[0]);
    exit(-1);
  }

  int nbL1set = atoi(argv[2]);
  int nbL1way = atoi(argv[3]);
  int nbL1linesize = atoi(argv[4]);

  int nbL2set = atoi(argv[5]);
  int nbL2way = atoi(argv[6]);
  int nbL2linesize = atoi(argv[7]);

  //add exit stat file
  string exitStatFile = string(argv[8]);

  // Create machine
  cfg = new Config(); // No file, take default values
  mmu = new MMU();
  physicalMemManager = new PhysicalMemManager();
  machine = new Machine(false);//true for debug
  stats = new Statistics();
  icache = new ICache(nbL1set,nbL1way,nbL1linesize);
  icacheL2 = new ICache(nbL2set,nbL2way,nbL2linesize);
  // Set the debug flags you want to see (original Nachos flags)
  DebugInit((char *)"");//lma

  // Launch program
  Process *p = new Process(argv[1],&err);
  assert(err==NoError);
  int userStackPointer = p->addrspace->StackAllocate();
  if (userStackPointer == -1) {
    printf("Error: unable to allocate stack\n");
    exit(-1);
  }
  int PC = (int) p->addrspace->getCodeStartAddress();
  machine->WriteIntRegister(PCReg,PC);
  machine->WriteIntRegister(NextPCReg,PC+4);
  machine->WriteIntRegister(StackReg,userStackPointer);
  mmu->translationTable=p->addrspace->translationTable;

  //initialisation of $ra register to stop the machine at the end of the main with NextPCReg
  // (cf machine::Run in mipssim.cc) rq: old section sys of nachos
  machine->WriteIntRegister(31,0x4008);
  //initialisation of gp
  machine->WriteIntRegister(28,p->addrspace->getGP_value());

  printf("Register contents OK!\n");

  machine->Run();

  printf("Finished !\n");
  stats->Print(exitStatFile);
}











