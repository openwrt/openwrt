#!/bin/sh

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. /lib/functions/network.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_gre_setup() { echo "iptunnel[$$] gre proto missing";}

[ -f ./gre.sh ] && . ./gre.sh

proto_iptunnel_setup() {
	local cfg="$1"

	local localaddr netmask
	json_get_vars localaddr netmask

	proto_gre_setup $@

	[ -n "$localaddr" ] && {
		local prefix
		[ -n "$netmask" ] && {
			local prefix
			eval "$(ipcalc.sh "$localaddr" "$netmask")"
			prefix="$PREFIX"
		}

		if [ -z "$prefix" ]; then
			echo "Assign IPv4 address $localaddr to device gre4-$cfg"
		else
			echo "Assign IPv4 transfer net ${localaddr}/${prefix:-32} to device gre4-$cfg"
		fi

		proto_init_update "gre4-$cfg" 1
		proto_set_keep 1
		proto_add_ipv4_address "$localaddr" "${prefix:-32}"
		proto_send_update "$cfg"
	}
}

proto_iptunnel_teardown() {
	local cfg="$1"

	proto_gre_teardown $@
}

iptunnel_generic_init_config() {
	no_device=1
	available=1

	proto_config_add_int "mtu"
	proto_config_add_int "ttl"
	proto_config_add_string "tos"
	proto_config_add_string "tunlink"
	proto_config_add_string "zone"
	proto_config_add_int "ikey"
	proto_config_add_int "okey"
	proto_config_add_boolean "icsum"
	proto_config_add_boolean "ocsum"
	proto_config_add_boolean "iseqno"
	proto_config_add_boolean "oseqno"
	proto_config_add_boolean "multicast"
}

proto_iptunnel_init_config() {
	gre_generic_init_config
	proto_config_add_string "ipaddr"
	proto_config_add_string "peeraddr"
	proto_config_add_boolean "df"
	proto_config_add_string "localaddr"
	proto_config_add_string "netmask"
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol iptunnel
}
