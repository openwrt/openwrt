#!/bin/sh /etc/rc.common
# SPDX-License-Identifier: GPL-2.0-only

START=99

boot() {
	case $(board_name) in
	glinet,gl-ar300m-nand)
		fw_setenv bootcount 0
		;;
	linksys,ea4500-v3)
		[ $(fw_printenv -n auto_recovery) = yes ] && \
			fw_setenv auto_recovery no
		;;
	esac
}
