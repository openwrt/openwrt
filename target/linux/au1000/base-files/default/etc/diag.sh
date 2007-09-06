#!/bin/sh
# Copyright (C) 2007 OpenWrt.org

set_led() {
	local led="$1"
	local state="$2"
	[ -d "/sys/class/leds/mtx1:$led" ] && echo "$state" > "/sys/class/leds/mtx1:$led/brightness"
}

set_state() {
	case "$1" in
		preinit)
			set_led green 0
			set_led red 1
		;;
		failsafe)
			set_led green 1
			set_led red 1
		;;
		done)
			set_led green 1
			set_led red 0
		;;
	esac
}
