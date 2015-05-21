#!/bin/sh
# Copyright (C) 2007-2013 OpenWrt.org

. /lib/functions/leds.sh
. /lib/brcm63xx.sh

set_state() {
	case "$(brcm63xx_board_name)" in
	a4001n1)
		status_led="A4001N1:green:power"
		;;
	a4001n)
		status_led="A4001N:green:power"
		;;
	ar-5381u)
		status_led="AR-5381u:green:power"
		;;
	ar-5387un)
		status_led="AR-5387un:green:power"
		;;
	bcm96348gw)
		status_led="96348GW:green:power"
		;;
	bcm963281tan)
		status_led="963281TAN::power"
		;;
	bcm96328avng)
		status_led="96328avng::power"
		;;
	bcm96348gw-11)
		status_led="96348GW-11:green:power"
		;;
	spw303v)
		status_led="spw303v:green:power+adsl"
		;;
	vr-3025un)
		status_led="VR-3025un:green:power"
		;;
	vr-3025u)
		status_led="VR-3025u:green:power"
		;;
	wap-5813n)
		status_led="WAP-5813n:green:power"
		;;
	ar1004g)
		status_led="AR1004G:green:power"
		;;
	dsl-274xb-c|\
	dsl-274xb-f)
		status_led="dsl-274xb:green:power"
		;;
	dsl-275xb-d)
		status_led="dsl-275xb:green:power"
		;;
	cpva642)
		status_led="CPVA642:green:power:"
		;;
	ct536_ct5621)
		status_led="CT536_CT5621:green:power"
		;;
	cvg834g)
		status_led="CVG834G:green:power"
		;;
	dsl-2640b-b)
		status_led="D-4P-W:green:power"
		;;
	dgnd3700v1_dgnd3800b)
		status_led="DGND3700v1_3800B:green:power"
		;;
	fast2504n)
		status_led="fast2504n:green:ok"
		;;
	fast2704v2)
		status_led="F@ST2704V2:green:power"
		;;
	homehub2a)
		status_led="HOMEHUB2A:green:upgrading"
		status_led2="HOMEHUB2A:blue:upgrading"
		;;
	gw6200)
		status_led="GW6200:green:line1"
		status_led2="GW6200:green:tel"
		;;
	hg553)
		status_led="HW553:blue:power"
		;;
	hg556a_*)
		status_led="HW556:red:power"
		;;
	hg520)
		status_led="HW520:green:net"
		;;
	hg655b)
		status_led="HW65x:green:power"
		;;
	p870hw-51a_v2)
		status_led="P870HW-51a:green:power"
		;;
	rta770bw)
		status_led="RTA770BW:green:diag"
		;;
	rta770w)
		status_led="RTA770W:green:diag"
		;;
	spw500v)
		status_led="SPW500V:green:power"
		;;
	v2110)
		status_led="V2110:power:green"
		;;
	esac

	case "$1" in
	preinit)
		status_led_set_timer 200 200
		;;
	failsafe)
		status_led_set_timer 50 50
		;;
	preinit_regular)
		status_led_set_timer 500 500
		;;
	done)
		if [ "${status_led/power}" != "$status_led" ]; then
			status_led_on
		else
			status_led_off
		fi
		;;
	esac
}
