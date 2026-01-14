/*! \file  machine.h 
    \brief Data structures to simulate the MIPS machine
  
 User programs are loaded into "mainMemory"; to Nachos,
 this looks just like an array of bytes.  Of course, the Nachos
 kernel is in memory too -- but as in most machines these days,
 the kernel is loaded into a separate memory region from user
 programs, and accesses to kernel memory are not translated or paged.
  
 In Nachos, user programs are executed one instruction at a time, 
 by the MIPS simulator.  Each memory reference is translated, checked
 for errors, etc.

 DO NOT CHANGE -- part of the machine emulation
  
 Copyright (c) 1992-1993 The Regents of the University of California.
 All rights reserved.  See copyright.h for copyright notice and limitation 
 of liability and disclaimer of warranty provisions.
*/

#ifndef MACHINE_H
#define MACHINE_H

#include "utility.h"
#include "translationtable.h"

// Possible exceptions recognized by the machine
enum ExceptionType { NoException,           //!< Everything ok!
		     SyscallException,      //!< A program executed a system call.
		     PageFaultException,    //!< Page fault exception
		     ReadOnlyException,     /*!< Write attempted to
					         page marked "read-only" */
		     BusErrorException,     /*!< Translation resulted
					      in an invalid physical
					      address (mis-aligned or
					      out-of-bounds) */
		     AddressErrorException, /*!< Reference that was
					     not mapped in the address
					     space */
		     OverflowException,     //!< Integer overflow in add or sub.
		     IllegalInstrException, //!< Unimplemented or reserved instr.
		     
		     NumExceptionTypes
};

// User program CPU state.  The full set of MIPS registers, plus a few
// more because we need to be able to start/stop a user program between
// any two instructions (thus we need to keep track of things like load
// delay slots, etc.)

#define StackReg	29	//!< User's stack pointer
#define RetAddrReg	31	//!< Holds return address for procedure calls
#define NumGPRegs	32	//!< 32 general purpose registers on MIPS
#define HiReg		32	//!< Double register to hold multiply result
#define LoReg		33
#define PCReg		34	//!< Current program counter
#define NextPCReg	35	//!< Next program counter (for branch delay) 
#define PrevPCReg	36	//!< Previous program counter (for debugging)
#define LoadReg		37	//!< The register target of a delayed load.
#define LoadValueReg 	38	//!< The value to be loaded by a delayed load.
#define BadVAddrReg	39	//!< The failing virtual address on an exception

#define NumIntRegs 	40      //!< Number of integer registers
#define NumFPRegs       32      //!< Number of floating point registers

/*! \brief  Defines an instruction
//
//  Represented in both
// 	undecoded binary form
//      decoded to identify
//	    - operation to do
//	    - registers to act on
//	    - any immediate operand value
*/
class Instruction {
  public:
    void Decode();	//!< Decode the binary representation of the instruction

    unsigned int value; //!< Binary representation of the instruction

    int opCode;     /*!< Type of instruction.  This is NOT the same as the
		       opcode field from the instruction: see defs in mips.h
		     */
    char rs, rt, rd; //!< Three registers from instruction.
    char fs, ft, fd; //!< The same thing, but for FP operations
    int extra;       /*!< Immediate or target or shamt field or offset.
		       Immediates are sign-extended.
		     */
};

/*! \brief Defines the simulated execution hardware
// 
// User programs shouldn't be able to tell that they are running on our 
// simulator or on the real hardware, except 
//	- we only partially support floating point instructions (only
//	  "ordered operations", no FP "likely branches", no fixed point
//	  words
//	- the system call interface to Nachos is not the same as UNIX 
//	  (10 system calls in Nachos vs. 200 in UNIX!)
// If we were to implement more of the UNIX system calls, we ought to be
// able to run Nachos on top of Nachos!
//
// The procedures in this class are defined in machine.cc, mipssim.cc, and
// translate.cc.
*/

class Machine {
  public:
  Machine(bool debug);	//!<  Constructor. Initialize the MIPS machine
			//!<  for running user programs
  ~Machine();		//!<  Destructor. De-allocate the data structures

// Routines callable by the Nachos kernel
  void Run();	 		//!< Run a user program

  int ReadIntRegister(int num);	//!< Read the contents of an Integer CPU register

  void WriteIntRegister(int num, int value);
				//!< Store a value into an Integer CPU register

  int ReadFPRegister(int num); //!< Read the contents of a floating point register

  void WriteFPRegister(int num, int value);
				//!< store a value into a floating point register

  char ReadCC (void);           //!< Read floating point code condition register
  void WriteCC (char cc);       //!< Write floating point code condition register

// Routines internal to the machine simulation -- DO NOT call these 

    int OneInstruction(Instruction *instr); 	
    				//!< Run one instruction of a user program.
                                //!< Return the execution time of the instr (cycle)
    void DelayedLoad(int nextReg, int nextVal);  	
				//!< Do a pending delayed load (modifying a reg)

    void RaiseException(ExceptionType which, int badVAddr);
				//!< Trap to the Nachos kernel, because of a
				//!< system call or other exception.  

    void DumpState();		//!< Print the user CPU and memory state 


// Data structures -- all of these are accessible to Nachos kernel code.
// "public" for convenience.
//
// Note that *all* communication between the user program and the kernel 
// are in terms of these data structures.

    int int_registers[NumIntRegs]; //!< CPU Integer registers, for executing user programs

    int float_registers[NumFPRegs]; //!< Floating point general purpose registers

    char cc;                     /*!< Floating point condition code. Note that
				   since only MIPS I FP instrs are implemented,
				   we did not provide full support for the
				   floating point condition code registers */

    char *mainMemory;		/*!< Physical memory to store user program,
				  code and data, while executing
				*/

  private:
    bool singleStep;		/*!< Drop back into the debugger after each
				  simulated instruction
				*/
};


// Routines for converting Words and Short Words to and from the
// simulated machine's format of little endian.  If the host machine
// is little endian (DEC and Intel), these end up being NOPs.
//
// What is stored in each format:
//	- host byte ordering:
//	   - kernel data structures
//	   - user registers
//	- simulated machine byte ordering:
//	   - contents of main memory
unsigned int WordToHost(unsigned int word);
unsigned short ShortToHost(unsigned short shortword);
unsigned int WordToMachine(unsigned int word);
unsigned short ShortToMachine(unsigned short shortword);

#endif // MACHINE_H
