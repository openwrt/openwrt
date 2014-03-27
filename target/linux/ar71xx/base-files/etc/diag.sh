#!/bin/sh
# Copyright (C) 2009-2013 OpenWrt.org

. /lib/functions/leds.sh
. /lib/ar71xx.sh

get_status_led() {
	case $(ar71xx_board_name) in
	alfa-nx)
		status_led="alfa:green:led_8"
		;;
	all0305)
		status_led="eap7660d:green:ds4"
		;;
	ap132)
		status_led="ap132:green:status"
		;;
	ap136-010|\
	ap136-020)
		status_led="ap136:green:status"
		;;
	ap135-020)
		status_led="ap135:green:status"
		;;
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
	bxu2000n-2-a1)
		status_led="bhu:green:status"
		;;
	cap4200ag)
		status_led="senao:green:pwr"
		;;
	db120)
		status_led="db120:green:status"
		;;
	dir-505-a1 |\
	dir-600-a1 |\
	dir-615-e4)
		status_led="d-link:green:power"
		;;
	dir-615-c1)
		status_led="d-link:green:status"
		;;
	dir-825-b1)
		status_led="d-link:orange:power"
		;;
	dir-825-c1 |\
	dir-835-a1)
		status_led="d-link:amber:power"
		;;
	eap7660d)
		status_led="eap7660d:green:ds4"
		;;
	hornet-ub)
		status_led="alfa:blue:wps"
		;;
	ja76pf | \
	ja76pf2)
		status_led="jjplus:green:led1"
		;;
	ls-sr71)
		status_led="ubnt:green:d22"
		;;
	mr600)
		status_led="mr600:orange:power"
		;;
	mr600v2)
		status_led="mr600:blue:power"
		;;
	mynet-n600 | \
	mynet-n750)
		status_led="wd:blue:power"
		;;
	mynet-rext)
		status_led="wd:blue:power"
		;;
	mzk-w04nu | \
	mzk-w300nh)
		status_led="planex:green:status"
		;;
	nbg460n_550n_550nh)
		status_led="nbg460n:green:power"
		;;
	om2p | \
	om2p-hs | \
	om2p-lc)
		status_led="om2p:blue:power"
		;;
	pb44)
		status_led="pb44:amber:jump1"
		;;
	rb-2011l|\
	rb-2011uas|\
	rb-2011uas-2hnd)
		status_led="rb:green:usr"
		;;
	rb-411 | rb-411u | rb-433 | rb-433u | rb-450 | rb-450g | rb-493)
		status_led="rb4xx:yellow:user"
		;;
	rb-750)
		status_led="rb750:green:act"
		;;
	rb-911g-2hpnd|\
	rb-911g-5hpnd|\
	rb-912uag-2hpnd|\
	rb-912uag-5hpnd)
		status_led="rb:green:user"
		;;
	routerstation | routerstation-pro)
		status_led="ubnt:green:rf"
		;;
	rw2458n)
		status_led="rw2458n:green:d3"
		;;
	oolite)
		status_led="oolite:red:system"
		;;
	tew-632brp)
		status_led="tew-632brp:green:status"
		;;
	tew-673gru)
		status_led="trendnet:blue:wps"
		;;
	tew-712br|\
	tew-732br)
		status_led="trendnet:green:power"
		;;
	tl-mr3020)
		status_led="tp-link:green:wps"
		;;
	tl-wa750re)
		status_led="tp-link:orange:re"
		;;
	tl-wa850re)
		status_led="tp-link:blue:re"
		;;
	tl-mr3220 | \
	tl-mr3220-v2 | \
	tl-mr3420 | \
	tl-mr3420-v2 | \
	tl-wa801nd-v2 | \
	tl-wa901nd | \
	tl-wa901nd-v2 | \
	tl-wa901nd-v3 | \
	tl-wdr3500 | \
	tl-wr1041n-v2 | \
	tl-wr1043nd | \
	tl-wr1043nd-v2 | \
	tl-wr741nd | \
	tl-wr741nd-v4 | \
	tl-wr841n-v1 | \
	tl-wr841n-v7 | \
	tl-wr841n-v8 | \
	tl-wr842n-v2 | \
	tl-wr941nd)
		status_led="tp-link:green:system"
		;;
	archer-c7 | \
	tl-wdr4900-v2 | \
	tl-mr10u | \
	tl-mr13u | \
	tl-wdr4300 | \
	tl-wr703n | \
	tl-wr710n | \
	tl-wr720n-v3)
		status_led="tp-link:blue:system"
		;;
	tl-wr2543n)
		status_led="tp-link:green:wps"
		;;
	unifi)
		status_led="ubnt:green:dome"
		;;
	uap-pro)
		status_led="ubnt:white:dome"
		;;
	whr-g301n | \
	whr-hp-g300n | \
	whr-hp-gn | \
	wzr-hp-g300nh)
		status_led="buffalo:green:router"
		;;
	wlae-ag300n)
		status_led="buffalo:green:status"
		;;
	wzr-hp-ag300h | \
	wzr-hp-g300nh2)
		status_led="buffalo:red:diag"
		;;
	wndap360 | \
	wndr3700 | \
	wndr4300 | \
	wnr2000 | \
	wnr2200 |\
	wnr612-v2)
		status_led="netgear:green:power"
		;;
	wp543)
		status_led="wp543:green:diag"
		;;
	wrt400n)
		status_led="wrt400n:blue:wps"
		;;
	wrt160nl)
		status_led="wrt160nl:blue:wps"
		;;
	zcn-1523h-2 | zcn-1523h-5)
		status_led="zcn-1523h:amber:init"
		;;
	wlr8100)
		status_led="sitecom:amber:status"
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
	done)
		status_led_on
		;;
	esac
}
