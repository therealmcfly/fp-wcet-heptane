#!/bin/sh

# mktests.sh ARCHI [-q]
if [ $# -lt 1 ]
then
    echo "usage: $0 ARCH SOLVER [-q]"
    echo "   * ARCH ::= MIPS | ARM"
    echo "   * SOLVER ::= cplex | lp_solve "
    echo "   * optional: -q for quiet "
    exit 1
fi

BENCHMARKS="$1"
ARCHI=$2
SOLVER=$3
QUIET=$4

echo "Parameters: $ARCHI $SOLVER $QUIET"

line()
{
    echo "------------------------------------------------------------------------------------------------------------------------------------------"
}

__run_benchmarks()
{
# using orange for loops
    for X in $1 ; do
	line
	echo "benchmark:  $X"
	cd benchmarks/$X
	# Pour avoir plus de détails : " --auto --print_exp "
	${HEPTANE_DIR}/softs/otawa/otawa/linux-x86_64/otawa-core/bin/orange -I .. --auto $X.c main -o $X.orange
	cd ../..
    done
}

for X in $BENCHMARKS ; do
    line
    echo "benchmark:  $X "
    bash run_benchmark.sh $X main $ARCHI $SOLVER $QUIET
done




