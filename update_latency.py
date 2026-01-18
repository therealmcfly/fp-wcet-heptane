#!/usr/bin/env python3

import csv
import re
import sys
from pathlib import Path

LATENCY_PATH = Path(sys.argv[1])
CSV_PATH     = Path(sys.argv[2])

# ----------------------------
# LOAD CSV (READ-ONLY)
# ----------------------------
inst_cycles = {}
csv_insts = set()

with CSV_PATH.open(newline="") as f:
    reader = csv.reader(f)
    header = next(reader, None)

    for row in reader:
        if len(row) < 2:
            continue
        inst = row[0].strip()
        cycle = row[1].strip()
        inst_cycles[inst] = cycle
        csv_insts.add(inst)

# ----------------------------
# PROCESS latency.data
# ----------------------------
line_re = re.compile(r'^(\s*"([^"]+)"\s*:\s*)(\d+)(.*)$')

latency_insts = set()
out_lines = []

total_processed = 0
updated = 0
not_found = 0

with LATENCY_PATH.open() as f:
    for line in f:
        m = line_re.match(line.rstrip("\n"))

        if not m:
            out_lines.append(line)
            continue

        total_processed += 1
        prefix, inst, value, suffix = m.groups()
        latency_insts.add(inst)

        if inst in inst_cycles:
            new_value = inst_cycles[inst]
            if new_value != value:
                updated += 1
                out_lines.append(f"{prefix}{new_value} UPDATED\n")
            else:
                out_lines.append(f"{prefix}{new_value}\n")
        else:
            not_found += 1
            out_lines.append(f"{prefix}{value} NOTFOUND\n")

# ----------------------------
# CSV → latency.data augmentation
# ----------------------------
missing_in_latency = sorted(csv_insts - latency_insts)
added = len(missing_in_latency)

for inst in missing_in_latency:
    cycle = inst_cycles[inst]
    out_lines.append(f"\"{inst}\"   : {cycle} ADDED\n")

# ----------------------------
# WRITE BACK latency.data ONLY
# ----------------------------
LATENCY_PATH.write_text("".join(out_lines))

# ----------------------------
# SUMMARY
# ----------------------------
print("=== latency.data update summary ===")
print(f"Total processed instructions : {total_processed}")
print(f"Updated instruction values   : {updated}")
print(f"Not found instruction values : {not_found}")
print(f"Added instruction values     : {added}")
