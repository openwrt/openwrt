#!/bin/sh
# Copyright (C) 2006 OpenWrt.org

set_led() {
	local led="$1"
	local state="$2"
	[ -f "/proc/diag/led/$1" ] && echo "$state" > "/proc/diag/led/$1"
}

set_state() {
	case "$1" in
		preinit)
			set_led dmz 1
			set_led diag 1
			set_led power 0
		;;
		failsafe)
			set_led diag f
			set_led power f
			set_led dmz f
		;;
		done)
			set_led dmz 0
			set_led diag 0
			set_led power 1
		;;
	esac
}
