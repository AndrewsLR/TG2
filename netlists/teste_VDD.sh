#!/bin/bash

for file in *.cdl; do
	[ -e "$file" ] || continue

 if grep -q '^M.*VDD' "$file"; then
  echo "$file"
 else
  printf "$file\n">> resultados.txt
 fi
done
