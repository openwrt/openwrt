#!/bin/sh
#
# Copyright (C) 2007 OpenWrt.org
#
#

. /lib/brcm63xx.sh

do_fixcrc() {
	mtd fixtrx linux
}

brcm63xx_detect

case "$board_name" in
	"CPVA502+" |\
	CPVA642 |\
	MAGIC |\
	V2110 |\
	V2500V_BB)
		do_fixcrc
		;;
esac

