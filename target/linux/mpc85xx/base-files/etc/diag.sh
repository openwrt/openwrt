#!/bin/sh
# Copyright (C) 2013 OpenWrt.org

. /lib/functions/leds.sh
. /lib/mpc85xx.sh

get_status_led() {
        case $(mpc85xx_board_name) in
        tl-wdr4900-v1)
                status_led="tp-link:blue:system"
                ;;
        esac
}

set_state() {
	get_status_led

	case "$1" in
	preinit)
		insmod leds-gpio
		insmod ledtrig-default-on
		insmod ledtrig-timer

		status_led_blink_preinit
		;;

	failsafe)
		status_led_blink_failsafe
		;;

	done)
		status_led_on
		;;
	esac
}
