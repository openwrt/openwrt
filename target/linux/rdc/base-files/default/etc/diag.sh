#!/bin/sh
# Copyright (C) 2007 OpenWrt.org

set_led() {
	local led="$1"
	local state="$2"
	[ -d "/sys/class/leds/rdc321x:$led" ] && echo "$state" > "/sys/class/leds/rdc321x:$led/brightness"
}

set_state() {
	case "$1" in
		preinit)
			set_led dmz 1
		;;
		done)
			set_led dmz 0
		;;
	esac
}
