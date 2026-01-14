#!/bin/sh

varchi=RISCV
vhour=$(date +%H%M)
vday=$(date +%Y%m%d)

./benchmarks/mktests.sh ${varchi} | tee LOGFILE_${varchi}_${vday}_${vhour}

