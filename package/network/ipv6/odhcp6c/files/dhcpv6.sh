#!/bin/sh

. /lib/functions.sh
. ../netifd-proto.sh
init_proto "$@"

proto_dhcpv6_init_config() {
	proto_config_add_string "reqaddress"
	proto_config_add_string "reqprefix"
	proto_config_add_string "clientid"
	proto_config_add_string "reqopts"
	proto_config_add_string "noslaaconly"
	proto_config_add_string "norelease"
	proto_config_add_string "ip6prefix"
}

proto_dhcpv6_setup() {
	local config="$1"
	local iface="$2"

	local reqaddress reqprefix clientid reqopts noslaaconly norelease ip6prefix iface_dslite
	json_get_vars reqaddress reqprefix clientid reqopts noslaaconly norelease ip6prefix iface_dslite


	# Configure
	local opts=""
	[ -n "$reqaddress" ] && append opts "-N$reqaddress"

	[ -z "$reqprefix" -o "$reqprefix" = "auto" ] && reqprefix=0
	[ "$reqprefix" != "no" ] && append opts "-P$reqprefix"

	[ -n "$clientid" ] && append opts "-c$clientid"

	[ "$noslaaconly" = "1" ] && append opts "-S"

	[ "$norelease" = "1" ] && append opts "-k"

	for opt in $reqopts; do
		append opts "-r$opt"
	done

	[ -n "$ip6prefix" ] && proto_export "USERPREFIX=$ip6prefix"
	[ -n "$iface_dslite" ] && proto_export "IFACE_DSLITE=$iface_dslite"

	proto_export "INTERFACE=$config"
	proto_run_command "$config" odhcp6c \
		-s /lib/netifd/dhcpv6.script \
		$opts $iface
}

proto_dhcpv6_teardown() {
	local interface="$1"
	proto_kill_command "$interface"
}

add_protocol dhcpv6

