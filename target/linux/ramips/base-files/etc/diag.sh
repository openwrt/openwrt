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
	dir-300-b1 | dir-600-b1 | dir-600-b2)
		status_led="d-link:green:status"
		;;
	esr-9753)
		status_led="esr-9753:orange:power"
		;;
	fonera20n)
		status_led="fonera20n:green:power"
		;;
	hw550-3g)
		status_led="hw550-3g:green:status"
		;;
	mofi3500-3gn)
		status_led="mofi3500-3gn:green:status"
		;;
	nbg-419n)
		status_led="nbg-419n:green:power"
		;;
	nw718)
		status_led="nw718:amber:cpu"
		;;
	omni-emb)
		status_led="emb:green:status"
		;;
	pwh2004)
		status_led="pwh2004:green:power"
		;;
	rt-n15)
		status_led="rt-n15:blue:power"
		;;
	v22rw-2x2)
		status_led="v22rw-2x2:green:security"
		;;
	w502u)
		status_led="alfa:blue:wps"
		;;
	whr-g300n)
		status_led="whr-g300n:green:router"
		;;
	wli-tx4-ag300n)
		status_led="buffalo:blue:power"
		;;
	wl-351)
		status_led="wl-351:amber:power"
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
