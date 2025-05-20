#!/bin/bash
> "nets_no_vdd.txt"
for file in *.cdl; do
	[ -e "$file" ] || continue
	
	if awk '{ if ($2 == "VDD" || $4 == "VDD") { found=1; exit } } END { exit !found }' "$file"; then
		echo "$file"
	else
		echo "$file" >> "nets_no_vdd.txt"
	fi
done
