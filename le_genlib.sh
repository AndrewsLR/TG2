#!/bin/bash
>"Netlists.spice"
>"netlists_test.cdl"
>"Nets.txt"
>"saida.txt"
>"Roteavel.txt"
col_index=4 
exe="./imp_pandr.exe"

awk "NR>=3 && NR%2==1 {print \$$col_index}" 44-6.genlib | while read -r word; do
    # strip first 3 chars and last char
    processed="${word:3}"                 # drop first 3 chars
    processed="${processed::-1}"          # drop last char
    "$exe" "$processed" "2"
done
