#!/bin/sh

. /etc/functions.sh
. ../netifd-proto.sh
init_proto "$@"

dhcp_init_config() {
	proto_config_add_string "ipaddr"
	proto_config_add_string "netmask"
	proto_config_add_string "hostname"
	proto_config_add_string "clientid"
	proto_config_add_string "vendorid"
	proto_config_add_boolean "broadcast"
	proto_config_add_string "reqopts"
}

dhcp_setup() {
	local config="$1"
	local iface="$2"

	json_get_var ipaddr ipaddr
	json_get_var hostname hostname
	json_get_var clientid clientid
	json_get_var vendorid vendorid
	json_get_var broadcast broadcast
	json_get_var reqopts reqopts

	local opt dhcpopts
	for opt in $reqopts; do
		append dhcpopts "-O opt"
	done

	[ "$broadcast" = 1 ] && broadcast="-O broadcast" || broadcast=

	proto_export "INTERFACE=$config"
	proto_run_command "$config" udhcpc \
		-p /var/run/udhcpc-$iface.pid \
		-s /lib/netifd/dhcp.script \
		-f -t 0 -i "$iface" \
		${ipaddr:+-r $ipaddr} \
		${hostname:+-H $hostname} \
		${clientid:+-c $clientid} \
		${vendorid:+-V $vendorid} \
		$broadcast $dhcpopts
}

dhcp_teardown() {
	proto_kill_command
}

add_protocol dhcp

