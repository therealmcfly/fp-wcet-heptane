#!/bin/sh

TMPFILE=/tmp/XXX
\cp $1 ${TMPFILE}
emacs -batch  ${TMPFILE} -l ${PWD}/scripts/logtests.el -f wcet_log_update

\rm $1_res;
grep WCET:  ${TMPFILE} | tee $1_res


