/*! \file  addrspace.cc 
//  \brief Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//         1. Generate an ELF (Executable and Linkable Format) binary
//            using a MIPS cross-compiler (see how to do this in
//            test/Makefile)
//         2. Load the ELF file into the Nachos file system
//            (see documentation of configuration file nachos.cfg)
//         3. Execute it. You can do this
//            - when stating Nachos (see Nachos.cfg)
//            - by calling the appropriate system call in another
//              program (Exec)
//            - by typing the program name in the Nachos shell
//
*/
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
#include <cassert>

#include "elf32.h"
#include "addrspace.h"


#define LONG2HOST(var) var = WordToHost(var)
#define SHORT2HOST(var) var = ShortToHost(var)

// Forward references
static void CheckELFHeader(Elf32_Ehdr *ehdr,int *err);
static void SwapELFSectionHeader (Elf32_Shdr *shdr);

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
/*! 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//      Executables are in ELF (Executable and Linkable Format) (see elf32.h)
//      and can be generated using a standard MIPS cross-compiler
//      (here gcc).
//
//      For now, the code and data are entirely loaded into memory
//      and the stacks are preallocated. This will be changed in the
//      virtual memory assignment.
//
//      Don't look at this code right now. You may get lost. You will
//      have plenty of time to do so in the virtual memory assignment
//
//	\param exec_file is the file containing the object code 
//             to load into memory, or NULL when the address space
//             should be empty
//      \param err: error code 0 if OK, -1 otherwise
*/
//----------------------------------------------------------------------
AddrSpace::AddrSpace(FILE* exec_file, Process *p, int *err)
{
  Elf32_Ehdr elfHdr;      // Header du fichier exécutable

  *err  = 0;
  translationTable = NULL;
  freePageId = 0;
  process = p;

  /* Empty user address space requested ? */
  if (exec_file == NULL)
    {
      // Allocate translation table now
      translationTable = new TranslationTable();
      return;
    }

  // Read the header
  //exec_file->ReadAt((char *) &elfHdr, sizeof(elfHdr), 0);
  fseek(exec_file,0,SEEK_SET);
  fread((char *) &elfHdr,1,sizeof(elfHdr),exec_file);

  // Check the file format
  CheckELFHeader(&elfHdr,err);

  /* Retrived the contents of section table*/
  Elf32_Shdr * section_table = new Elf32_Shdr[elfHdr.e_shnum*sizeof(elfHdr)];
  //exec_file->ReadAt((char *) section_table, elfHdr.e_shnum*sizeof(elfHdr),
  //		    elfHdr.e_shoff);
  fseek(exec_file,elfHdr.e_shoff,SEEK_SET);
  fread((char *) section_table,1,elfHdr.e_shnum*sizeof(elfHdr),exec_file);

  /* Swap the section header */
  int i;
  for (i = 0 ; i < elfHdr.e_shnum ; i++)
    SwapELFSectionHeader(& section_table[i]);

  /* Retrieve the section containing section names */
  Elf32_Shdr * shname_section = & section_table[elfHdr.e_shstrndx];
  char *shnames = new char[shname_section->sh_size];
  //exec_file->ReadAt(shnames, shname_section->sh_size,
  //		    shname_section->sh_offset);
  fseek(exec_file,shname_section->sh_offset,SEEK_SET);
  fread(shnames,1,shname_section->sh_size,exec_file);

  

  // Create an empty translation table
  translationTable = new TranslationTable();

  // Compute the highest virtual address to init the translation table
  int mem_topaddr = 0;
  for (i = 0 ; i < elfHdr.e_shnum ; i++) {
      // Ignore empty sections
      if (section_table[i].sh_size <= 0)
	continue;
      int section_topaddr = section_table[i].sh_addr
	                    + section_table[i].sh_size;
      if ((section_table[i].sh_flags & SHF_ALLOC)
	  && (section_topaddr > mem_topaddr))
	mem_topaddr = section_topaddr;
    }

  // Allocate space in virtual memory
  int base_addr = this->Alloc(divRoundUp(mem_topaddr, cfg->PageSize));
  // Make sure this region really starts at virtual address 0
  ASSERT(base_addr == 0);

  DEBUG('l', "Allocated virtual area [0x0,0x%x[ for program\n",
	mem_topaddr);

  // Loading of all sections
  for (i = 0 ; i < elfHdr.e_shnum ; i++)
    {
      // Retrieve the section name
      const char *section_name = shnames + section_table[i].sh_name;

      DEBUG('l', "Section %d : size=0x%x name=\"%s\"\n",
	     i, section_table[i].sh_size, section_name);

      // Ignore empty sections
      if (section_table[i].sh_size <= 0)
	continue;

      // Look if this section has to be loaded (SHF_ALLOC flag)
      if (! (section_table[i].sh_flags & SHF_ALLOC))
	continue;

      printf("\t- Section %s : file offset 0x%x, size 0x%x, addr 0x%x, %s%s\n",
	     section_name,
	     (unsigned)section_table[i].sh_offset,
	     (unsigned)section_table[i].sh_size,
	     (unsigned)section_table[i].sh_addr,
	     (section_table[i].sh_flags & SHF_WRITE)?"R/W":"R",
	     (section_table[i].sh_flags & SHF_EXECINSTR)?"/X":"");

      // Make sure section is aligned on page boundary
      ASSERT((section_table[i].sh_addr % cfg->PageSize)==0);

      // Defaut contents of a translation table entry
      PageTableEntry entry;
      entry.swap = false;
      entry.readAllowed = true;
      entry.writeAllowed = (section_table[i].sh_flags & SHF_WRITE);
      entry.io = false;


      // Initializes the page table entries and loads the section
      // in memory (demand paging will be implemented later on)
      for (unsigned int pgdisk = 0,
	     pgmem = section_table[i].sh_addr / cfg->PageSize ;
	   pgdisk < divRoundUp(section_table[i].sh_size, cfg->PageSize) ;
	   pgdisk++, pgmem ++)
	{
	  /* Without demand paging */
	  
	  // Get a page in physical memory
	  entry.physicalPage = 
	    physicalMemManager->get_new_page(translationTable, pgmem);

	  // Check if the program fits in memory
	  if (entry.physicalPage == -1) {
	    printf("Not enough free space to load program\n");
	    exit(-1);
	  }
	  
	  // The SHT_NOBITS flag indicates if the section has an image
	  // in the executable file (text or data section) or not 
	  // (bss section)
	  if (section_table[i].sh_type != SHT_NOBITS) {
	    // The section has an image in the executable file
	    // Read it from the disk
	    //exec_file->ReadAt((char *)&(machine->mainMemory[entry.physicalPage*cfg->PageSize]),
	    //	      cfg->PageSize,
	    //		      section_table[i].sh_offset + pgdisk*cfg->PageSize);
	    fseek(exec_file,section_table[i].sh_offset + pgdisk*cfg->PageSize,SEEK_SET);
	    fread((char *)&(machine->mainMemory[entry.physicalPage*cfg->PageSize]),
		  1,cfg->PageSize,exec_file);

	  }
	  else {
	    // The section does not have an image in the executable
	    // Fill it with zeroes
	    memset(&(machine->mainMemory[entry.physicalPage*cfg->PageSize]),
		   0, cfg->PageSize);
	  }
	  
	  // The page has been loded in physical memory but
	  // later-on will be saved in the swap disk. We have to indicate this
	  // in the translation table
	  entry.addrDisk = -1;

	  // The entry is valid
	  entry.valid    = true;

	  // Set the translation table entry
	  translationTable->setPageTableEntry(pgmem, entry);
	  
	  /* End of code without demand paging */
	}
    }

///---------
//parsing of symbole table to find gp register value

  gp_value=0;
  Elf32_Shdr * section_symtab ;
  Elf32_Shdr * section_strtab ;
  bool foundSymtab = false;
  bool foundStrtab = false;
  bool foundGP = false;

  for(int currentSection = 0; currentSection < elfHdr.e_shnum ; currentSection++){
    const char *section_name = shnames + section_table[currentSection].sh_name;
    if(section_table[currentSection].sh_type == SHT_SYMTAB){
      assert(!foundSymtab);//FIXME if there is multiple symtab in an object file
      section_symtab =& section_table[currentSection];
      foundSymtab=true;
    }
    if(section_table[currentSection].sh_type == SHT_STRTAB && strcmp(section_name,".strtab")==0 ){
      assert(!foundStrtab);//FIXME if there is multiple strtab in an object file
      section_strtab =& section_table[currentSection];
      foundStrtab=true;
    }
  }

  assert(foundStrtab && foundSymtab);
  Elf32_Sym * symbol = new  Elf32_Sym[sizeof(Elf32_Sym)];//one entry of symtab

  for (unsigned int ii=0;ii<section_symtab->sh_size/section_symtab->sh_entsize;ii++){

    //get the current symtab entry
    int offset= section_symtab->sh_offset+ii*section_symtab->sh_entsize;
    fseek(exec_file,offset,SEEK_SET);
    fread((Elf32_Sym *) symbol,1,sizeof(Elf32_Sym),exec_file);
   
    //get the name of symbol
    int step=LONG2HOST(symbol->st_name);
    int offsetstr = section_strtab->sh_offset+step;
    char *ch =new char[30];
    fseek(exec_file,offsetstr,SEEK_SET);
    fread((char *) ch,30,sizeof(char),exec_file);

    if(strncmp(ch,"_gp",3)==0){ //if it's gp register get its value
      assert(!foundGP);
      gp_value = LONG2HOST(symbol->st_value);
      foundGP=true;
    }
  }
  if (!foundGP) {
    // FIXME, Isabelle
    // With the last version of mips cross-compiler, symbol gp is not in symbol table anymore
    // Transformed the former error message into a warning, since it seems to have no impact
    // on execution. 
    printf("**** Warning: initial value of GP not found in ELF file, setting GP to 0\n");
  }
  delete [] symbol;
  ///---------


  delete [] shnames;

  // Get program start address
  CodeStartAddress = (int32_t)elfHdr.e_entry;
  printf("\t- Program start address : 0x%x\n\n",
	 (unsigned)CodeStartAddress);

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//!   Destructor. Dealloate an address space.
//    Check if these addrSpace is linked to a process by checking
//    value of numThread
//----------------------------------------------------------------------
AddrSpace::~AddrSpace()
{
    int i;

    if (translationTable != NULL) {

      // For every virtual page
      for (i = 0 ; i <  freePageId ; i++) {

	// if it is in physical memory, free the physical page
	if (translationTable->getBitValid(i))
	  physicalMemManager->release_page(translationTable->getPhysicalPage(i));
      }
      delete translationTable;
    }
}

//----------------------------------------------------------------------
// AddrSpace::StackAllocate
/*!	Allocate a new stack of fixed size USERSTACKSIZE
//
//      \return stack pointer (at the end of the allocated stack)
*/
//----------------------------------------------------------------------
int
AddrSpace::StackAllocate(void)
{
  // Optional : leave an anmapped blank space below the stack to
  // detect stack overflows
  /*
#define STACK_BLANK_LEN 4 // in pages
  int blankaddr = this->Alloc(STACK_BLANK_LEN);
  DEBUG('l', "Allocated unmapped virtual area [0x%x,0x%x[ for stack overflow detection\n",
	blankaddr*cfg->PageSize, (blankaddr+STACK_BLANK_LEN)*cfg->PageSize);
  */

  // The new stack parameters
  int stackBasePage, numPages;
  numPages = divRoundUp(USERSTACKSIZE, cfg->PageSize);

  // Allocate virtual space for the new stack
  stackBasePage = this->Alloc(numPages);
  ASSERT (stackBasePage >= 0);
  DEBUG('l', "Allocated virtual area [0x%x,0x%x[ for stack\n",
	stackBasePage*cfg->PageSize,
	(stackBasePage+numPages)*cfg->PageSize);

  PageTableEntry entry; // page table entry used as a model to
                        // initialize the translation table for the stack
  entry.physicalPage = 0;
  entry.addrDisk = -1;
  entry.valid = false;
  entry.swap = false;
  entry.readAllowed = true;
  entry.writeAllowed = true;
  entry.io = false;
  for (int i = stackBasePage ; i < (stackBasePage + numPages) ; i++) {
      /* Without demand paging */

      // Allocate a new physical page for the stack
      entry.physicalPage
	= physicalMemManager->get_new_page(translationTable,i);
      if (entry.physicalPage == -1) {
	printf("Not enough free space to create new stack\n");
	exit(-1);
      }

      // Fill the page with zeroes
      memset(&(machine->mainMemory[entry.physicalPage*cfg->PageSize]),
	     0x0,cfg->PageSize);

      // Set the page table entry
      entry.valid = true;	
      translationTable->setPageTableEntry(i,entry);
      /* End of code without demand paging */
    }

  int stackpointer = (stackBasePage+numPages)*cfg->PageSize - 4*sizeof(int);
  return stackpointer;
}


//----------------------------------------------------------------------
// AddrSpace::SaveState
/*! 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
*/
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
/*! 	On a context switch, restore the machine state so that
//	this address space can run (translationTable)
*/
//----------------------------------------------------------------------
void AddrSpace::RestoreState() 
{
#ifndef DECOUPE
    mmu->translationTable = translationTable;
#endif // DECOUPE
#ifdef DECOUPE
    printf("*** Attention: fonction RestoreState non implementee\n");
    exit(-1);
#endif // DECOUPE
}


//----------------------------------------------------------------------
// AddrSpace::Alloc(int numPages)
/*!  Allocate the required number of continuous virtual pages in the
//   current address space
//    \param numPages the number of contiguous virtual pages to allocate
//    \return the virtual address of the beginning of the allocated
//      area, or -1 when not enough virtual space is available
*/
//----------------------------------------------------------------------
int AddrSpace::Alloc(int numPages) 
{
  int result = freePageId;

  DEBUG('l', "Virtual space alloc request for %d pages\n", numPages);

  // Check if the translation table is big enough for the allocation
  // to succeed
  if (freePageId + numPages >= translationTable->getMaxNumPages())
    return -1;

  // Do the allocation.
  // The allocation procedure is VERY SIMPLE. It just remembers
  // the number of the lastly allocated virtual page and increments it
  // when new pages are allocated. No de-allocation mechanisms is
  // implemented.
  freePageId += numPages;
  return result;
}

//----------------------------------------------------------------------
// SwapELFHeader
/*! 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//
*/
//----------------------------------------------------------------------
static void 
SwapELFHeader (Elf32_Ehdr *ehdr)
{
  SHORT2HOST(ehdr->e_type);
  SHORT2HOST(ehdr->e_machine);

  LONG2HOST(ehdr->e_version);
  LONG2HOST(ehdr->e_entry);
  LONG2HOST(ehdr->e_phoff);
  LONG2HOST(ehdr->e_shoff);
  LONG2HOST(ehdr->e_flags);

  SHORT2HOST(ehdr->e_ehsize);
  SHORT2HOST(ehdr->e_phentsize);
  SHORT2HOST(ehdr->e_phnum);
  SHORT2HOST(ehdr->e_shentsize);
  SHORT2HOST(ehdr->e_shnum);
  SHORT2HOST(ehdr->e_shstrndx);
}

//----------------------------------------------------------------------
// SwapELFSectionHeader
/*! 	Do little endian to big endian conversion on the bytes in the 
//	section header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//
*/
//----------------------------------------------------------------------
static void 
SwapELFSectionHeader (Elf32_Shdr *shdr)
{
  LONG2HOST(shdr->sh_name);
  LONG2HOST(shdr->sh_type);
  LONG2HOST(shdr->sh_flags);
  LONG2HOST(shdr->sh_addr);
  LONG2HOST(shdr->sh_offset);
  LONG2HOST(shdr->sh_size);
  LONG2HOST(shdr->sh_link);
  LONG2HOST(shdr->sh_info);
  LONG2HOST(shdr->sh_addralign);
  LONG2HOST(shdr->sh_entsize);
}

//----------------------------------------------------------------------
// CheckELFHeader
/*! 	Check the ELF header found in the executable file is correct
//
// \param shdr pointer to ELF header
// \param err error code (NoError if everything is correct)
*/
//----------------------------------------------------------------------
static void 
CheckELFHeader (Elf32_Ehdr *elfHdr, int *err)
{
  /* Make sure it is an ELF file by looking at its header (see elf32.h) */
  if (elfHdr->e_ident[EI_MAG0] != 0x7f ||
      elfHdr->e_ident[EI_MAG1] != 'E' ||
      elfHdr->e_ident[EI_MAG2] != 'L' ||
      elfHdr->e_ident[EI_MAG3] != 'F') {
      *err = ExecFileFormatError;
      return;
    }

  /* Make sure the ELF file type is correct */
  if (elfHdr->e_ident[EI_CLASS] != ELFCLASS32 ||
      elfHdr->e_ident[EI_DATA] != ELFDATA2MSB ||
      elfHdr->e_ident[EI_VERSION] != EV_CURRENT) {
      *err = ExecFileFormatError;
      return;
    }

  /* Transpose the header so that it can be interpreted by the kernel */
  SwapELFHeader(elfHdr);
      
  /* Make sure ELF binary code a MIPS executable */
  if (elfHdr->e_machine != EM_MIPS ||
      elfHdr->e_type != ET_EXEC) {
      *err = ExecFileFormatError;
      return;
  }

  /* Make sure ELF file internal structures are consistent with what
     we expect */
  if (elfHdr->e_ehsize != sizeof(Elf32_Ehdr) ||
      elfHdr->e_shentsize != sizeof(Elf32_Shdr))
    {
      *err = ExecFileFormatError;
      return;
    }

  /* Make sure ELF section table is available */
  if (elfHdr->e_shoff < sizeof(Elf32_Ehdr))
    {
      *err = ExecFileFormatError;
      return;
    }

  /* Make sure there is a string section name section */
  if (elfHdr->e_shstrndx >= elfHdr->e_shnum)
    {
      *err = ExecFileFormatError;
      return;
    }
}
