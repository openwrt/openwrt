#!/bin/sh
# Copyright (C) 2011 OpenWrt.org

. /usr/share/libubox/jshn.sh

find_config() {
	return
}

unbridge() {
	return
}

ubus_call() {
	json_init
	local _data="$(ubus call "$1" "$2")"
	[ $? -ne 0 ] && return "$?"
	json_load "$_data"
	return 0
}


fixup_interface() {
	local config="$1"
	local ifname

	config_get type "$config" type
	config_get ifname "$config" ifname
	config_get device "$config" device "$ifname"
	[ "bridge" = "$type" ] && ifname="br-$config"
	config_set "$config" device "$ifname"
	ubus_call "network.interface.$config" status
	json_get_var l3dev l3_device
	[ -n "$l3dev" ] && ifname="$l3dev"
	json_init
	config_set "$config" ifname "$ifname"
	config_set "$config" device "$device"
}

scan_interfaces() {
	config_load network
	config_foreach fixup_interface interface
}

prepare_interface_bridge() {
	local config="$1"

	[ -n "$config" ] || return 0
	ubus call network.interface."$config" prepare
}

setup_interface() {
	local iface="$1"
	local config="$2"

	[ -n "$config" ] || return 0
	ubus call network.interface."$config" add_device "{ \"name\": \"$iface\" }"
}

