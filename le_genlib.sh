#!/bin/bash

#Script to iterate over the genlib file
#Height variable defines how many row of metal are available, ignoring power
#Height 0 means routing using only the necessary lines for interconnect, ignoring contacts
>"Netlists.spice"
>"netlists_test.cdl"
>"Nets.txt"
>"saida.txt"
>"Roteavel.txt"
col_index=4
<<<<<<< Updated upstream
height=0
=======
height=4
>>>>>>> Stashed changes
exe="./imp_pandr.exe"

awk "NR>=3 && NR%2==1 {print \$$col_index}" 44-6.genlib | while read -r word; do
    # strip first 3 chars and last char
    processed="${word:3}"                 # drop first 3 chars
    processed="${processed::-1}"          # drop last char
    "$exe" "$processed" "$height"
done
