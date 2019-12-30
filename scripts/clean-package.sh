#!/usr/bin/env bash
IFS=$'\n'

if [ -z "$1" ] || [ -z "$2" ]; then
	echo "Usage: $0 <file> <directory>"
	exit 1
fi
if [ ! -f "$1" ] || [ ! -d "$2" ]; then
	echo "File/directory not found"
	exit 1
fi
(
	cd "$2" || exit 1
	while read -r entry; do
		[ -n "$entry" ] || break
		[ ! -d "$entry" ] || [ -L "$entry" ] && rm -f "$entry"
	done
) < "$1"
sort -r "$1" | (
	cd "$2" || exit 1
	while read -r entry; do
		[ -n "$entry" ] || break
		[ -d "$entry" ] && rmdir "$entry" > /dev/null 2>&1
	done
)
true
