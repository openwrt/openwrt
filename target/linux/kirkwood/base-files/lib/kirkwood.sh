#!/bin/sh
#
# Copyright (C) 2014 OpenWrt.org
#

KIRKWOOD_BOARD_NAME=
KIRKWOOD_MODEL=

kirkwood_board_detect() {
	local machine
	local name

	machine=$(cat /proc/device-tree/model)

	case "$machine" in
	"Seagate FreeAgent Dockstar")
		name="dockstar"
		;;

	"Seagate GoFlex Net")
		name="goflexnet"
		;;

	"Iomega Iconnect")
		name="iconnect"
		;;

	"RaidSonic ICY BOX IB-NAS62x0 (Rev B)")
		name="ib62x0"
		;;

	"Cloud Engines Pogoplug E02")
		name="pogo_e02"
		;;

	"Linksys EA3500")
		name="ea3500"
		;;

	"Linksys EA4500")
		name="ea4500"
		;;

	"Globalscale Technologies Guruplug Server Plus")
		name="guruplug-server-plus"
		;;

	"Globalscale Technologies SheevaPlug")
		name="sheevaplug"
		;;

	"Globalscale Technologies eSATA SheevaPlug")
		name="sheevaplug-esata"
		;;

	*)
		name="generic"
		;;
	esac

	[ -z "$KIRKWOOD_BOARD_NAME" ] && KIRKWOOD_BOARD_NAME="$name"
	[ -z "$KIRKWOOD_MODEL" ] && KIRKWOOD_MODEL="$machine"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$KIRKWOOD_BOARD_NAME" > /tmp/sysinfo/board_name
	echo "$KIRKWOOD_MODEL" > /tmp/sysinfo/model
}

kirkwood_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] || kirkwood_board_detect
	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
