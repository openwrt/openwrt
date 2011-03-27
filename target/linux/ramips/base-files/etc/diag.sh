#!/bin/sh
#
# Copyright (C) 2010 OpenWrt.org
#
#

. /lib/ramips.sh

status_led=""

led_set_attr() {
	[ -f "/sys/class/leds/$1/$2" ] && echo "$3" > "/sys/class/leds/$1/$2"
}

status_led_set_timer() {
	led_set_attr $status_led "trigger" "timer"
	led_set_attr $status_led "delay_on" "$1"
	led_set_attr $status_led "delay_off" "$2"
}

status_led_on() {
	led_set_attr $status_led "trigger" "none"
	led_set_attr $status_led "brightness" 255
}

status_led_off() {
	led_set_attr $status_led "trigger" "none"
	led_set_attr $status_led "brightness" 0
}

get_status_led() {
	case $(ramips_board_name) in
	dir-300-b1)
		status_led="dir-300b:green:status"
		;;
	fonera20n)
		status_led="fonera20n:green:power"
		;;
	v22rw-2x2)
		status_led="v22rw-2x2:green:security"
		;;
	whr-g300n)
		status_led="whr-g300n:red:diag"
		;;
	hw550-3g)
		status_led="hw550-3g:green:status"
		;;
	esac
}

set_state() {
	get_status_led

	case "$1" in
	preinit)
		insmod leds-gpio
		status_led_set_timer 200 200
		;;
	failsafe)
		status_led_set_timer 50 50
		;;
	done)
		status_led_on
		;;
	esac
}
