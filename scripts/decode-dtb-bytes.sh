#!/bin/sh
#
# Decode dtb bytes
# - written by kimocoder (christian@aircrack-ng.org)
#

while read -r line; do
    # Remove brackets and spaces, convert hex to a continuous hex string
    hexstring=$(echo "$line" | tr -d '[] ')

    # Use xxd to convert hex to ascii
    echo "$hexstring" | xxd -r -p
done
