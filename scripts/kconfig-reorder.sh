#!/bin/sh

# This script reorders all config-* files in the target directory.

find_files=$(find target -type f -name 'config-*' -print)

if [ -n "$find_files" ]; then
  for file in $find_files; do
    echo "Reordering options in $file"
    LANG=C ./scripts/kconfig.pl '+' "$file" /dev/null > "$file"-new
    mv "$file"-new "$file"
  done
else
  echo "No files named config-* found."
fi

