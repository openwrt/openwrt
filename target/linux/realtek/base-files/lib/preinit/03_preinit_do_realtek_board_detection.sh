#!/bin/sh

do_realtek_board_detection() {
	. /lib/realtek.sh

	realtek_board_detect
}

boot_hook_add preinit_main do_realtek_board_detection
