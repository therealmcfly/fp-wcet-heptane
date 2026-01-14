#!/bin/sh

#---------------------------------------------------------------------
#
# Copyright IRISA, 2003-2017
#
# This file is part of Heptane, a tool for Worst-Case Execution Time (WCET)
# estimation.
# APP deposit IDDN.FR.001.510039.000.S.P.2003.000.10600
#
# Heptane is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Heptane is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details (COPYING.txt).
#
# See CREDITS.txt for credits of authorship
#
#---------------------------------------------------------------------

function printUsage {
    echo " usage: run_benchmark_forall_entrypoints.sh benchmark_name arch solver [-q]"
}

if [ $# -lt 1 ] || [ $# -gt 4 ]
then
    printUsage
    exit 1
fi

HERE=`pwd`
OPTION_ANALISYS=
BENCH=$1
ARCH=$2
SOLVER=$3
OPTION=$4
OUTPUTDIR=.
RESULT_DIR=${HERE}/benchmarks/${BENCH}

# (1)  Producing the user fonctions file user_functions.txt in  ${OUTPUTDIR} directory.
echo " **** Getting the entry points"
./extract.sh $BENCH main $ARCH ${OUTPUTDIR}
status=$?
if [ "$status" == "0" ]
then
 # (2) foreach entry point calling the wcept computation.
 ENTRYPOINTS=
 for line in $(cat ${RESULT_DIR}/user_functions.txt); do ENTRYPOINTS="${ENTRYPOINTS} $line" ; done 
 echo " **** ENTRY POINTS = ${ENTRYPOINTS}"
 echo ""
 for ENTRYPOINT in ${ENTRYPOINTS}
 do 
#   echo " run_benchmark.sh ${BENCH} ${ENTRYPOINT} $ARCH ${SOLVER} ${ENTRYPOINT} ${OPTION}"
   run_benchmark.sh ${BENCH} ${ENTRYPOINT} $ARCH ${SOLVER} ${ENTRYPOINT} ${OPTION}
 done

else
  echo " *** ERROR on command: "
  echo " ./extract.sh $1 main $2 ${OUTPUTDIR}"
fi
