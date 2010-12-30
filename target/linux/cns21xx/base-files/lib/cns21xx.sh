#!/bin/sh
#
# Copyright (C) 2010 OpenWrt.org
#

get_board_name() {
	local hardware
	local name

	hardware=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /Hardware/ {print $2}' /proc/cpuinfo)

	case "$hardware" in
	*NSB3AST)
		name="nsb3ast"
		;;
	"NS-K330 NAS")
		name="ns-k330"
		;;
	*)
		name="generic"
		;;
	esac

	echo $name
}
