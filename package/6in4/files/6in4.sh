#!/bin/sh
# 6in4.sh - IPv6-in-IPv4 tunnel backend
# Copyright (c) 2010 OpenWrt.org

[ -n "$INCLUDE_ONLY" ] || {
	. /etc/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

find_6in4_wanif() {
	local if=$(ip -4 r l e 0.0.0.0/0); if="${if#default* dev }"; if="${if%% *}"
	[ -n "$if" ] && grep -qs "^ *$if:" /proc/net/dev && echo "$if"
}

find_6in4_wanip() {
	local ip=$(ip -4 a s dev "$1"); ip="${ip#*inet }"
	echo "${ip%%[^0-9.]*}"
}

# Hook into scan_interfaces() to synthesize a .device option
# This is needed for /sbin/ifup to properly dispatch control
# to setup_interface_6in4() even if no .ifname is set in
# the configuration.
scan_6in4() {
	config_set "$1" device "6in4-$1"
}

coldplug_interface_6in4() {
	setup_interface_6in4 "6in4-$1" "$1"
}


tun_error() {
	local cfg="$1"; shift;

	[ -n "$1" ] && proto_notify_error "$cfg" "$@"
	proto_block_restart "$cfg"
}

proto_6in4_setup() {
	local cfg="$1"
	local iface="$2"
	local link="6in4-$cfg"

	json_get_var mtu mtu
	json_get_var ttl ttl
	json_get_var local4 ipaddr
	json_get_var remote4 peeraddr
	json_get_var ip6addr ip6addr
	json_get_var tunnelid tunnelid
	json_get_var username username
	json_get_var password password

	[ -z "$ip6addr" -o -z "$remote4" ] && {
		tun_error "$cfg" "MISSING_ADDRESS"
		return
	}

	[ -z "$local4" ] && {
		local wanif=$(find_6in4_wanif)
		[ -z "$wanif" ] && {
			tun_error "$cfg" "NO_WAN_LINK"
			return
		}

		. /lib/network/config.sh
		local wancfg="$(find_config "$wanif")"
		[ -z "$wancfg" ] && {
			tun_error "$cfg" "NO_WAN_LINK"
			return
		}

		# If local4 is unset, guess local IPv4 address from the
		# interface used by the default route.
		[ -n "$wanif" ] && local4=$(find_6in4_wanip "$wanif")

		[ -z "$local4" ] && {
			tun_error "$cfg" "NO_WAN_LINK"
			return
		}
	}

	local local6="${ip6addr%%/*}"
	local mask6="${ip6addr##*/}"
	[[ "$local6" = "$mask6" ]] && mask6=

	proto_init_update "$link" 1
	proto_add_ipv6_address "$local6" "$mask6"
	proto_add_ipv6_route "::" 0

	proto_add_tunnel
	json_add_string mode sit
	json_add_int mtu "${mtu:-1280}"
	json_add_int ttl "${ttl:-64}"
	json_add_string local "$local4"
	json_add_string remote "$remote4"
	proto_close_tunnel

	proto_send_update "$cfg"

	[ -n "$tunnelid" -a -n "$username" -a -n "$password" ] && {
		[ "${#password}" == 32 -a -z "${password//[a-fA-F0-9]/}" ] || {
			password="$(echo -n "$password" | md5sum)"; password="${password%% *}"
		}

		local url="http://ipv4.tunnelbroker.net/ipv4_end.php?ip=AUTO&apikey=$username&pass=$password&tid=$tunnelid"
		local try=0
		local max=3

		while [ $((++try)) -le $max ]; do
			wget -qO/dev/null "$url" 2>/dev/null && break
			sleep 1
		done
	}
}

proto_6in4_teardown() {
	local cfg="$1"
}

proto_6in4_init_config() {
	no_device=1             
	available=1

	proto_config_add_string "ipaddr"
	proto_config_add_string "ip6addr"
	proto_config_add_string "peeraddr"
	proto_config_add_string "tunnelid"
	proto_config_add_string "username"
	proto_config_add_string "password"
	proto_config_add_int "mtu"
	proto_config_add_int "ttl"
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol 6in4
}
