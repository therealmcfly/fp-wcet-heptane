# NOTE: this is a GNU Makefile.  You must use "gmake" rather than "make" !

all: simulator

# Archives used by the Nachos kernel: each .a may be repeated several
# times when undefined symbols are found
KERNEL_LIBS = simul/archive.a

TOPDIR=.
include $(TOPDIR)/Makefile.config

#
# Main Targets
#
simulator: $(KERNEL_LIBS)
	$(HOST_GXX) -o $@ $(KERNEL_LIBS) $(HOST_LDFLAGS)

user_tests: user_lib
	$(MAKE) -C test

user_lib:
	$(MAKE) -C userlib

showconfig:
	@echo Config=$(CFG).

#
# Dependencies
#
.PHONY: $(KERNEL_LIBS) # Pour forcer make a aller dans les subdirs

$(sort $(KERNEL_LIBS)):
	$(MAKE) -C $(dir $@) $(notdir $@)

#
# Useful targets
#
clean:
	$(RM) nachos *~ core DISK "Swap DISK" *.ps
	-NO_DEP=no_dep ; export NO_DEP ; \
	for d in simul  \
	  userlib  ; do \
	  [ -d $$d ] && \
	  $(MAKE) -C $$d clean ; \
	done

doxygen:
	$(DOXYGEN) doxygen.cfg
	cd latex; make; dvips -o -f refman.dvi > ../doc/refman.ps
	$(RM) -r latex man

src.ps:
	f=`echo Makefile* */Makefile` ; \
	for d in simul userlib test ; do \
	  [ -d $$d ] && \
	  f=`echo $$f $$d/*.h $$d/*.cc $$d/*.c $$d/*.s $$d/*.lds` ; \
	done ; \
	$(A2PS) -o $@ $$f

print: src.ps
	$(LPR) $<
