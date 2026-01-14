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

if [ "${XARCH}" = "MIPS" ] || [ "${XARCH}" = "ARM" ] || [ "${XARCH}" = "MSP430" ] || [ "${XARCH}" = "RISCV" ] || [ "${XARCH}" = "FLEXPRET" ]
then
    cd ${RESULT_DIR}
    sed  -e "s#BENCH_DIR#${RESULT_DIR}#g" -e "s/X_BENCH/${BENCH}/g" -e "s/_ENTRY_POINT_/${ENTRYPOINT}/g"  -e "s/_SOLVER_/${SOLVER}/g"  -e "s#_CROSS_COMPILER_DIR_#/home/eugene/heptane-master/CROSS_COMPILERS/${XARCH}/bin#g" /home/eugene/heptane-master/config_files/configWCET_template_${XARCH}.xml > ${CONFIGFILE}
    chmod gou+x  ${CONFIGFILE}
    /home/eugene/heptane-master/bin/HeptaneAnalysis ${OPTION}  ${CONFIGFILE}  | tee ${RESULT_DIR}/analysis_${BENCH}_${XARCH}_${SOLVER}.log

		# ------------------------------------------------------------
		# Generate CSV: node_id, execution_count, instructions
		# ------------------------------------------------------------

		RES_IPET_XML="${RESULT_DIR}/resIPET.xml"
		CSV_OUT="${RESULT_DIR}/node_exec_count.csv"

		if [ -f "${RES_IPET_XML}" ]; then
				echo "Generating node execution CSV..."

				echo "node_id,context,execution_count,instructions" > "${CSV_OUT}"

				awk '
				BEGIN {
					in_node = 0
					node_id = ""
					instr = ""
				}

				# Start node
				/<NODE[[:space:]]/ {
					in_node = 1
					node_id = ""
					instr = ""
					delete freq

					if (match($0, /id="([0-9]+)"/, id_match))
						node_id = id_match[1]
				}

				# Collect instructions
				in_node && /<INSTRUCTION / {
					if (match($0, /code="([^"]+)"/, instr_match)) {
						if (instr == "")
							instr = instr_match[1]
						else
							instr = instr "; " instr_match[1]
					}
				}

				# Collect all frequency_cX
				in_node && /name="frequency_c[0-9]+"/ {
					if (match($0, /name="(frequency_c[0-9]+)"/, name_match) &&
							match($0, /value="([0-9]+)"/, val_match)) {
						freq[name_match[1]] = val_match[1]
					}
				}

				# End node
				/<\/NODE>/ {
					if (node_id != "") {
						for (k in freq) {
							if (freq[k] + 0 > 0) {
								ctx = k
								sub(/^frequency_/, "", ctx)   # frequency_c2 → c2
								gsub(/"/, "\"\"", instr)
								print node_id "," ctx "," freq[k] ",\"" instr "\""
							}
						}
					}
					in_node = 0
				}
				' "${RES_IPET_XML}" >> "${CSV_OUT}"

				echo "CSV written to ${CSV_OUT}"
		else
				echo "WARNING: ${RES_IPET_XML} not found, CSV not generated"
		fi

		# ------------------------------------------------------------
		# Generate CSV: per-instruction WCET contribution
		# Columns: node_id, execution_count, instruction, cycles
		# ------------------------------------------------------------

		FP_TIMING_MAP="/home/eugene/heptane-master/fp_inst_timing.csv"
		INSTR_WCET_OUT="${RESULT_DIR}/instruction_cost_by_node_context_exec.csv"

		if [ -f "${RES_IPET_XML}" ] && [ -f "${FP_TIMING_MAP}" ]; then
				echo "Generating instruction WCET CSV (context-aware)..."

				echo "node_id,context,node_exec_count,instruction,cycles" > "${INSTR_WCET_OUT}"

				awk -F',' '
						# -------------------------------------------------
						# First pass: load instruction timing table
						# -------------------------------------------------
						NR==FNR {
								timing[$1] = $2
								next
						}

						# -------------------------------------------------
						# Start of a CFG node
						# -------------------------------------------------
						/<NODE[[:space:]]/ {
								node_id = ""
								instr_count = 0
								delete freq

								if (match($0, /id="([0-9]+)"/, a))
										node_id = a[1]
						}

						# -------------------------------------------------
						# Collect instructions
						# -------------------------------------------------
						/<INSTRUCTION / {
								if (match($0, /code="([^"]+)"/, c)) {
										split(tolower(c[1]), parts, /[[:space:]]+/)
										instrs[++instr_count] = parts[1]
								}
						}

						# -------------------------------------------------
						# Collect per-context execution counts
						# -------------------------------------------------
						/name="frequency_c[0-9]+"/ {
								if (match($0, /name="frequency_c([0-9]+)"/, c) &&
										match($0, /value="([0-9]+)"/, v)) {
										freq[c[1]] = v[1]
								}
						}

						# -------------------------------------------------
						# End of node → emit rows
						# -------------------------------------------------
						/<\/NODE>/ {
								for (ctx in freq) {
										if (freq[ctx] > 0) {
												for (i = 1; i <= instr_count; i++) {
														instr = instrs[i]

														if (!(instr in timing)) {
																printf("ERROR: UNKNOWN timing for instruction %s\n", instr) > "/dev/stderr"
																exit 1
														}

														print node_id ",c" ctx "," freq[ctx] "," instr "," timing[instr]
												}
										}
								}

								delete instrs
								delete freq
						}
				' "${FP_TIMING_MAP}" "${RES_IPET_XML}" >> "${INSTR_WCET_OUT}"

				echo "CSV written to ${INSTR_WCET_OUT}"
		else
				echo "WARNING: expanded instruction WCET CSV not generated (missing files)"
		fi

    #clean: uncomment if you want to clean
     #rm ${CONFIGFILE}
    cd $HERE
else
 echo ">>> ERROR: unknown target= ${XARCH}, waiting for MIPS, ARM, MSP430 or RISCV"
fi
