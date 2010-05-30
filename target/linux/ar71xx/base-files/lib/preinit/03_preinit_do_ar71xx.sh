#!/bin/sh

do_ar71xx() {
	. /lib/ar71xx.sh
}

boot_hook_add preinit_main do_ar71xx
