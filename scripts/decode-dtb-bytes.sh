#!/bin/sh
#
# Decode dtb bytes
# - written by kimocoder (christian@aircrack-ng.org)
#
# This is how it works; you find strings like these two in columns. Run the byte decoder to reveal them.
# dev@kimocoder:$ echo '[43 52 4d 2d 54 5a 2e 57 4e 53 2e 34 2e 30 2d 30 30 31 30 37 0a]' | ./scripts/decode-dtb-bytes.sh
# CRM-TZ.WNS.4.0-00107
# dev@kimocoder:$ echo '[43 52 4d 2d 42 4f 4f 54 2e 42 46 2e 33 2e 33 2e 31 2e 31 2d 30 30 30 36 37 0a]' | ./scripts/decode-dtb-bytes.sh
# CRM-BOOT.BF.3.3.1.1-00067
#
#

while read -r line; do
    # Remove brackets and spaces, convert hex to a continuous hex string
    hexstring=$(echo "$line" | tr -d '[] ')

    # Use xxd to convert hex to ascii
    echo "$hexstring" | xxd -r -p
done
