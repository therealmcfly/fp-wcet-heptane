#!/bin/sh

testfailure="false"

export HEPTANE_FORCE_MODE=1

# Look devices.csv file to get infos about the ISA  (before option -mcpu=xxx was used, now deprecated).
# for ISA 430 : ARCH=MSP430F4250, for ISA 430X: ARCH= , for ISA 430xv2 : ARCH=MSP430FR5739
ARCH=MSP430F4250

SOLVER=lp_solve
DIR_SCRIPT=./benchmarks

if [ ! -e ${DIR_SCRIPT}/../bin/HeptaneExtract ]; then
   echo "HeptaneExtract not found. Exiting..."
   exit -1
fi

if [ ! -e ${DIR_SCRIPT}/../bin/HeptaneAnalysis ]; then
   echo "HeptaneExtract not found. Exiting..."
   exit -1
fi

vhour=$(date +%H%M)
vday=$(date +%Y%m%d)


TEST_SET="simple bs fibcall insertsort select cover lcdnum"
${DIR_SCRIPT}/run_benchmarks.sh "$TEST_SET" $ARCH $SOLVER  | tee LOGFILE_${ARCH}_${vday}_${vhour}

# To HAVE MORE RAM.
ARCH=MSP430F439
TEST_SET=" crc "
${DIR_SCRIPT}/run_benchmarks.sh "$TEST_SET" $ARCH $SOLVER  | tee LOGFILE_${ARCH}_${vday}_${vhour}

# --------------------------------------------------  
if [ "$testfailure" == " true" ]
then
    TEST_SET=" expint matmult ud fft jfdctint ludcmp qurt sqrt statemate prime minver adpcm "
    ${DIR_SCRIPT}/run_benchmarks.sh "$TEST_SET" $ARCH $SOLVER  | tee LOGFILE_${ARCH}_${vday}_${vhour}_failure
fi

exit


cover  : Cfg main does not have endnodes with -ggdb


bsort100    : Warning: a loop does not have the maxiter attribute set in CFG __mspabi_mpyi
fdct minmax : Warning: a loop does not have the maxiter attribute set in CFG __mspabi_mpyi

ns          : region data overflowed by 1476 bytes
nsichneu    : region text overflowed by 608 bytes
fir         : region data overflowed by 4804 bytes

lms         : address 0x95e of /tmp/lms section .bss is not within region data

ndes        : EXCLUDED  (for MSP430, MIPS, ARM)

