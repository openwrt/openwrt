#!/bin/sh
# dslite.sh - IPv4-in-IPv6 tunnel backend for ipip6 and ds-lite
# Copyright (c) 2013 OpenWrt.org
# Copyright (c) 2013 Steven Barth <steven@midlink.org>
# Copyright (c) 2021 Kenji Uno <ku@digitaldolphins.jp>
# Copyright (c) 2024 Arayuki Mago <ms@missing233.com>

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. /lib/functions/network.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

tnl_setup() {
	local cfg="$1"
	local iface="$2"
	local tnl_type="$3"
	local ip4addr="$4"
	local ip4gateway="$5"
	local link="$tnl_type-$cfg"
	local remoteip6

	local mtu ttl peeraddr ip6addr tunlink zone weakif encaplimit
	json_get_vars mtu ttl peeraddr ip6addr tunlink zone weakif encaplimit

	[ -z "$peeraddr" ] && {
		proto_notify_error "$cfg" "MISSING_ADDRESS"
		proto_block_restart "$cfg"
		return
	}

	( proto_add_host_dependency "$cfg" "::" "$tunlink" )

	remoteip6=$(resolveip -6 "$peeraddr")
	if [ -z "$remoteip6" ]; then
		sleep 3
		remoteip6=$(resolveip -6 "$peeraddr")
		if [ -z "$remoteip6" ]; then
			proto_notify_error "$cfg" "AFTR_DNS_FAIL"
			return
		fi
	fi

	for ip6 in $remoteip6; do
		peeraddr=$ip6
		break
	done

	[ -z "$ip6addr" ] && {
		local wanif="$tunlink"
		if [ -z "$wanif" ] && ! network_find_wan6 wanif; then
			proto_notify_error "$cfg" "NO_WAN_LINK"
			return
		fi

		if ! network_get_ipaddr6 ip6addr "$wanif"; then
			[ -z "$weakif" ] && weakif="lan"
			if ! network_get_ipaddr6 ip6addr "$weakif"; then
				proto_notify_error "$cfg" "NO_WAN_LINK"
				return
			fi
		fi
	}

	proto_init_update "$link" 1
	proto_add_ipv4_route "0.0.0.0" 0
	proto_add_ipv4_address "$ip4addr" "" "" "$ip4gateway"

	proto_add_tunnel
	json_add_string mode ipip6
	json_add_int mtu "${mtu:-1280}"
	json_add_int ttl "${ttl:-64}"
	json_add_string local "$ip6addr"
	json_add_string remote "$peeraddr"
	[ -n "$tunlink" ] && json_add_string link "$tunlink"
	json_add_object "data"
	  [ -n "$encaplimit" ] && json_add_string encaplimit "$encaplimit"
	json_close_object
	proto_close_tunnel

	proto_add_data
	[ -n "$zone" ] && json_add_string zone "$zone"

	if [ "$tnl_type" = "ds" ]; then
		json_add_array firewall
			json_add_object ""
				json_add_string type nat
				json_add_string target ACCEPT
			json_close_object
		json_close_array
	fi

	proto_close_data

	proto_send_update "$cfg"
}

init_config() {
	no_device=1
	available=1

	proto_config_add_string "ip6addr"
	proto_config_add_string "peeraddr"
	proto_config_add_string "tunlink"
	proto_config_add_int "mtu"
	proto_config_add_int "ttl"
	proto_config_add_string "encaplimit"
	proto_config_add_string "zone"
	proto_config_add_string "weakif"
}

proto_ipip6_init_config() {
	init_config
	proto_config_add_string "ip4ifaddr"
}

proto_ipip6_setup() {
	local ip4ifaddr
	json_get_vars ip4ifaddr
	tnl_setup "$1" "$2" "ipip6" "$ip4ifaddr" "0.0.0.0"
}

proto_ipip6_teardown() {
	local cfg="$1"
}

proto_dslite_init_config() {
	init_config
}

proto_dslite_setup() {
	tnl_setup "$1" "$2" "ds" "192.0.0.2" "192.0.0.1"
}

proto_dslite_teardown() {
	local cfg="$1"
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol ipip6
	add_protocol dslite
}
