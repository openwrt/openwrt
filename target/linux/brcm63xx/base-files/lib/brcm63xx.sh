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

brcm63xx_dt_detect() {
	local board_name

	case "$1" in
	"ADB P.DG A4001N")
		board_name="a4001n"
		;;
	"ADB P.DG A4001N1")
		board_name="a4001n1"
		;;
	"Alcatel RG100A")
		board_name="rg100a"
		;;
	"ASMAX AR 1004g")
		board_name="ar100g"
		;;
	"Belkin F5D7633")
		board_name="f5d7633"
		;;
	"Broadcom 96348R reference board")
		board_name="bcm96348r"
		;;
	"Broadcom bcm963281TAN reference board")
		board_name="bcm963281tan"
		;;
	"Broadcom BCM96328avng reference board")
		board_name="bcm6328avng"
		;;
	"Broadcom BCM96345GW2 reference board")
		board_name="bcm96345gw2"
		;;
	"Broadcom BCM96348GW-10 reference board")
		board_name="bcm96348gw-10"
		;;
	"Broadcom BCM96348GW-11 reference board")
		board_name="bcm96348gw-11"
		;;
	"Broadcom BCM96348GW reference board")
		board_name="bcm96358gw"
		;;
	"Broadcom BCM96358VW reference board")
		board_name="bcm96358vw"
		;;
	"Broadcom BCM96358VW2 reference board")
		board_name="bcm96358vw2"
		;;
	"Broadcom BCM96368MVNgr reference board")
		board_name="bcm96368mvngr"
		;;
	"Broadcom BCM96368MVWG reference board")
		board_name="bcm96368mvwg"
		;;
	"BT Voyager V2500V")
		board_name="v2500v"
		;;
	"Comtrend AR-5381u")
		board_name="ar-5381u"
		;;
	"Comtrend AR-5387un")
		board_name="ar-5387un"
		;;
	"Comtrend CT-5365")
		board_name="ct-5365"
		;;
	"Comtrend CT-536+/CT-5621T")
		board_name="ct-536p_5621t"
		;;
	"Comtrend CT-6373")
		board_name="ct-6373"
		;;
	"Comtrend VR-3025u")
		board_name="vr-3025u"
		;;
	"Comtrend VR-3025un")
		board_name="vr-3025un"
		;;
	"Comtrend WAP-5813n")
		board_name="wap-5813n"
		;;
	"Davolink DV-201AMR")
		board_name="dv-201amr"
		;;
	"D-Link DSL-2640B rev B2")
		board_name="dsl-2640b-b"
		;;
	"D-Link DSL-2650U")
		board_name="dsl-2650u"
		;;
	"D-Link DSL-2740B/DSL-2741B rev C2/3")
		board_name="dsl-274xb-c"
		;;
	"D-Link DSL-2740B/DSL-2741B rev F1")
		board_name="dsl-274xb-f"
		;;
	"D-Link DVA-G3810BN/TL")
		board_name="dva-g3810bn"
		;;
	"Dynalink RTA1025W")
		board_name="rta1025w"
		;;
	"Dynalink RTA1320")
		board_name="rta1320"
		;;
	"Huawei EchoLife HG520v")
		board_name="hg520v"
		;;
	"Huawei EchoLife HG553")
		board_name="hg553"
		;;
	"Huawei EchoLife HG556a (version A)")
		board_name="hg556a_a"
		;;
	"Huawei EchoLife HG556a (version B)")
		board_name="hg556a_b"
		;;
	"Huawei EchoLife HG556a (version C)")
		board_name="hg556a_c"
		;;
	"Inventel Livebox 1")
		board_name="livebox1"
		;;
	"Netgear CVG834G")
		board_name="cvg834g"
		;;
	"Netgear DG834GT/PN")
		board_name="dg834gt"
		;;
	"Netgear DGND3700v1/DGND3800B")
		board_name="dgnd3700v1_dgnd3800b"
		;;
	"Pirelli A226G")
		board_name="a226g"
		;;
	"Pirelli A226M")
		board_name="a226m"
		;;
	"Pirelli A226M-FWB")
		board_name="a226m-fwb"
		;;
	"Pirelli Alice Gate AGPF-S0")
		board_name="agpf-s0"
		;;
	"Sagem F@ST2404")
		board_name="fast2404"
		;;
	"Sagem F@ST2504n")
		board_name="fast2504n"
		;;
	"Sagem F@ST2604")
		board_name="fast2604"
		;;
	"Sagem F@ST2704V2")
		board_name="fast2704v2"
		;;
	"SFR Neuf Box 4"*)
		board_name="neufbox4"
		;;
	"SFR neufbox 6 (Sercomm)")
		board_name="neufbox6"
		;;
	"T-Com Speedport W303 V")
		board_name="spw303v"
		;;
	"T-Com Speedport W500 V")
		board_name="spw500v"
		;;
	"TECOM GW6000")
		board_name="g6000"
		;;
	"TECOM GW6200")
		board_name="g6200"
		;;
	"Telsey CPVA642-type (CPA-ZNTE60T)")
		board_name="cpva642"
		;;
	"TP-Link TD-W8900GB")
		board_name="td-w8900gb"
		;;
	"USRobotics 9108")
		board_name="usr9108"
		;;
	"Zyxel P870HW-51a v2")
		board_name="p870hw-51a_v2"
		;;
	*)
		board_name="unknown"
		;;
	esac

	echo "$board_name"
}

brcm63xx_legacy_detect() {
	local board_name

	case "$1" in
	RTA770BW)
		board_name="rta770bw"
		;;
	RTA770W)
		board_name="rta770w"
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

	if [ -e /proc/device-tree ]; then
		model=$(cat /proc/device-tree/model)
		board_name=$(brcm63xx_dt_detect "$model")
	else
		model="Unknown bcm63xx board"
		board_name=$(brcm63xx_legacy_detect "$board_id")
	fi

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
