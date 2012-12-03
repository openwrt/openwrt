#!/bin/sh
# Copyright (c) 2012 OpenWrt.org
. /lib/ipv6/support.sh
. /lib/netifd/netifd-proto.sh

local device="$1"
local state="$2"
local network=""

resolve_network network "$device"

# Unknown network
[ -z "$network" ] && exit 0


# Announce prefixes
for prefix in $PREFIXES; do
	announce_prefix "$prefix" "$network"
done

for prefix in $PREFIXES_LOST; do
	announce_prefix "$prefix" "$network" delprefix
done


# Enable relaying if requested
local prefix_fallback
config_get prefix_fallback "$network" prefix_fallback
[ "$prefix_fallback" == "relay" -a -z "$PREFIXES" -a "$state" != "unbound" ] &&
	restart_relay "$network" "fallback"

# Disable relay if requested
[ "$prefix_fallback" != "relay" -o -n "$PREFIXES" -o "$state" == "unbound" ] &&
	restart_relay "$network"


# Operations in case of success
[ "$state" == "timeout" || "$state" == "unbound" ] && exit 0

local peerdns
config_get_bool peerdns "$network" peerdns 0
[ "peerdns" -eq "1" ] && {
	proto_init_update "*" 1
	for server in $RDNSS; do
		proto_add_dns_server "$server"
	done
	for domain in $DOMAINS; do
		proto_add_dns_search "$domain"
	done
	proto_send_update "$network"
}

