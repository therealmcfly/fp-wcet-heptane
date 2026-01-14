#!/bin/sh

ARCH=$1
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

# Attention ludcmp ok avec type float / nok en double. (ok mais pointer)

TEST_SET="bs bsort100 crc expint fft fibcall insertsort jfdctint lcdnum ludcmp matmult ns nsichneu qurt "
${DIR_SCRIPT}/run_benchmarks.sh "$TEST_SET" $ARCH $SOLVER

TEST_SET="select simple sqrt statemate ud  cover minver minmax prime "
${DIR_SCRIPT}/run_benchmarks.sh "$TEST_SET" $ARCH $SOLVER

exit

TEST_SET="cnt lms "  # OK pour ARM, not for MIPS


TEST_SET="bsort100_1 fdct fir "
${DIR_SCRIPT}/run_benchmarks.sh "$TEST_SET" $ARCH $SOLVER

exit

excluded: edn adpcm ndes compress




