#!/bin/sh
# Copyright (C) 2007 OpenWrt.org

set_led() {
	local led="$1"
	local state="$2"
	[ -d "/sys/class/leds/rb500led:$led" ] && echo "$state" > "/sys/class/leds/rb500led:$led/brightness"
}

set_state() {
	case "$1" in
		preinit)
			set_led amber 1
		;;
		done)
			set_led amber 0
		;;
	esac
}
