#!/bin/sh

. /lib/functions.sh
. ../netifd-proto.sh
init_proto "$@"

proto_batadv_vlan_init_config() {
	proto_config_add_boolean 'ap_isolation:bool'
}

proto_batadv_vlan_setup() {
	local config="$1"
	local iface="$2"

	# batadv_vlan options
	local ap_isolation

	json_get_vars ap_isolation

	[ -n "$ap_isolation" ] && batctl vlan "$iface" ap_isolation "$ap_isolation"
	proto_init_update "$iface" 1
	proto_send_update "$config"
}

add_protocol batadv_vlan
