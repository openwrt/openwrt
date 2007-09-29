#!/bin/sh
# Copyright (C) 2007 OpenWrt.org

set_led() {
	local led="$1"
	local state="$2"
	[ -d "/sys/class/leds/ar7:$led" ] && echo "$state" > "/sys/class/leds/ar7:$led/brightness"
}

set_state() {
	case "$1" in
		preinit)
			set_led status 1
		;;
		done)
			set_led status 0
		;;
	esac
}
