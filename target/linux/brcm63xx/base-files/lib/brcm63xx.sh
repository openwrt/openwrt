#!/bin/sh
#
# Copyright (C) 2007 OpenWrt.org
#
#

board_id=""
sys_mtd_part=""
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
	963268BU_P300)
		board_name="bcm963268bu_p300"
		;;
	96338W2_E7T)
		board_name="dsl-2640u"
		;;
	96348W3)
		board_name="dg834g_v4"
		;;
	CPVA502+)
		board_name="cpva502p"
		;;
	MAGIC)
		board_name="magic"
		;;
	RTA770BW)
		board_name="rta770bw"
		;;
	RTA770W)
		board_name="rta770w"
		;;
	V2110)
		board_name="v2110"
		;;
	V2500V_BB)
		board_name="v2500v_bb"
		;;
	VW6339GU)
		board_namge="vg50"
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
		ifname=eth0
		;;
	a4001n1)
		ifname=eth0
		;;
	bcm96328avng)
		ifname=eth0
		;;
	a4001n)
		ifname="eth0"
		;;
	ar-5381u)
		ifname=eth0
		;;
	ar-5387un)
		ifname=eth0
		;;
	bcm96348gw)
		ifname=eth1
		;;
	bcm96348gw-11)
		ifname=eth1
		;;
	spw303v)
		ifname=eth0
		;;
	vr-3025un)
		ifname="eth0"
		;;
	vr-3025u)
		ifname="eth0"
		;;
	wap-5813n)
		ifname="eth0"
		;;
	dsl-274xb-c)
		ifname=eth0
		;;
	dsl-275xb-d)
		ifname=eth0
		;;
	cpva642)
		ifname=eth0
		;;
	ct536_ct5621)
		ifname=eth0
		;;
	cvg834g)
		ifname=eth0
		;;
	dsl-2640b-b)
		ifname=eth0
		;;
	dgnd3700v1_dgnd3800b)
		ifname="eth0"
		;;
	fast2504n)
		ifname=eth0
		;;
	fast2704v2)
		ifname=eth0
		;;
	gw6000)
		ifname=eth1
		;;
	gw6200)
		ifname=eth1
		;;
	hg553)
		ifname=eth0
		;;
	hg556a_*)
		ifname=eth0
		;;
	hg520)
		ifname="eth0"
		;;
	neufbox6)
		ifname=eth0
		;;
	p870hw-51a_v2)
		ifname="eth0"
		;;
	rta770bw)
		ifname=eth0
		;;
	rta770w)
		ifname=eth0
		;;
	spw500v)
		ifname=eth0
		;;
	v2110)
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
