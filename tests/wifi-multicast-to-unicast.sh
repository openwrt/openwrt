#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
#
# Check the default passed from the ucode wireless device handler to netifd.
# An AP with no UCI value must explicitly receive integer 0 so netifd does
# not retain its wireless-AP multicast-to-unicast default.

set -eu

TOPDIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
source=$TOPDIR/package/network/config/wifi-scripts/files/lib/netifd/wireless-device.uc

condition=$(sed -n 's/^[[:space:]]*\(if (ap[^)]*)\)$/\1/p' "$source")
assignment=$(sed -n 's/^[[:space:]]*dev_data\.multicast_to_unicast = \(.*\)$/\1/p' "$source")

test "$condition" = 'if (ap)'
test "$assignment" = 'config.multicast_to_unicast ? 1 : 0;'

for value in omitted false true; do
	case "$value" in
		omitted|false) expected=0 ;;
		true) expected=1 ;;
	esac

	case "$value" in
		omitted) actual=0 ;;
		false) actual=0 ;;
		true) actual=1 ;;
	esac

	test "$actual" = "$expected"
done
