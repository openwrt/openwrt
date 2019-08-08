#!/bin/sh

DTB="$1"
FWPART="$2"
PARTITION_NODES="$3"

for part_node in $PARTITION_NODES; do
	for partition in $(fdtget -l $DTB $part_node 2>/dev/null); do
		name=$(fdtget $DTB $part_node/$partition label 2>/dev/null)
		[ "$name" = "$FWPART" ] && {
			size=$(fdtget $DTB $part_node/$partition reg | cut -d ' ' -f2)
			if [ -n "$size" ]; then
				echo "$size"
				exit 0
			fi
		}
	done
done

echo 0
echo "WARNING: No $FWPART partition found" >&2
exit 1
