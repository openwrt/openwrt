#!/bin/sh
# 6in4.sh - IPv6-in-IPv4 tunnel backend
# Copyright (c) 2010-2014 OpenWrt.org

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. /lib/functions/network.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_6in4_setup() {
	local cfg="$1"
	local iface="$2"
	local link="6in4-$cfg"

	local mtu ttl tos ipaddr peeraddr ip6addr ip6prefix tunnelid username password updatekey sourcerouting
	json_get_vars mtu ttl tos ipaddr peeraddr ip6addr ip6prefix tunnelid username password updatekey sourcerouting

	[ -z "$peeraddr" ] && {
		proto_notify_error "$cfg" "MISSING_ADDRESS"
		proto_block_restart "$cfg"
		return
	}

	( proto_add_host_dependency "$cfg" 0.0.0.0 )

	[ -z "$ipaddr" ] && {
		local wanif
		if ! network_find_wan wanif || ! network_get_ipaddr ipaddr "$wanif"; then
			proto_notify_error "$cfg" "NO_WAN_LINK"
			return
		fi
	}

	proto_init_update "$link" 1

	local source=""
	[ "$sourcerouting" != "0" ] && source="::/128"
	proto_add_ipv6_route "::" 0 "" "" "" "$source"

	[ -n "$ip6addr" ] && {
		local local6="${ip6addr%%/*}"
		local mask6="${ip6addr##*/}"
		[[ "$local6" = "$mask6" ]] && mask6=
		proto_add_ipv6_address "$local6" "$mask6"
		[ "$sourcerouting" != "0" ] && proto_add_ipv6_route "::" 0 "" "" "" "$local6/$mask6"
	}

	[ -n "$ip6prefix" ] && {
		proto_add_ipv6_prefix "$ip6prefix"
		[ "$sourcerouting" != "0" ] && proto_add_ipv6_route "::" 0 "" "" "" "$ip6prefix"
	}

	proto_add_tunnel
	json_add_string mode sit
	json_add_int mtu "${mtu:-1280}"
	json_add_int ttl "${ttl:-64}"
	[ -n "$tos" ] && json_add_string tos "$tos"
	json_add_string local "$ipaddr"
	json_add_string remote "$peeraddr"
	proto_close_tunnel

	proto_send_update "$cfg"

	[ -n "$tunnelid" -a -n "$username" -a \( -n "$password" -o -n "$updatekey" \) ] && {
		[ -n "$updatekey" ] && password="$updatekey"

		local http="http"
		local wget_opts="-qO/dev/null"
		if wget --version | grep -qF "+https"; then
			http="https"
			[ -z "$(find ${SSL_CERT_DIR-/etc/ssl/certs} -name "*.0" 2>/dev/null)" ] && {
				wget_opts="$wget_opts --no-check-certificate"
			}
		fi

		local url="$http://ipv4.tunnelbroker.net/nic/update?username=$username&password=$password&hostname=$tunnelid"
		local try=0
		local max=3

		while [ $((++try)) -le $max ]; do
			( exec wget $wget_opts "$url" 2>/dev/null ) &
			local pid=$!
			( sleep 5; kill $pid 2>/dev/null ) &
			wait $pid && break
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
	proto_config_add_string "ip6prefix"
	proto_config_add_string "peeraddr"
	proto_config_add_string "tunnelid"
	proto_config_add_string "username"
	proto_config_add_string "password"
	proto_config_add_string "updatekey"
	proto_config_add_int "mtu"
	proto_config_add_int "ttl"
	proto_config_add_string "tos"
	proto_config_add_boolean "sourcerouting"
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol 6in4
}
