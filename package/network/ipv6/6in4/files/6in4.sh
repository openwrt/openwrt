#!/bin/sh
# 6in4.sh - IPv6-in-IPv4 tunnel backend
# Copyright (c) 2010-2015 OpenWrt.org

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. /lib/functions/network.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

# Function taken from 6to4 package (6to4.sh), flipped returns
test_6in4_rfc1918()
{
	local oIFS="$IFS"; IFS="."; set -- $1; IFS="$oIFS"
	[ $1 -eq  10 ] && return 1
	[ $1 -eq 192 ] && [ $2 -eq 168 ] && return 1
	[ $1 -eq 172 ] && [ $2 -ge  16 ] && [ $2 -le  31 ] && return 1

	# RFC 6598
	[ $1 -eq 100 ] && [ $2 -ge  64 ] && [ $2 -le 127 ] && return 1

	return 0
}

proto_6in4_add_prefix() {
	append "$3" "$1"
}

proto_6in4_setup() {
	local cfg="$1"
	local iface="$2"
	local link="6in4-$cfg"

	local mtu ttl tos ipaddr peeraddr ip6addr ip6prefix ip6prefixes tunlink add_peer_route \
		tunnelid username password updatekey send_ip delay timeout retry_interval max_retries
	json_get_vars mtu ttl tos ipaddr peeraddr ip6addr tunlink add_peer_route \
		tunnelid username password updatekey send_ip delay timeout retry_interval max_retries
	json_for_each_item proto_6in4_add_prefix ip6prefix ip6prefixes

	[ -z "$peeraddr" ] && {
		proto_notify_error "$cfg" "MISSING_ADDRESS"
		proto_block_restart "$cfg"
		return
	}

	[ ${add_peer_route:=1} -eq 1 ] && {
		proto_add_host_dependency "$cfg" "$peeraddr" "$tunlink"
	}

	[ -z "$ipaddr" ] && {
		local wanif="$tunlink"
		if [ -z "$wanif" ] && ! network_find_wan wanif; then
			proto_notify_error "$cfg" "NO_WAN_LINK"
			return
		fi

		if ! network_get_ipaddr ipaddr "$wanif"; then
			proto_notify_error "$cfg" "NO_WAN_LINK"
			return
		fi
	}

	proto_init_update "$link" 1

	[ -n "$ip6addr" ] && {
		local local6="${ip6addr%%/*}"
		local mask6="${ip6addr##*/}"
		[ "$local6" = "$mask6" ] && mask6=
		proto_add_ipv6_address "$local6" "$mask6"
		proto_add_ipv6_route "::" 0 "" "" "" "$local6/$mask6"
	}

	for ip6prefix in $ip6prefixes; do
		proto_add_ipv6_prefix "$ip6prefix"
		proto_add_ipv6_route "::" 0 "" "" "" "$ip6prefix"
	done

	proto_add_tunnel
	json_add_string mode sit
	json_add_int mtu "${mtu:-1280}"
	json_add_int ttl "${ttl:-64}"
	[ -n "$tos" ] && json_add_string tos "$tos"
	json_add_string local "$ipaddr"
	json_add_string remote "$peeraddr"
	[ -n "$tunlink" ] && json_add_string link "$tunlink"
	proto_close_tunnel

	proto_send_update "$cfg"

	[ -n "$tunnelid" -a -n "$username" -a \( -n "$password" -o -n "$updatekey" \) ] && {
		[ -n "$updatekey" ] && password="$updatekey"

		local http="http"
		local urlget="uclient-fetch"
		local urlget_opts="-qO- -T ${timeout:-5}"
		local ca_path="${SSL_CERT_DIR:-/etc/ssl/certs}"

		[ -f /lib/libustream-ssl.so ] && http=https
		[ "$http" = "https" -a -z "$(find $ca_path -name "*.0" 2>/dev/null)" ] && {
			urlget_opts="$urlget_opts --no-check-certificate"
		}

		local url="$http://ipv4.tunnelbroker.net/nic/update?hostname=$tunnelid"

		test_6in4_rfc1918 "$ipaddr" && [ ${send_ip:=1} -eq 1 ] && {
			url="${url}&myip=${ipaddr}"
		}

		[ ${delay:=0} -gt 0 ] && sleep $delay

		(
			local try=0
			while [ $((++try)) -le ${max_retries:=3} ]; do
				local response rc error
				response=$($urlget $urlget_opts --user="$username" --password="$password" "$url" 2>&1)
				rc=$?
				case $rc in
					4) error="timeout";;
					5) error="SSL error";;
					*) error="unknown error";;
				esac

				logger -t "$link" "update $try/$max_retries: ${response:-$error}"

				local msg
				if [ $rc -eq 0 ]; then
					case "$response" in
						good*) msg="updated";;
						nochg*) msg="not changed";;
						*) msg="unexpected response received";;
					esac
					logger -t "$link" "$msg"
					return 0
				fi
				[ $try -lt $max_retries ] && sleep ${retry_interval:=5}
			done
			logger -t "$link" "max update retries exceeded"
		)
	}

	[ -f "/etc/6in4.user" ] && {
		env -i INTERFACE="$cfg" LINK="$link" \
			/bin/sh /etc/6in4.user
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
	proto_config_add_array "ip6prefix"
	proto_config_add_string "peeraddr"
	proto_config_add_string "tunlink"
	proto_config_add_boolean "add_peer_route"
	proto_config_add_string "tunnelid"
	proto_config_add_string "username"
	proto_config_add_string "password"
	proto_config_add_string "updatekey"
	proto_config_add_boolean "send_ip"
	proto_config_add_int "delay"
	proto_config_add_int "timeout"
	proto_config_add_int "retry_interval"
	proto_config_add_int "max_retries"
	proto_config_add_int "mtu"
	proto_config_add_int "ttl"
	proto_config_add_string "tos"
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol 6in4
}
