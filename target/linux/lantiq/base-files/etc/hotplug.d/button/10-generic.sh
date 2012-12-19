#!/bin/sh

[ "${ACTION}" = "released" ] || exit 0

. /lib/functions.sh

case "${BUTTON}" in
	reset)
		logger "reset pressed"
		echo "REBOOT" > /dev/console
		sleep 3
		sync
		reboot
		;;
	BTN_1)
		logger "factory pressed"
		echo "FACTORY RESET" > /dev/console
		jffs2_mark_erase "rootfs_data"
		sync
		reboot
		;;
	*)
		logger "unknown button ${BUTTON}"
		;;
esac
