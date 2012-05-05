#!/bin/sh

do_mpc83xx() {
	. /lib/mpc83xx.sh

	mpc83xx_board_detect
}

boot_hook_add preinit_main do_mpc83xx
