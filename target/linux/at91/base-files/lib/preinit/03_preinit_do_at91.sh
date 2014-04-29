#!/bin/sh

do_at91() {
	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"
	echo $(strings /proc/device-tree/compatible | head -1) > /tmp/sysinfo/board_name
	echo $(cat /proc/device-tree/model) > /tmp/sysinfo/model
}

boot_hook_add preinit_main do_at91
