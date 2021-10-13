#!/bin/sh

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_batadv_hardif_init_config() {
	proto_config_add_int 'elp_interval'
	proto_config_add_int 'hop_penalty'
	proto_config_add_string "master"
	proto_config_add_string 'throughput_override'
}

proto_batadv_hardif_setup() {
	local config="$1"
	local iface="$2"

	local elp_interval
	local hop_penalty
	local master
	local throughput_override

	json_get_vars elp_interval
	json_get_vars hop_penalty
	json_get_vars master
	json_get_vars throughput_override

	( proto_add_host_dependency "$config" '' "$master" )

	batctl meshif "$master" interface -M add "$iface"

	[ -n "$elp_interval" ] && batctl hardif "$iface" elp_interval "$elp_interval"
	[ -n "$hop_penalty" ] && batctl hardif "$iface" hop_penalty "$hop_penalty"
	[ -n "$throughput_override" ] && batctl hardif "$iface" throughput_override "$throughput_override"

	proto_init_update "$iface" 1
	proto_send_update "$config"
}

proto_batadv_hardif_teardown() {
	local config="$1"
	local iface="$2"

	local master

	json_get_vars master

	batctl meshif "$master" interface -M del "$iface" || true
}

add_protocol batadv_hardif
