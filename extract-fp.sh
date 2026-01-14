#!/bin/bash

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

function MSP430_GET_MCU_ISA {
# Specific to MSP430
# Setting the MCU and ISA variables.
# The  MCU is the lower case of the $XARCH value (ie second argumen of the script)
# Getting the ISA (Instruction Set Architecture): msp430, msp430x, and msp430xv2.

if [[ "$XARCH" =~ ^MSP430.* ]]
    then
    MCU=$(echo ${XARCH,,})
    ISA="430"
    local mcu_infos=$(grep "${MCU}," /home/eugene/heptane-master/CROSS_COMPILERS/MSP430/include/devices.csv)
    if [ -z "${mcu_infos}" ]
	then
	echo " MCU ${MCU} not found in the cross compiler devices /home/eugene/heptane-master/CROSS_COMPILERS/MSP430/include/devices.csv"
	echo " You can try  MSP430F4250 for ISA 430, MSP430FR5739 for ISA 430xv2 "
	exit -1
    else
	# msp430f4250,CODE_CPU,... with CODE_CPU=0 for MSP430, 1 for MSP430X and 2  for MSP430XV2
	# echo " mcu_infos=${mcu_infos}"
        local e="$(expr index "${mcu_infos}" ',')"
        local xx="$(echo ${mcu_infos:${e}})"
        local e="$(expr index "${xx}" ',')"
        local CODEMCU="$(echo ${xx:0:${e}-1})"
	if [ "$CODEMCU" = "0" ]
	then 
	    ISA=430
	else 
	    if [ "$CODEMCU" = "1" ]
	    then 
		ISA=430x 
	    else
		if [ "$CODEMCU" = "2" ]
		then
		    ISA=430xv2 
		else  
		    echo " Error code CPU from MCU ${mcu_infos} ... exiting "
                    exit -1
		fi
	    fi
	fi
    fi
    XARCH=MSP430
fi
}

if [ $# -lt 1 ] || [ $# -gt 5 ]
then
    echo "Usage: $0 benchmark_name entrypoint arch outputdir [-v]"
    exit 1
fi

HERE=`pwd`
BENCH=$1
ENTRYPOINT=$2
XARCH=$3
OUTPUTSUBDIR=$4
OPTION=$5

RESULT_DIR=${HERE}/benchmarks/${BENCH}

if [ ! -d "${RESULT_DIR}" ];then
    echo ">>> ERROR: the directory benchmarks/${BENCH} does not exist ! exiting ...";
    exit -1
fi

mkdir -p ${RESULT_DIR}/${OUTPUTSUBDIR}


MCU=
ISA=
# Specific for MSP430 get the definitions of MCU and ISA to use.
# Example : ./extract.sh expint MSP430FR5739 .
# The MCU is "msp430fr5739" (lower case of the ${XARCH} argument)
# The ISA is defined from MCU and an internal table (devices.csv) of the compiler.
MSP430_GET_MCU_ISA

if [ "$XARCH" = "MIPS" ] || [ "$XARCH" = "ARM" ] || [ "$XARCH" = "MSP430" ] || [ "$XARCH" = "RISCV" ] || [ "$XARCH" = "FLEXPRET" ]
then
    if [ -d "/home/eugene/heptane-master/CROSS_COMPILERS/$XARCH" ];then
	cd ${RESULT_DIR}
	\rm *.xml
        #configExtract.xml generation : _MPCU_ and _ISA_ are parameters of the MSP430 family.
	sed -e "s#BENCH_DIR#${RESULT_DIR}#g" -e "s#X_BENCH#${BENCH}#g" -e "s#_ENTRY_POINT_#${ENTRYPOINT}#g" -e "s#_RESULTDIR_#${OUTPUTSUBDIR}#g" -e "s#_MCU_#${MCU}#g" -e "s#_ISA_#${ISA}#g" /home/eugene/heptane-master/config_files/configExtract_template_${XARCH}.xml >  ${OUTPUTSUBDIR}/configExtract.xml
	chmod gou+x ${OUTPUTSUBDIR}/configExtract.xml
         #extraction
	/home/eugene/heptane-master/bin/HeptaneExtract ${OPTION}  ${OUTPUTSUBDIR}/configExtract.xml  | tee  ${OUTPUTSUBDIR}/extract_${BENCH}_${XARCH}.log
	
	cd $HERE
	exit 0
    else
	echo ">>> ERROR: the ${XARCH} cross compiler does not exist in /home/eugene/heptane-master/CROSS_COMPILERS !!! "
	exit -1
    fi
else
    echo ">>> ERROR: unknown target ${XARCH}, waiting for MIPS, ARM, MSP430 MCU or RISCV"
    exit -1
fi


