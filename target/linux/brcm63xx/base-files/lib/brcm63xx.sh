#!/bin/sh
#
# Copyright (C) 2007 OpenWrt.org
#
#

board_id=""
status_led=""
status_led2=""
sys_mtd_part=""
brcm63xx_has_reset_button=""
ifname=""

brcm63xx_legacy_detect() {
	local board_name

	case "$1" in
	963281TAN)
		board_name="bcm963281tan"
		;;
	963281T_TEF)
		board_name="a4001n1"
		;;
	96328avng)
		board_name="bcm96328avng"
		;;
	96328dg2x2)
		board_name="a4401n"
		;;
	96328A-1241N)
		board_name="ar-5381u"
		;;
	96328A-1441N1)
		board_name="ar-5387un"
		;;
	96348GW)
		board_name="bcm96348gw"
		;;
	96348GW-11)
		board_name="bcm96348gw-11"
		;;
	96358-502V)
		board_name="spw303v"
		;;
	96368M-1341N)
		board_name="vr-3025un"
		;;
	96368M-1541N)
		board_name="vr-3025u"
		;;
	96369R-1231N)
		board_name="wap-5813n"
		;;
	AR1004G)
		board_name="ar1004g"
		;;
	AW4139 |\
	AW4339U)
		board_name="dsl-274xb-c"
		;;
	AW5200B)
		board_name="dsl-275xb-d"
		;;
	CPVA642)
		board_name="cpva642"
		;;
	CT536_CT5621)
		board_name="ct536_ct5621"
		;;
	CVG834G_E15R3921)
		board_name="cvg834g"
		;;
	D-4P-W)
		board_name="dsl-2640b-b"
		;;
	DGND3700v1_3800B)
		board_name="dgnd3700v1_dgnd3800b"
		;;
	"F@ST2504n")
		board_name="fast2504n"
		;;
	'F@ST2704V2')
		board_name="fast2704v2"
		;;
	GW6000)
		board_name="gw6000"
		;;
	GW6200)
		board_name="gw6200"
		;;
	HW553)
		board_name="hg553"
		;;
	HW556_A)
		board_name="hg556a_a"
		;;
	HW556_B)
		board_name="hg556a_b"
		;;
	HW556_C)
		board_name="hg556a_c"
		;;
	HW6358GW_B)
		board_name="hg620"
		;;
	NB6)
		board_name="neufbox6"
		;;
	P870HW-51a_v2)
		board_name="p870hw-51a_v2"
		;;
	RTA770BW)
		board_name="rta770bw"
		;;
	RTA770W)
		board_name="rta770w"
		;;
	SPW500V)
		board_name="spw500v"
		;;
	V2110)
		board_name="v2110"
		;;
	*)
		board_name="unknown"
		;;
	esac

	echo "$board_name"
}

brcm63xx_detect() {
	local board_name model

	board_id=$(awk 'BEGIN{FS="[ \t:/]+"} /system type/ {print $4}' /proc/cpuinfo)

	if [ "$board_id" = "96358VW" ] && [ -n "$(swconfig dev eth1 help 2>/dev/null)" ]; then
		board_id="DVAG3810BN"
	fi

	if [ -e /proc/device-tree ]; then
		model=$(cat /proc/device-tree/model)
	else
		model="Unknown bcm63xx board"
	fi

	board_name=$(brcm63xx_legacy_detect "$board_id")

	case "$board_name" in
	bcm963281tan)
		status_led="963281TAN::power"
		ifname=eth0
		;;
	a4001n1)
		brcm63xx_has_reset_button="true"
		status_led="A4001N1:green:power"
		ifname=eth0
		;;
	bcm96328avng)
		status_led="96328avng::power"
		ifname=eth0
		;;
	a4001n)
		brcm63xx_has_reset_button="true"
		status_led="A4001N:green:power"
		ifname="eth0"
		;;
	ar-5381u)
		brcm63xx_has_reset_button="true"
		status_led="AR-5381u:green:power"
		ifname=eth0
		;;
	ar-5387un)
		brcm63xx_has_reset_button="true"
		status_led="AR-5387un:green:power"
		ifname=eth0
		;;
	bcm96348gw)
		status_led="96348GW:green:power"
		brcm63xx_has_reset_button="true"
		ifname=eth1
		;;
	bcm96348gw-11)
		status_led="96348GW-11:green:power"
		brcm63xx_has_reset_button="true"
		ifname=eth1
		;;
	spw303v)
		status_led="spw303v:green:power+adsl"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	vr-3025un)
		brcm63xx_has_reset_button="true"
		status_led="VR-3025un:green:power"
		ifname="eth0"
		;;
	vr-3025u)
		brcm63xx_has_reset_button="true"
		status_led="VR-3025u:green:power"
		ifname="eth0"
		;;
	wap-5813n)
		brcm63xx_has_reset_button="true"
		status_led="WAP-5813n:green:power"
		ifname="eth0"
		;;
	ar1004g)
		status_led="AR1004G:green:power"
		brcm63xx_has_reset_button="true"
		;;
	dsl-274xb-c)
		status_led="dsl-274xb:green:power"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	dsl-275xb-d)
		status_led="dsl-275xb:green:power"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	cpva642)
		status_led="CPVA642:green:power:"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	ct536_ct5621)
		status_led="CT536_CT5621:green:power"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	cvg834g)
		status_led="CVG834G:green:power"
		ifname=eth0
		;;
	dsl-2640b-b)
		status_led="D-4P-W:green:power"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	dgnd3700v1_dgnd3800b)
		status_led="DGND3700v1_3800B:green:power"
		brcm63xx_has_reset_button="true"
		ifname="eth0"
		;;
	fast2504n)
		status_led="fast2504n:green:ok"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	fast2704v2)
		status_led="F@ST2704V2:green:power"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	gw6000)
		brcm63xx_has_reset_button="true"
		ifname=eth1
		;;
	gw6200)
		status_led="GW6200:green:line1"
		status_led2="GW6200:green:tel"
		brcm63xx_has_reset_button="true"
		ifname=eth1
		;;
	hg553)
		status_led="HW553:blue:power"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	hg556a_*)
		status_led="HW556:red:power"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	hg520)
		status_led="HW520:green:net"
		brcm63xx_has_reset_button="true"
		ifname="eth0"
		;;
	neufbox6)
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	p870hw-51a_v2)
		brcm63xx_has_reset_button="true"
		status_led="P870HW-51a:green:power"
		ifname="eth0"
		;;
	rta770bw)
		brcm63xx_has_reset_button="true"
		status_led="RTA770BW:green:diag"
		ifname=eth0
		;;
	rta770w)
		brcm63xx_has_reset_button="true"
		status_led="RTA770W:green:diag"
		ifname=eth0
		;;
	spw500v)
		status_led="SPW500V:green:power"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	v2110)
		status_led="V2110:power:green"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	esac

	[ -e "/tmp/sysinfo" ] || mkdir -p "/tmp/sysinfo"

	echo "$board_name" > /tmp/sysinfo/board_name
	echo "$model" > /tmp/sysinfo/model
}

brcm63xx_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -n "$name" ] || name="unknown"

	echo $name
}

brcm63xx_detect
