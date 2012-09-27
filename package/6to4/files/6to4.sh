#!/bin/sh
# 6to4.sh - IPv6-in-IPv4 tunnel backend
# Copyright (c) 2010-2012 OpenWrt.org

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. /lib/functions/network.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

find_6to4_prefix() {
	local ip4="$1"
	local oIFS="$IFS"; IFS="."; set -- $ip4; IFS="$oIFS"

	printf "2002:%02x%02x:%02x%02x\n" $1 $2 $3 $4
}

test_6to4_rfc1918()
{
	local oIFS="$IFS"; IFS="."; set -- $1; IFS="$oIFS"
	[ $1 -eq  10 ] && return 0
	[ $1 -eq 192 ] && [ $2 -eq 168 ] && return 0
	[ $1 -eq 172 ] && [ $2 -ge  16 ] && [ $2 -le  31 ] && return 0

	# RFC 6598
	[ $1 -eq 100 ] && [ $2 -ge  64 ] && [ $2 -le 127 ] && return 0

	return 1
}

set_6to4_radvd_interface() {
	local cfgid="$1"
	local lanif="${2:-lan}"
	local ifmtu="${3:-1280}"
	local ifsection=""

	find_ifsection() {
		local net
		local cfg="$1"
		config_get net "$cfg" interface

		[ "$net" = "$lanif" ] && {
			ifsection="$cfg"
			return 1
		}
	}

	config_foreach find_ifsection interface

	[ -z "$ifsection" ] && {
		ifsection="iface_$sid"
		uci_set_state radvd "$ifsection" "" interface
		uci_set_state radvd "$ifsection" interface "$lanif"
	}

	uci_set_state radvd "$ifsection" ignore            0
	uci_set_state radvd "$ifsection" IgnoreIfMissing   1
	uci_set_state radvd "$ifsection" AdvSendAdvert     1
	uci_set_state radvd "$ifsection" MaxRtrAdvInterval 30
	uci_set_state radvd "$ifsection" AdvLinkMTU        "$ifmtu"
}

set_6to4_radvd_prefix() {
	local cfgid="$1"
	local lanif="${2:-lan}"
	local wanif="${3:-wan}"
	local prefix="${4:-0:0:0:1::/64}"
	local vlt="${5:-300}"
	local plt="${6:-120}"
	local pfxsection=""

	find_pfxsection() {
		local net base
		local cfg="$1"
		config_get net  "$cfg" interface
		config_get base "$cfg" Base6to4Interface

		[ "$net" = "$lanif" ] && [ "$base" = "$wanif" ] && {
			pfxsection="$cfg"
			return 1
		}
	}

	config_foreach find_pfxsection prefix

	[ -z "$pfxsection" ] && {
		pfxsection="prefix_${sid}_${lanif}"
		uci_set_state radvd "$pfxsection" ""                   prefix
		uci_set_state radvd "$pfxsection" ignore               0
		uci_set_state radvd "$pfxsection" interface            "$lanif"
		uci_set_state radvd "$pfxsection" prefix               "$prefix"
		uci_set_state radvd "$pfxsection" AdvOnLink            1
		uci_set_state radvd "$pfxsection" AdvAutonomous        1
		uci_set_state radvd "$pfxsection" AdvValidLifetime     "$vlt"
		uci_set_state radvd "$pfxsection" AdvPreferredLifetime "$plt"
		uci_set_state radvd "$pfxsection" Base6to4Interface    "$wanif"
	}
}

proto_6to4_setup() {
	local cfg="$1"
	local iface="$2"
	local link="6to4-$cfg"

	local mtu ttl ipaddr adv_subnet adv_interface adv_valid_lifetime adv_preferred_lifetime
	json_get_vars mtu ttl ipaddr adv_subnet adv_interface adv_valid_lifetime adv_preferred_lifetime

	( proto_add_host_dependency "$cfg" 0.0.0.0 )

	local wanif
	if ! network_find_wan wanif; then
		proto_notify_error "$cfg" "NO_WAN_LINK"
		return
	fi

	[ -z "$ipaddr" ] && {
		if ! network_get_ipaddr ipaddr "$wanif"; then
			proto_notify_error "$cfg" "NO_WAN_ADDRESS"
			return
		fi
	}

	test_6to4_rfc1918 "$ipaddr" && {
		proto_notify_error "$cfg" "INVALID_LOCAL_ADDRESS"
		return
	}

	# find our local prefix
	local prefix6=$(find_6to4_prefix "$ipaddr")
	local local6="$prefix6::1"

	proto_init_update "$link" 1
	proto_add_ipv6_address "$local6" 16
	proto_add_ipv6_route "::" 0 "::192.88.99.1"

	proto_add_tunnel
	json_add_string mode sit
	json_add_int mtu "${mtu:-1280}"
	json_add_int ttl "${ttl:-64}"
	json_add_string local "$ipaddr"
	proto_close_tunnel

	proto_send_update "$cfg"

	[ -f /etc/config/radvd ] && /etc/init.d/radvd enabled && {
		local sid="6to4_$cfg"

		uci_revert_state radvd
		config_load radvd

		adv_subnet=$((0x${adv_subnet:-1}))

		local adv_subnets=""

		for adv_interface in ${adv_interface:-lan}; do
			local adv_ifname
			network_get_device adv_ifname "${adv_interface:-lan}" || continue

			local subnet6="$(printf "%s:%x::1/64" "$prefix6" $adv_subnet)"

			logger -t "$link" " * Advertising IPv6 subnet $subnet6 on ${adv_interface:-lan} ($adv_ifname)"
			ip -6 addr add $subnet6 dev $adv_ifname

			set_6to4_radvd_interface "$sid" "$adv_interface" "$mtu"
			set_6to4_radvd_prefix    "$sid" "$adv_interface" \
				"$wanif" "$(printf "0:0:0:%x::/64" $adv_subnet)" \
				"$adv_valid_lifetime" "$adv_preferred_lifetime"

			adv_subnets="${adv_subnets:+$adv_subnets }$adv_ifname:$subnet6"
			adv_subnet=$(($adv_subnet + 1))
		done

		uci_set_state network "$cfg" adv_subnets "$adv_subnets"

		/etc/init.d/radvd restart
	}
}

proto_6to4_teardown() {
	local cfg="$1"
	local link="6to4-$cfg"

	local adv_subnets=$(uci_get_state network "$cfg" adv_subnets)

	grep -qs "^ *$link:" /proc/net/dev && {
		[ -n "$adv_subnets" ] && {
			uci_revert_state radvd
			/etc/init.d/radvd enabled && /etc/init.d/radvd restart
		}
	}
}

proto_6to4_init_config() {
	no_device=1
	available=1

	proto_config_add_string "ipaddr"
	proto_config_add_int "mtu"
	proto_config_add_int "ttl"
	proto_config_add_string "adv_interface"
	proto_config_add_string "adv_subnet"
	proto_config_add_int "adv_valid_lifetime"
	proto_config_add_int "adv_preferred_lifetime"
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol 6to4
}
