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
	antminer-s1)
		status_led="antminer-s1:green:system"
		;;
	antminer-s3)
		status_led="antminer-s3:green:system"
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
	bsb)
		status_led="bsb:red:sys"
		;;
	bullet-m | rocket-m | rocket-m-xw | nano-m | nanostation-m | nanostation-m-xw | loco-m-xw)
		status_led="ubnt:green:link4"
		;;
	bxu2000n-2-a1)
		status_led="bhu:green:status"
		;;
	cap4200ag)
		status_led="senao:green:pwr"
		;;
	cpe510)
		status_led="tp-link:green:link4"
		;;
	db120)
		status_led="db120:green:status"
		;;
	dgl-5500-a1 |\
	dhp-1565-a1|\
	dir-505-a1 |\
	dir-600-a1 |\
	dir-615-e1 |\
	dir-615-i1 |\
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
	dlan-pro-500-wp)
		status_led="devolo:green:wlan-2g"
		;;
	dlan-pro-1200-ac)
		status_led="devolo:status:wlan"
		;;
	dragino2)
		status_led="dragino2:red:system"
		;;
	eap300v2)
		status_led="engenius:blue:power"
		;;
	eap7660d)
		status_led="eap7660d:green:ds4"
		;;
	el-mini | \
	el-m150)
		status_led="easylink:green:system"
		;;
	ew-dorin | ew-dorin-router)
		status_led="dorin:green:status"
		;;
	f9k1115v2)
		status_led="belkin:blue:status"
		;;
	gl-inet)
		status_led="gl-connect:green:lan"
		;;
	epg5000)
		status_led="epg5000:amber:power"
		;;
	esr1750)
		status_led="esr1750:amber:power"
		;;
	esr900)
		status_led="engenius:amber:power"
		;;
	hiwifi-hc6361)
		status_led="hiwifi:blue:system"
		;;
	hornet-ub | \
	hornet-ub-x2)
		status_led="alfa:blue:wps"
		;;
	ja76pf | \
	ja76pf2)
		status_led="jjplus:green:led1"
		;;
	ls-sr71)
		status_led="ubnt:green:d22"
		;;
	mc-mac1200r)
		status_led="mercury:green:system"
		;;
	mr12)
		status_led="mr12:green:power"
		;;
	mr16)
		status_led="mr16:green:power"
		;;
	mr600)
		status_led="mr600:orange:power"
		;;
	mr600v2)
		status_led="mr600:blue:power"
		;;
	mr900 | \
	mr900v2)
		status_led="mr900:blue:power"
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
	nbg6716)
		status_led="zyxel:white:power"
		;;
	om2p | \
	om2pv2 | \
	om2p-hs | \
	om2p-hsv2 | \
	om2p-lc)
		status_led="om2p:blue:power"
		;;
	om5p | \
	om5p-an)
		status_led="om5p:blue:power"
		;;
	onion-omega)
		status_led="onion:amber:system"
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
	rb-911g-5hpacd|\
	rb-911g-5hpnd|\
	rb-912uag-2hpnd|\
	rb-912uag-5hpnd)
		status_led="rb:green:user"
		;;
	rb-951ui-2hnd)
		status_led="rb:green:act"
		;;
	rb-sxt2n|\
	rb-sxt5n)
		status_led="rb:green:power"
		;;
	routerstation | routerstation-pro)
		status_led="ubnt:green:rf"
		;;
	rw2458n)
		status_led="rw2458n:green:d3"
		;;
	smart-300)
		status_led="nc-link:green:system"
		;;
	minibox-v1)
		status_led="minibox-v1:green:system"
		;;
	oolite)
		status_led="oolite:red:system"
		;;
	qihoo-c301)
		status_led="qihoo:green:status"
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
	tl-wa860re)
		status_led="tp-link:green:power"
		;;
	tl-mr3220 | \
	tl-mr3220-v2 | \
	tl-mr3420 | \
	tl-mr3420-v2 | \
	tl-wa701nd-v2 | \
	tl-wa801nd-v2 | \
	tl-wa901nd | \
	tl-wa901nd-v2 | \
	tl-wa901nd-v3 | \
	tl-wa901nd-v4 | \
	tl-wdr3500 | \
	tl-wr1041n-v2 | \
	tl-wr1043nd | \
	tl-wr1043nd-v2 | \
	tl-wr741nd | \
	tl-wr741nd-v4 | \
	tl-wr841n-v1 | \
	tl-wr841n-v7 | \
	tl-wr841n-v8 | \
	tl-wa830re-v2 | \
	tl-wr842n-v2 | \
	tl-wr941nd | \
	tl-wr941nd-v5)
		status_led="tp-link:green:system"
		;;
	archer-c5 | \
	archer-c7 | \
	tl-wdr4900-v2 | \
	tl-mr10u | \
	tl-mr12u | \
	tl-mr13u | \
	tl-wdr4300 | \
	tl-wr703n | \
	tl-wr710n | \
	tl-wr720n-v3 | \
	tl-wr941nd-v6)
		status_led="tp-link:blue:system"
		;;
	tl-wr841n-v9)
		status_led="tp-link:green:qss"
		;;
	tl-wr2543n)
		status_led="tp-link:green:wps"
		;;
	tube2h)
		status_led="alfa:green:signal4"
		;;
	unifi)
		status_led="ubnt:green:dome"
		;;
	uap-pro)
		status_led="ubnt:white:dome"
		;;
	unifi-outdoor-plus)
		status_led="ubnt:white:front"
		;;
	airgateway)
		status_led="ubnt:white:status"
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
	r6100 | \
	wndap360 | \
	wndr3700 | \
	wndr3700v4 | \
	wndr4300 | \
	wnr2000 | \
	wnr2200 |\
	wnr612-v2 |\
	wnr1000-v2 |\
	wpn824n)
		status_led="netgear:green:power"
		;;
	wp543)
		status_led="wp543:green:diag"
		;;
	wpj344)
		status_led="wpj344:green:status"
		;;
	wpj531)
		status_led="wpj531:green:sig3"
		;;
	wpj558)
		status_led="wpj558:green:sig3"
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
	preinit_regular)
		status_led_blink_preinit_regular
		;;
	done)
		status_led_on
		case $(ar71xx_board_name) in
		qihoo-c301)
			local n=$(fw_printenv activeregion | cut -d = -f 2)
			fw_setenv "image${n}trynum" 0
			;;
		esac
		;;
	esac
}
