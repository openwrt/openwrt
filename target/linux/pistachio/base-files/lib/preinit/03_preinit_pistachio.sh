#!/bin/sh
#
# Copyright (C) 2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

do_pistachio() {
	. /lib/pistachio.sh

	pistachio_board_detect
}

boot_hook_add preinit_main do_pistachio
