#!/bin/sh /etc/rc.common
# Copyright (C) 2012-2014 OpenWrt.org

if [ "$( which vdsl_cpe_control )" ]; then
	XDSL_CTRL=vdsl_cpe_control
else
	XDSL_CTRL=dsl_cpe_control
fi

#
# Basic functions to send CLI commands to the vdsl_cpe_control daemon
#
dsl_cmd() {
	killall -0 ${XDSL_CTRL} && (
		echo "$@" > /tmp/pipe/dsl_cpe0_cmd
		cat /tmp/pipe/dsl_cpe0_ack
	)
}
dsl_val() {
	echo $(expr "$1" : '.*'$2'=\([-\.[:alnum:]]*\).*')
}

#
# Simple divide by 10 routine to cope with one decimal place
#
dbt() {
	local a=$(expr $1 / 10)
	local b=$(expr $1 % 10)
	echo "${a}.${b}"
}
#
# Take a number and convert to k or meg
#
scale() {
	local val=$1
	local a
	local b

	if [ "$val" -gt 1000000 ]; then
		a=$(expr $val / 1000)
		b=$(expr $a % 1000)
		a=$(expr $a / 1000)
		printf "%d.%03d Mb" ${a} ${b}
	elif [ "$val" -gt 1000 ]; then
		a=$(expr $val / 1000)
		printf "%d Kb" ${a}
	else
		echo "${val} b"
	fi
}

#
# Read the data rates for both directions
#
data_rates() {
	local csg
	local dru
	local drd
	local sdru
	local sdrd

	csg=$(dsl_cmd g997csg 0 1)
	drd=$(dsl_val "$csg" ActualDataRate)

	csg=$(dsl_cmd g997csg 0 0)
	dru=$(dsl_val "$csg" ActualDataRate)

	[ -z "$drd" ] && drd=0
	[ -z "$dru" ] && dru=0

	sdrd=$(scale $drd)
	sdru=$(scale $dru)

	if [ "$action" = "lucistat" ]; then
		echo "dsl.data_rate_down=$drd"
		echo "dsl.data_rate_up=$dru"
		echo "dsl.data_rate_down_s=\"$sdrd\""
		echo "dsl.data_rate_up_s=\"$sdru\""
	else
		echo "Data Rate:		${sdrd}/s / ${sdru}/s"
	fi
}

#
# Chipset
#
chipset() {
	local vig
	local cs
	local csv

	vig=$(dsl_cmd vig)
	cs=$(dsl_val "$vig" DSL_ChipSetType)
	csv=$(dsl_val "$vig" DSL_ChipSetHWVersion)

	if [ "$action" = "lucistat" ]; then
		echo "dsl.chipset=\"${cs} ${csv}\""
	else
		echo "Chipset:		${cs} ${csv}"
	fi
}

#
# Work out how long the line has been up
#
line_uptime() {
	local ccsg
	local et
	local etr
	local d
	local h
	local m
	local s
	local rc=""

	ccsg=$(dsl_cmd pmccsg 0 0 0)
	et=$(dsl_val "$ccsg" nElapsedTime)

	[ -z "$et" ] && et=0

	if [ "$action" = "lucistat" ]; then
		echo "dsl.line_uptime=${et}"
		return
	fi

	d=$(expr $et / 86400)
	etr=$(expr $et % 86400)
	h=$(expr $etr / 3600)
	etr=$(expr $etr % 3600)
	m=$(expr $etr / 60)
	s=$(expr $etr % 60)


	[ "${d}${h}${m}${s}" -ne 0 ] && rc="${s}s"
	[ "${d}${h}${m}" -ne 0 ] && rc="${m}m ${rc}"
	[ "${d}${h}" -ne 0 ] && rc="${h}h ${rc}"
	[ "${d}" -ne 0 ] && rc="${d}d ${rc}"

	[ -z "$rc" ] && rc="down"
	echo "Line Uptime:		${rc}"
}

#
# Get noise and attenuation figures
#
line_data() {
	local lsg
	local latnu
	local latnd
	local snru
	local snrd

	lsg=$(dsl_cmd g997lsg 1 1)
	latnd=$(dsl_val "$lsg" LATN)
	snrd=$(dsl_val "$lsg" SNR)

	lsg=$(dsl_cmd g997lsg 0 1)
	latnu=$(dsl_val "$lsg" LATN)
	snru=$(dsl_val "$lsg" SNR)

	[ -z "$latnd" ] && latnd=0
	[ -z "$latnu" ] && latnu=0
	[ -z "$snrd" ] && snrd=0
	[ -z "$snru" ] && snru=0

	latnd=$(dbt $latnd)
	latnu=$(dbt $latnu)
	snrd=$(dbt $snrd)
	snru=$(dbt $snru)
	
	if [ "$action" = "lucistat" ]; then
		echo "dsl.line_attenuation_down=$latnd"
		echo "dsl.line_attenuation_up=$latnu"
		echo "dsl.noise_margin_down=$snrd"
		echo "dsl.noise_margin_up=$snru"
	else
		echo "Line Attenuation:	${latnd}dB / ${latnu}dB"
		echo "Noise Margin:		${snrd}dB / ${snru}dB"
	fi
}

#
# Is the line up? Or what state is it in?
#
line_state() {
	local lsg=$(dsl_cmd lsg)
	local ls=$(dsl_val "$lsg" nLineState);
	local s;

	case "$ls" in
		"0x0")		s="not initialized" ;;
		"0x1")		s="exception" ;;
		"0x10")		s="not updated" ;;
		"0xff")		s="idle request" ;;
		"0x100")	s="idle" ;;
		"0x1ff")	s="silent request" ;;
		"0x200")	s="silent" ;;
		"0x300")	s="handshake" ;;
		"0x380")	s="full_init" ;;
		"0x400")	s="discovery" ;;
		"0x500")	s="training" ;;
		"0x600")	s="analysis" ;;
		"0x700")	s="exchange" ;;
		"0x800")	s="showtime_no_sync" ;;
		"0x801")	s="showtime_tc_sync" ;;
		"0x900")	s="fastretrain" ;;
		"0xa00")	s="lowpower_l2" ;;
		"0xb00")	s="loopdiagnostic active" ;;
		"0xb10")	s="loopdiagnostic data exchange" ;;
		"0xb20")	s="loopdiagnostic data request" ;;
		"0xc00")	s="loopdiagnostic complete" ;;
		"0x1000000")	s="test" ;;
		"0xd00")	s="resync" ;;
		"0x3c0")	s="short init entry" ;;
		"")		s="not running daemon"; ls="0xfff" ;;
		*)		s="unknown" ;;
	esac

	if [ "$action" = "lucistat" ]; then
		echo "dsl.line_state_num=$ls"
		echo "dsl.line_state_detail=\"$s\""
		if [ "$ls" = "0x801" ]; then
			echo "dsl.line_state=\"UP\""
		else
			echo "dsl.line_state=\"DOWN\""
		fi
	else
		if [ "$ls" = "0x801" ]; then
			echo "Line State:		UP [$ls: $s]"
		else
			echo "Line State:		DOWN [$ls: $s]"
		fi
	fi
}

status() {
	chipset
	line_state
	data_rates
	line_data
	line_uptime
}

lucistat() {
	echo "local dsl={}"
	status
	echo "return dsl"
}
