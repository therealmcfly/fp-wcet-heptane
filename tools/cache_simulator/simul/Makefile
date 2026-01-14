# NOTE: this is a GNU Makefile.  You must use "gmake" rather than "make".

OBJS = main.o process.o physMem.o icache.o 	\
       machine.o mipssim.o mmu.o translationtable.o config.o utility.o addrspace.o stats.o

archive.a: $(OBJS)

TOPDIR = ../
include $(TOPDIR)/Makefile.kernel
