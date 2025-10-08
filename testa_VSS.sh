#!/bin/bash
> "nets_no_vss.txt"
for file in netlists/*.cdl; do
	[ -e "$file" ] || continue
	
	if awk '{ if ($2 == "VSS" || $4 == "VSS") { found=1; exit } } END { exit !found }' "$file"; then
		echo "$file"
	else
		echo "$file" >> "nets_no_vss.txt"
	fi
done
