#!/bin/sh

lantiq_board_detect() {
	name="$(strings /proc/device-tree/compatible | head -1)"
	model="$(cat /proc/device-tree/model)"
	[ -z "$name" ] && name="unknown"
	[ -z "$model" ] && model="unknown"
	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"
	echo $name > /tmp/sysinfo/board_name
	echo $model > /tmp/sysinfo/model
}

do_lantiq() {
	. /lib/functions/lantiq.sh

	lantiq_board_detect
}

boot_hook_add preinit_main do_lantiq
