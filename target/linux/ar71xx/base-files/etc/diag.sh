#!/bin/sh
#
# Copyright (C) 2009 OpenWrt.org
#
#

. /lib/ar71xx.sh

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
	case $(ar71xx_board_name) in
	ap81)
		status_led="ap81:green:status"
		;;
	ap83)
		status_led="ap83:green:power"
		;;
	aw-nr580)
		status_led="aw-nr580:green:ready"
		;;
	bullet-m | rocket-m | nano-m)
		status_led="ubnt:green:link4"
		;;
	dir-825-b1)
		status_led="dir825b1:orange:power"
		;;
	ls-sr71)
		status_led="ubnt:green:d22"
		;;
	mzk-w04nu)
		status_led="mzk-w04nu:green:status"
		;;
	mzk-w300nh)
		status_led="mzk-w300nh:green:status"
		;;
	pb44)
		status_led="pb44:amber:jump1"
		;;
	routerstation | routerstation-pro)
		status_led="ubnt:green:rf"
		;;
	tew-632brp)
		status_led="tew-632brp:green:status"
		;;
	tl-wr1043nd)
		status_led="tl-wr1043nd:green:system"
		;;
	tl-wr741nd)
		status_led="tl-wr741nd:green:system"
		;;
	tl-wr941nd)
		status_led="tl-wr941nd:green:system"
		;;
	wndr3700)
		status_led="wndr3700:green:power"
		;;
	wnr2000)
		status_led="wnr2000:green:power"
		;;
	wp543)
		status_led="wp543:green:diag"
		;;
	wrt400n)
		status_led="wrt400n:blue:status"
		;;
	wrt160nl)
		status_led="wrt160nl:blue:wps"
		;;
	esac;
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
