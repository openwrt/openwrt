#!/bin/sh
. /lib/sunxi.sh

do_b53_hack() {
	# hack: enable switch on Lamobo R1 and reset counters
	case "$( sunxi_board_name )" in
	"lamobo-r1")
		ifconfig eth0 up
		sleep 1
		swconfig dev switch0 set reset 1
		swconfig dev switch0 set reset_mib 1
		swconfig dev switch0 set apply 1
		;;
	esac
}

boot_hook_add preinit_main do_b53_hack
