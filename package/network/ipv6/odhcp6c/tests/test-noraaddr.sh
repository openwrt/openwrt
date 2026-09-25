#!/bin/sh

set -eu

script=${1:-"$(dirname "$0")/../files/dhcpv6.script"}

merge=$(awk '
/^[[:space:]]*# Merge addresses/ { in_block = 1; next }
/^[[:space:]]*for entry in \$ADDRESSES; do$/ { exit }
in_block { print }
' "$script")

run_merge()
{
	eval "$merge"
}

original='2001:db8:1::1/64,3600,7200'
ra_address='2001:db8:ffff::1/64,3600,7200'

ADDRESSES="$original"
RA_ADDRESSES="$ra_address"
NORAADDR=1
run_merge
[ "$ADDRESSES" = "$original" ] || {
	echo "noraaddr failed to suppress an RA-derived address" >&2
	exit 1
}

ADDRESSES="$original"
RA_ADDRESSES="$ra_address"
NORAADDR=0
run_merge
case " $ADDRESSES " in
	*" $ra_address "*) ;;
	*) echo "RA-derived address was not merged by default" >&2; exit 1 ;;
esac

echo "noraaddr regression passed"
