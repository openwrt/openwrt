#!/bin/sh
# Copyright (C) 2010-2013 OpenWrt.org

. /lib/functions/leds.sh
. /lib/ramips.sh

get_status_led() {
	case $(ramips_board_name) in
	3g-6200n)
		status_led="edimax:green:power"
		;;
	3g300m)
		status_led="tenda:blue:ap"
		;;
	argus-atp52b)
		status_led="argus-atp52b:green:run"
		;;
	br6524n)
		status_led="edimax:blue:power"
		;;
	br6425)
		status_led="edimax:green:power"
		;;
	dir-300-b1 | dir-600-b1 | dir-600-b2 | dir-615-h1 | dir-615-d | dir-620-a1)
		status_led="d-link:green:status"
		;;
	dir-645)
		status_led="d-link:green:wps"
		;;
	dap-1350)
		status_led="d-link:blue:power"
		;;
	esr-9753)
		status_led="esr-9753:orange:power"
		;;
	f5d8235-v2)
		status_led="f5d8235v2:blue:router"
		;;
	fonera20n)
		status_led="fonera20n:green:power"
		;;
	rt-n13u)
		status_led="rt-n13u:power"
		;;
	all0239-3g|\
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
	omni-emb|\
	omni-emb-hpm)
		status_led="emb:green:status"
		;;
	psr-680w)
		status_led="psr-680w:red:wan"
		;;
	pwh2004)
		status_led="pwh2004:green:power"
		;;
	rt-n15)
		status_led="rt-n15:blue:power"
		;;
	rt-n10-plus)
		status_led="asus:green:wps"
		;;
	rt-n56u | wl-330n | wl-330n3g)
		status_led="asus:blue:power"
		;;
	sl-r7205)
		status_led="sl-r7205:green:status"
		;;
	tew-691gr|\
	tew-692gr)
		status_led="trendnet:green:wps"
		;;
	v11st-fe)
		status_led="v11st-fe:green:status"
		;;
	v22rw-2x2)
		status_led="v22rw-2x2:green:security"
		;;
	w306r-v20)
		status_led="w306r-v20:green:sys"
		;;
	w502u)
		status_led="alfa:blue:wps"
		;;
	wcr-150gn)
		status_led="wcr150gn:amber:power"
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
	wr512-3gn)
		status_led="wr512:green:wps"
		;;
	mzk-w300nh2)
		status_led="mzkw300nh2:green:power"
		;;
	ur-336un)
		status_led="ur336:green:wps"
		;;
	xdxrn502j)
		status_led="xdxrn502j:green:power"
		;;
	esac
}

set_state() {
	get_status_led

	case "$1" in
	preinit)
		insmod leds-gpio
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
