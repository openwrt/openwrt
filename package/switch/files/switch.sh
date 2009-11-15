#!/bin/sh
# Copyright (C) 2006-2009 OpenWrt.org

setup_switch_hw() {
	local dev="$1"
	local enable reset evlan

	config_get_bool enable "$dev" enable 1
	config_get_bool evlan  "$dev" enable_vlan 1
	config_get_bool reset  "$dev" reset 1

	local proc="/proc/switch/$dev"
	[ -d "$proc" ] && {
		echo "$reset"  > "$proc/reset"
		echo "$evlan"  > "$proc/enable_vlan"
		echo "$enable" > "$proc/enable"
	}
}

setup_switch_vlan() {
	local s="$1"
	local dev vlan ports

	config_get dev   "$s" device
	config_get vlan  "$s" vlan
	config_get ports "$s" ports

	[ -n "$dev" ] && [ -n "$vlan" ] && { 
		local proc="/proc/switch/$dev/vlan/$vlan/ports"
		[ -f "$proc" ] && echo "$ports" > "$proc"
	}
}

setup_switch() {
	config_load network
	config_foreach setup_switch_hw switch
	config_foreach setup_switch_vlan switch_vlan
}
