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
	"bcm63xx/CPVA642 "* | "bcm63xx/MAGIC "* | "bcm63xx/V2500V_BB "* | \
	"bcm63xx/V2110 "* | "bcm63xx/CPVA502+ "*)
		do_fixcrc
		;;
esac

