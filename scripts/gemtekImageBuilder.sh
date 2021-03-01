#!/bin/sh
IMAGE=$1
OUTPUT=$2
MAGIC=".GEMTEK."
CHKSUM=$(cksum $IMAGE | awk '{print $1}')
# 16 bytes
HDR=$(printf "%8s%08X" $MAGIC $CHKSUM)

(cat $IMAGE ; echo -n $HDR) > $OUTPUT
(md5sum $OUTPUT | cut -d' ' -f 1) > ${OUTPUT}.md5
