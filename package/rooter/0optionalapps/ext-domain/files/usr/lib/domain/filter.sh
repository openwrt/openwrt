#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Domain Filter " "$@"
}

handle_ipset() {
	local ips=$1
	uci add_list dhcp.@dnsmasq[0].ipset='/'$ips'/filter,filter6'
}

do_ipset() {
	local config=$1
	local ipset

	config_list_foreach "$config" ipset handle_ipset
}

sleep 3

uci -q delete dhcp.@dnsmasq[0].ipset
config_load filter
config_foreach do_ipset filter
uci commit dhcp
/etc/init.d/dnsmasq restart