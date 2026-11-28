#!/bin/sh

reject() {
	echo "WR3000K transition refused: $*" >&2
	command -v system_start_all >/dev/null && system_start_all
	exit 1
}

[ "$(cat /tmp/sysinfo/board_name)" = mt7981-spim-nand-gsw-wr3000k ] ||
	reject "unexpected device"
grep -qx "DISTRIB_RELEASE='V1.0-202603161623'" /rom/etc/openwrt_release ||
	reject "unsupported stock firmware"
awk '
	$2 == "03000000" && $3 == "00020000" && $4 == "\"ubi\"" { a++ }
	$2 == "03000000" && $3 == "00020000" && $4 == "\"ubi1\"" { b++ }
	END { exit !(a == 1 && b == 1) }
' /proc/mtd || reject "unexpected NAND layout"

slot="$(sed -n 's/.* ubi\.mtd=\(ubi[1]*\) .*/\1/p' /proc/cmdline)"
case "$slot" in
	ubi|ubi1) ;;
	*) reject "unknown active slot" ;;
esac

[ "$(fw_printenv -n env_init 2>/dev/null)" = 1 ] &&
[ "$(fw_printenv -n boot_from 2>/dev/null)" = "$slot" ] ||
	reject "inconsistent boot environment"

[ "$(cat /sys/module/boot_param/parameters/dual_boot 2>/dev/null)" = N ] ||
	reject "unexpected upgrade mode"
exit 0
