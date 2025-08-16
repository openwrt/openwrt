#!/bin/sh
#
# Copyright (C) 2011-2012 OpenWrt.org
#

_ubootenv_add_uci_config() {
	local cfgtype=$1
	local dev=$2
	local offset=$3
	local envsize=$4
	local secsize=$5
	local numsec=$6
	uci batch <<EOF
add ubootenv $cfgtype
set ubootenv.@$cfgtype[-1].dev='$dev'
set ubootenv.@$cfgtype[-1].offset='$offset'
set ubootenv.@$cfgtype[-1].envsize='$envsize'
set ubootenv.@$cfgtype[-1].secsize='$secsize'
set ubootenv.@$cfgtype[-1].numsec='$numsec'
EOF
	uci commit ubootenv
}

ubootenv_add_uci_config() {
	_ubootenv_add_uci_config "ubootenv" "$@"
}

ubootenv_add_uci_sys_config() {
	_ubootenv_add_uci_config "ubootsys" "$@"
}

ubootenv_add_app_config() {
	local cfgtype
	local dev
	local offset
	local envsize
	local secsize
	local numsec
	config_get cfgtype "$1" TYPE
	config_get dev "$1" dev
	config_get offset "$1" offset
	config_get envsize "$1" envsize
	config_get secsize "$1" secsize
	config_get numsec "$1" numsec
	grep -q "^[[:space:]]*${dev}[[:space:]]*${offset}" "/etc/fw_${cfgtype#uboot}.config" || echo "$dev $offset $envsize $secsize $numsec" >>"/etc/fw_${cfgtype#uboot}.config"
}

ubootenv_add_mtd() {
	local idx="$(find_mtd_index "${1}")"
	[ -n "$idx" ] && \
		ubootenv_add_uci_config "/dev/mtd$idx" "${2}" "${3}" "${4}" "${5}"
}

ubootenv_add_sys_mtd() {
	local idx="$(find_mtd_index "${1}")"
	[ -n "$idx" ] && \
		ubootenv_add_uci_sys_config "/dev/mtd$idx" "${2}" "${3}" "${4}" "${5}"
}

ubootenv_add_mmc() {
	local mmcpart="$(find_mmc_part "${1}" "${2}")"
	[ -n "$mmcpart" ] && \
		ubootenv_add_uci_config "$mmcpart" "${3}" "${4}" "${5}" "${6}"
}

ubootenv_add_ubi_default() {
	. /lib/upgrade/nand.sh
	local envubi=$(nand_find_ubi ubi)
	local envdev=/dev/$(nand_find_volume $envubi ubootenv)
	local envdev2=/dev/$(nand_find_volume $envubi ubootenv2)
	ubootenv_add_uci_config "$envdev" "0x0" "0x1f000" "0x1f000" "1"
	ubootenv_add_uci_config "$envdev2" "0x0" "0x1f000" "0x1f000" "1"
}
