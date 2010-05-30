#!/bin/sh

do_ramips() {
	. /lib/ramips.sh
}

boot_hook_add preinit_main do_ramips
