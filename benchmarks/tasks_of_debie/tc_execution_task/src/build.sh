#!/bin/sh
#
# Script to compile debie1 on the host Intel/Linux system.
#
# Any arguments go into the CCOPT (eg. -Wpadded).


# Native gcc and options:

export CC="gcc"
export CCOPT="-g -I. -wAll -DMIPS $*"
export LD="gcc"
export LDOPT=

EXPR='s/^.*asm.*loopbound.*$//'


for file in class classtab debie harness health hw_if measure target tc_hand telem
do
    echo $file
    sed -e ${EXPR} ${file}.c > ${file}_sed.c
    ${CC} ${CCOPT} -c ${file}_sed.c -o ${file}.o
done
    
${CC} ${LDOPT}           \
    -o debie1            \
    class.o              \
    classtab.o           \
    debie.o              \
    harness.o            \
    health.o             \
    hw_if.o              \
    measure.o            \
    target.o             \
    tc_hand.o            \
    telem.o

