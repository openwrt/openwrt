#!/bin/sh
#
# SPDX-License-Identifier: GPL-2.0-only
#
# Records one build stage event. See scripts/build-time-report.pl.
#
# Usage: build-time-log.sh <logfile> <begin|end> <stage> <name>

[ $# -eq 4 ] || exit 0

now=$(date +%s.%N 2>/dev/null)
case "$now" in
	''|*N*) now=$(date +%s) ;;
esac

printf '%s\t%s\t%s\t%s\n' "$now" "$2" "$3" "$4" >> "$1" 2>/dev/null

exit 0
