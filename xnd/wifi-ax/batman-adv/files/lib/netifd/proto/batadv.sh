#!/bin/sh

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_batadv_init_config() {
	no_device=1
	available=1

	proto_config_add_boolean 'aggregated_ogms:bool'
	proto_config_add_boolean 'ap_isolation:bool'
	proto_config_add_boolean 'bonding:bool'
	proto_config_add_boolean 'bridge_loop_avoidance:bool'
	proto_config_add_boolean 'distributed_arp_table:bool'
	proto_config_add_boolean 'fragmentation:bool'
	proto_config_add_string 'gw_bandwidth'
	proto_config_add_string 'gw_mode'
	proto_config_add_int 'gw_sel_class'
	proto_config_add_int 'hop_penalty'
	proto_config_add_string 'isolation_mark'
	proto_config_add_string 'log_level'
	proto_config_add_int 'multicast_fanout'
	proto_config_add_boolean 'multicast_mode:bool'
	proto_config_add_boolean 'network_coding:bool'
	proto_config_add_int 'orig_interval'
	proto_config_add_string 'routing_algo'
}

proto_batadv_setup() {
	local config="$1"
	local iface="$config"

	local aggregated_ogms
	local ap_isolation
	local bonding
	local bridge_loop_avoidance
	local distributed_arp_table
	local fragmentation
	local gw_bandwidth
	local gw_mode
	local gw_sel_class
	local hop_penalty
	local isolation_mark
	local log_level
	local multicast_fanout
	local multicast_mode
	local network_coding
	local orig_interval
	local routing_algo

	json_get_vars aggregated_ogms
	json_get_vars ap_isolation
	json_get_vars bonding
	json_get_vars bridge_loop_avoidance
	json_get_vars distributed_arp_table
	json_get_vars fragmentation
	json_get_vars gw_bandwidth
	json_get_vars gw_mode
	json_get_vars gw_sel_class
	json_get_vars hop_penalty
	json_get_vars isolation_mark
	json_get_vars log_level
	json_get_vars multicast_fanout
	json_get_vars multicast_mode
	json_get_vars network_coding
	json_get_vars orig_interval
	json_get_vars routing_algo

	set_default routing_algo 'BATMAN_IV'

	batctl routing_algo "$routing_algo"
	batctl meshif "$iface" interface create

	[ -n "$aggregated_ogms" ] && batctl meshif "$iface" aggregation "$aggregated_ogms"
	[ -n "$ap_isolation" ] && batctl meshif "$iface" ap_isolation "$ap_isolation"
	[ -n "$bonding" ] && batctl meshif "$iface" bonding "$bonding"
	[ -n "$bridge_loop_avoidance" ] &&  batctl meshif "$iface" bridge_loop_avoidance "$bridge_loop_avoidance" 2>&-
	[ -n "$distributed_arp_table" ] && batctl meshif "$iface" distributed_arp_table "$distributed_arp_table" 2>&-
	[ -n "$fragmentation" ] && batctl meshif "$iface" fragmentation "$fragmentation"

	case "$gw_mode" in
	server)
		if [ -n "$gw_bandwidth" ]; then
			batctl meshif "$iface" gw_mode "server" "$gw_bandwidth"
		else
			batctl meshif "$iface" gw_mode "server"
		fi
		;;
	client)
		if [ -n "$gw_sel_class" ]; then
			batctl meshif "$iface" gw_mode "client" "$gw_sel_class"
		else
			batctl meshif "$iface" gw_mode "client"
		fi
		;;
	*)
		batctl meshif "$iface" gw_mode "off"
		;;
	esac

	[ -n "$hop_penalty" ] && batctl meshif "$iface" hop_penalty "$hop_penalty"
	[ -n "$isolation_mark" ] && batctl meshif "$iface" isolation_mark "$isolation_mark"
	[ -n "$multicast_fanout" ] && batctl meshif "$iface" multicast_fanout "$multicast_fanout"
	[ -n "$multicast_mode" ] && batctl meshif "$iface" multicast_mode "$multicast_mode" 2>&-
	[ -n "$network_coding" ] && batctl meshif "$iface" network_coding "$network_coding" 2>&-
	[ -n "$log_level" ] && batctl meshif "$iface" loglevel "$log_level" 2>&-
	[ -n "$orig_interval" ] && batctl meshif "$iface" orig_interval "$orig_interval"

	proto_init_update "$iface" 1
	proto_send_update "$config"
}

proto_batadv_teardown() {
	local config="$1"
	local iface="$config"

	batctl meshif "$iface" interface destroy
}

add_protocol batadv
