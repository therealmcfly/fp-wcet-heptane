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
    echo " usage: run_benchmark.sh benchmark_name entrypoint arch solver [ outputdir [-q]]"
    echo "   -q for quiet "
    echo "   outputdir:  subdir of benchmark_name. It will contain the results."
}

if [ $# -lt 1 ] || [ $# -gt 6 ]
then
    printUsage
    exit 1
fi

optExtract=
optAnalysis=
if [ $# == 6 ]
    then
    outputdir=$5
    if [ $6 == "-q" ]
	then
	optAnalysis="-t"
    else
        printUsage
	exit 1
    fi
else
    optExtract="-v"
    if [ $# == 4 ]
    then
     # or set to the arch
     outputdir=.
    else
     outputdir=$5
    fi
fi

./extract.sh $1 $2 $3 $outputdir $optExtract

status=$?
if [ "$status" == "0" ]
then
./analysis.sh $1 $2 $3 $4 $outputdir $optAnalysis
fi
