#!/bin/bash

for file in *.cdl; do
	[ -e "$file" ] || continue
	
    if awk '{ if ($2 == "VDD" || $4 == "VDD") { found=1; exit } } END { exit !found }' "$file"; then
        echo "$file" >> "$output_file"
    fi
done
