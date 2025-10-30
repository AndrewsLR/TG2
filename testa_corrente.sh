#!/usr/bin/env bash

output_file="mismatch_files.txt"
> "$output_file"  # Clear previous results

for file in netlists/*.cdl; do
    echo "Processing: $file"

    # Use awk to detect if any mismatch occurs before a line starting with "."
    has_mismatch=$(awk '
    BEGIN { mismatch = 0 }
    /^\s*\./ { exit }  # Stop when a line starts with "."
    {
        if(NR < 3){
        	next
        }
        
        prev4 = col4
        col4 = $4
        col2 = $2             
        if(NR == 3){
        	next
        }
        if (prev4 != col2) {
            mismatch = 1
            exit     # no need to continue scanning this file
        }
        
    }
    END { print mismatch }
    ' "$file")

    if [[ "$has_mismatch" -eq 1 ]]; then
        echo "$file" >> "$output_file"
        echo "  → Mismatch found, added to $output_file"
    else
        echo "  → All lines matched"
    fi
done

echo
echo "Done. Files with mismatches are listed in: $output_file"

