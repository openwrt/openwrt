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
	ap96)
		status_led="ap96:green:led2"
		;;
	aw-nr580)
		status_led="aw-nr580:green:ready"
		;;
	bullet-m | rocket-m | nano-m | nanostation-m)
		status_led="ubnt:green:link4"
		;;
	dir-600-a1)
		status_led="dir-600-a1:green:power"
		;;
	dir-615-c1)
		status_led="dir-615c1:green:status"
		;;
	dir-825-b1)
		status_led="dir825b1:orange:power"
		;;
	eap7660d)
		status_led="eap7660d:green:ds4"
		;;
	ja76pf)
		status_led="ja76pf:green:led1"
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
	nbg460n_550n_550nh)
		status_led="nbg460n:green:power"
		;;
	pb44)
		status_led="pb44:amber:jump1"
		;;
	rb-411 | rb-411u | rb-433 | rb-433u | rb-450 | rb-450g | rb-493)
		status_led="rb4xx:yellow:user"
		;;
	routerstation | routerstation-pro)
		status_led="ubnt:green:rf"
		;;
	tew-632brp)
		status_led="tew-632brp:green:status"
		;;
	tl-mr3220 | tl-mr3420)
		status_led="tl-mr3x20:green:system"
		;;
	tl-wa901nd)
		status_led="tl-wa901nd:green:system"
		;;
	tl-wa901nd-v2)
		status_led="tl-wa901nd-v2:green:system"
		;;
	tl-wr1043nd)
		status_led="tl-wr1043nd:green:system"
		;;
	tl-wr741nd)
		status_led="tl-wr741nd:green:system"
		;;
	tl-wr841n-v1)
		status_led="tl-wr841n:green:system"
		;;
	tl-wr941nd)
		status_led="tl-wr941nd:green:system"
		;;
	unifi)
		status_led="ubnt:green:dome"
		;;
	wndr3700 | wndr3700v2)
		status_led="wndr3700:green:power"
		;;
	wnr2000)
		status_led="wnr2000:green:power"
		;;
	wp543)
		status_led="wp543:green:diag"
		;;
	wrt400n)
		status_led="wrt400n:green:status"
		;;
	wrt160nl)
		status_led="wrt160nl:blue:wps"
		;;
	wzr-hp-g300nh | wzr-hp-g301nh)
		status_led="wzr-hp-g300nh:red:diag"
		;;
	zcn-1523h-2 | zcn-1523h-5)
		status_led="zcn-1523h:amber:init"
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
		status_led_off
		;;
	esac
}
