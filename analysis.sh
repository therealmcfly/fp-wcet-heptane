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

if [ $# -lt 1 ] || [ $# -gt 6 ]
then
    echo "usage: $0 benchmark_name entrypoint arch solver outputDir [-t]"
    exit 1
fi

HERE=`pwd`
BENCH=$1
ENTRYPOINT=$2
ARCHI=$3
SOLVER=$4
OUTPUTDIR=$5
OPTION=$6

RESULT_DIR=${HERE}/benchmarks/${BENCH}/${OUTPUTDIR}

if [ ! -d "${RESULT_DIR}" ];then
    echo ">>> ERROR: The directory benchmarks/${BENCH} does not exist ! exiting ...";
    exit -1
fi

if [ ! -f "${RESULT_DIR}/${BENCH}.xml" ];then
    echo ">>> ERROR: The file ${RESULT_DIR}/${BENCH}.xml does not exist ! exiting ...";
    exit -1
fi

if [ "${SOLVER}" != "lp_solve" ] && [ "${SOLVER}" != "cplex" ]
then
    echo ">>> ERROR: Unknown solver: ${SOLVER} !, waiting for lp_solve or cplex."
    exit -1
fi

CONFIGFILE=${RESULT_DIR}/configWCET.xml

if [[ "${ARCHI}" =~ ^MSP430.* ]] ; then XARCH=MSP430 ; else XARCH=${ARCHI} ; fi

if [ "${XARCH}" = "MIPS" ] || [ "${XARCH}" = "ARM" ] || [ "${XARCH}" = "MSP430" ] || [ "${XARCH}" = "RISCV" ]
then
    cd ${RESULT_DIR}
    sed  -e "s#BENCH_DIR#${RESULT_DIR}#g" -e "s/X_BENCH/${BENCH}/g" -e "s/_ENTRY_POINT_/${ENTRYPOINT}/g"  -e "s/_SOLVER_/${SOLVER}/g"  -e "s#_CROSS_COMPILER_DIR_#/home/eugene/heptane-master/CROSS_COMPILERS/${XARCH}/bin#g" /home/eugene/heptane-master/config_files/configWCET_template_${XARCH}.xml > ${CONFIGFILE}
    chmod gou+x  ${CONFIGFILE}
    /home/eugene/heptane-master/bin/HeptaneAnalysis ${OPTION}  ${CONFIGFILE}  | tee ${RESULT_DIR}/analysis_${BENCH}_${XARCH}_${SOLVER}.log

    #clean: uncomment if you want to clean
     #rm ${CONFIGFILE}
    cd $HERE
else
 echo ">>> ERROR: unknown target= ${XARCH}, waiting for MIPS, ARM, MSP430 or RISCV"
fi
