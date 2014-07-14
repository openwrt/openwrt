#!/bin/sh

do_lantiq() {
	. /lib/functions/lantiq.sh

	lantiq_board_detect
}

boot_hook_add preinit_main do_lantiq
