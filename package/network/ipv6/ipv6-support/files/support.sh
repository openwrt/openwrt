#!/bin/sh
# Copyright (c) 2012 OpenWrt.org
. /lib/functions.sh
. /lib/functions/service.sh
. /lib/functions/network.sh

config_load network6


conf_get() {
	local __return="$1"
	local __device="$2"
	local __option="$3"
	local __value=$(cat "/proc/sys/net/ipv6/conf/$device/$option")
	eval "$__return=$__value"
}


conf_set() {
	local device="$1"
	local option="$2"
	local value="$3"
	echo "$value" > "/proc/sys/net/ipv6/conf/$device/$option"
}


stop_service() {
	local __exe="$1"
	SERVICE_PID_FILE="$2"
	local __return="$3"

	service_check "$__exe" && {
		service_stop "$__exe"
		[ -n "$__return" ] && eval "$__return=1"
	}
	rm -f "$SERVICE_PID_FILE"
}


start_service() {
	local cmd="$1"
	local pidfile="$2"

	SERVICE_DAEMONIZE=1
	SERVICE_WRITE_PID=1
	SERVICE_PID_FILE="$pidfile"
	service_start $cmd
}


resolve_network_add() {
	local __section="$1"
	local __device="$2"
	local __return="$3"

	local __cdevice
	network_get_device __cdevice "$__section"
	[ "$__cdevice" != "$__device" ] && return
	
	eval "$__return"'="'"$__section"'"'
}


resolve_network() {
	local __return="$1"
	local __device="$2"
	config_foreach resolve_network_add interface "$__device" "$__return"
}


announce_prefix() {
	local prefix="$1"
	local network="$2"
	local cmd="$3"

	local addr=$(echo "$prefix" | cut -d/ -f1)
	local rem=$(echo "$prefix" | cut -d/ -f2)
	local length=$(echo "$rem" | cut -d, -f1)
	local prefer=""
	local valid=""

	# If preferred / valid provided
	[ "$rem" != "$length" ] && {
		prefer=$(echo "$rem" | cut -d, -f2)
		valid=$(echo "$rem" | cut -d, -f3)
	}

	local msg='{"network": "'"$network"'", "prefix": "'"$addr"'", "length": '"$length"
	[ -n "$valid" ] && msg="$msg"', "valid": '"$valid"', "preferred": '"$prefer"
	[ -z "$cmd" ] && cmd=newprefix
	
	ubus call 6distributed "$cmd" "$msg}"
}


disable_downstream() {
	local network="$1"

	# Notify the address distribution daemon
	ubus call 6distributed deliface '{"network": "'"$network"'"}'

	# Disable advertisement daemon
	stop_service /usr/sbin/6relayd "/var/run/ipv6-downstream-$network.pid"
}


restart_relay_add() {
	local __section="$1"
	local __return="$2"
	local __master="$3"
	local __disable="$4"

	network_is_up "$__section" || return

	# Match master network
	local __cmaster=""
	config_get __cmaster "$__section" relay_master
	[ "$__master" != "$__cmaster" ] && return
	
	# Disable any active distribution
	disable_downstream "$__section"

	local __device=""
	network_get_device __device "$__section"
	
	# Coming from stop relay, reenable distribution
	[ "$__disable" == "disable" ] && {
		enable_downstream "$__section" "$__device"
		return
	}

	
	eval "$__return"'="$'"$__return"' '"$__device"'"'
}


stop_relay() {
	local network="$1"
	local pid="/var/run/ipv6-relay-$network.pid"
	local was_running=""
	
	stop_service /usr/sbin/6relayd "$pid" was_running

	# Reenable normal distribution on slave interfaces	
	[ -n "$was_running" ] && config_foreach restart_relay_add interface dummy "$network" disable
}


restart_relay() {
	local network="$1"
	local force="$2"
	local pid="/var/run/ipv6-relay-$network.pid"

	local not_running=0
	[ -f "$pid" ] || not_running=1

	# Don't start if not desired
	[ "$force" != "1" ] && [ "$not_running" == "1" ] && return

	# Kill current relay and distribution daemon
	stop_relay "$network"

	# Detect master device
	local device=""
	network_get_device device $network

	# Generate command string
	local cmd="/usr/sbin/6relayd -A $device "
	config_foreach restart_relay_add interface cmd "$network"

	# Start relay
	start_service "$cmd" "$pid"
}


restart_master_relay() {
	local network="$1"

	# Disable active relaying to this interface
	local relay_master
	config_get relay_master "$network" relay_master
	[ -n "$relay_master" ] && restart_relay "$relay_master"
}


disable_interface() {
	local network="$1"

	# Delete all prefixes routed to this interface
	ubus call 6distributed delprefix '{"network": "'"$network"'"}'

	# Restart Relay
	restart_master_relay "$network"

	# Disable distribution
	disable_downstream "$network"

	# Disable relay
	stop_relay "$network"

	# Disable DHCPv6 client if enabled, state script will take care
	stop_service /usr/sbin/odhcp6c "/var/run/ipv6-upstream-$network.pid"
}


enable_static() {
	local network="$1"
	local device="$2"

	# Enable global forwarding
	local global_forward
	conf_get global_forward all forwarding
	[ "$global_forward" != "1" ] && conf_set all forwarding 1

	# Configure device
	conf_set "$device" accept_ra 1
	conf_set "$device" forwarding 1

	# ULA-integration
	local ula_prefix=""
	config_get ula_prefix "$network" ula_prefix

	# ULA auto configuration (first init)
	[ "$ula_prefix" == "auto" ] && {
		local r1=""
		local r2=""
		local r3=""

		# Sometimes results are empty, therefore try until it works...		
		while [ -z "$r1" -o -z "$r2" -o -z "$r3" ]; do
			r1=$(printf "%02x" $(($(</dev/urandom tr -dc 0-9 | dd bs=9 count=1) % 256)))
			r2=$(printf "%01x" $(($(</dev/urandom tr -dc 0-9 | dd bs=9 count=1) % 65536)))
			r3=$(printf "%01x" $(($(</dev/urandom tr -dc 0-9 | dd bs=9 count=1) % 65536)))
		done
		
		ula_prefix="fd$r1:$r2:$r3::/48"

		# Save prefix so it will be preserved across reboots
		uci set network6.$network.ula_prefix=$ula_prefix
		uci commit network6
	}

	# Announce ULA
	[ -n "$ula_prefix" ] && announce_prefix $ula_prefix $network

	# Announce all static prefixes
	config_list_foreach "$network" static_prefix announce_prefix $network
}


enable_downstream() {
	local network="$1"
	local device="$2"

	# Get IPv6 prefixes
	local length
	config_get length "$network" advertise_prefix
	[ -z "$length" ] && length=64
	[ "$length" -ne "0" ] && ubus call 6distributed newiface '{"network": "'"$network"'", "iface": "'"$device"'", "length": '"$length"'}'

	# Start RD & DHCPv6 service
	local pid="/var/run/ipv6-downstream-$network.pid"
	start_service "/usr/sbin/6relayd -Rserver -Dserver . $device" "$pid"

	# Try relaying if necessary
	restart_master_relay "$network"
}


enable_upstream() {
	local network="$1"
	local device="$2"
	
	# Configure device
	conf_set "$device" accept_ra 2
	conf_set "$device" forwarding 2
	
	# Trigger RS
	conf_set "$device" disable_ipv6 1
	conf_set "$device" disable_ipv6 0

	# Configure DHCPv6-client
	local dhcp6_opts="$device"

	# Configure DHCPv6-client (e.g. requested prefix)
	local request_prefix
	config_get request_prefix "$network" request_prefix
	[ -z "$request_prefix" ] && request_prefix="auto"
	[ "$request_prefix" != "no" ] && {
		[ "$request_prefix" == "auto" ] && request_prefix=0
		dhcp6_opts="-P$request_prefix $dhcp6_opts"
	}
	
	# Start DHCPv6 client
	local pid="/var/run/ipv6-upstream-$network.pid"
	start_service "/usr/sbin/odhcp6c -s/lib/ipv6/dhcpv6.sh $dhcp6_opts" "$pid"

	# Refresh RA on all interfaces
	for pid in /var/run/ipv6-downstream-*.pid; do
		kill -SIGUSR1 $(cat "$pid")
	done
}


