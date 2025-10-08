#!/bin/bash

for file in netlists/*.cdl; do
  [ -e "$file" ] || continue

  dir=$(dirname "$file")
  raw_name=$(basename "$file" .cdl)

  modified_name="${modified_name//\(/_}"  # Replace ( with _
  modified_name="${modified_name//\)/_}"  # Replace ) with _
  modified_name="${modified_name//\!/n}"  # Replace ) with _
  echo "Processing $file -> replacing .SUBCKT name with $modified_name"

  sed -i "/\.SUBCKT / s/(/_/g;/\.SUBCKT / s/)/_/g;/\.SUBCKT / s/!/n/g" "$file"
  sed -i "/\PLACE_BEGIN / s/(/_/g;/\PLACE_BEGIN / s/)/_/g;/\PLACE_BEGIN / s/!/n/g" "$file"
  sed -i "/\PLACE_END / s/(/_/g;/\PLACE_END / s/)/_/g;/\PLACE_END / s/!/n/g" "$file"

  filename_out="${raw_name//\(/_}"
  filename_out="${filename_out//\)/_}"
  filename_out="${filename_out//\!/n}"
  filename_out="${filename_out}.cdl"

  mv "$file" "$filename_out"
  echo "Renamed $file → $filename_out"
done
