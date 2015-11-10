#!/bin/sh
# Copyright (C) 2014 OpenWrt.org

. /lib/functions/leds.sh
. /lib/kirkwood.sh

get_status_led() {
	case $(kirkwood_board_name) in
	dockstar|\
	goflexhome|\
	goflexnet|\
	pogo_e02)
		status_led="status:orange:fault"
		;;
	linksys-audi)
		status_led="audi:green:power"
		;;
	linksys-viper)
		status_led="viper:white:health"
		;;
	esac
}

set_state() {
	get_status_led

	case "$1" in
	preinit)
		status_led_blink_preinit
		;;
	failsafe)
		status_led_blink_failsafe
		;;
	preinit_regular)
		status_led_blink_preinit_regular
		;;
	done)
		status_led_on
		;;
	esac
}
