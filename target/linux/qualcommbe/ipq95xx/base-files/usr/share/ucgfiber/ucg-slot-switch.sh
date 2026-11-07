#!/bin/sh
# On the stock firmware: boot OpenWrt slot A or B from the next start on.
# usage: sh /opt/ucg-switch/ucg-slot-switch.sh a|b
# Uses the tools in this folder, written there by OpenWrt (ucgfiber-stockkit);
# writes only the U-Boot env (ucg_boot). Reboots only when the switch succeeded.
D="$(cd "$(dirname "$0")" && pwd)"
case "$1" in
	a|A) slot=a ;;
	b|B) slot=b ;;
	*) echo "usage: sh $0 a|b"; exit 1 ;;
esac
if sh "$D/usr/sbin/ucgfiber-stock-envset" ucg_boot "owrt_$slot"; then
	echo "ucg_boot=owrt_$slot set, rebooting"
	${UCG_REBOOT:-reboot}
else
	echo "refused, nothing written"
	exit 1
fi
