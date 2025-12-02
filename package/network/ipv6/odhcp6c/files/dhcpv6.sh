#!/bin/sh

. /lib/functions.sh
. ../netifd-proto.sh
. /lib/config/uci.sh
init_proto "$@"

proto_dhcpv6_init_config() {
	renew_handler=1

	proto_config_add_string 'reqaddress:or("try","force","none")'
	proto_config_add_string 'reqprefix:or("auto","no",range(0, 64))'
	proto_config_add_string clientid
	proto_config_add_string 'reqopts:list(uinteger)'
	proto_config_add_string 'defaultreqopts:bool'
	proto_config_add_string 'noslaaconly:bool'
	proto_config_add_string 'forceprefix:bool'
	proto_config_add_string 'extendprefix:bool'
	proto_config_add_string 'norelease:bool'
	proto_config_add_string 'noserverunicast:bool'
	proto_config_add_string 'noclientfqdn:bool'
	proto_config_add_string 'noacceptreconfig:bool'
	proto_config_add_array 'ip6prefix:list(ip6addr)'
	proto_config_add_string iface_dslite
	proto_config_add_string zone_dslite
	proto_config_add_string encaplimit_dslite
	proto_config_add_int mtu_dslite
	proto_config_add_string iface_map
	proto_config_add_string zone_map
	proto_config_add_string encaplimit_map
	proto_config_add_string iface_464xlat
	proto_config_add_string zone_464xlat
	proto_config_add_string zone
	proto_config_add_string 'ip6ifaceid:ip6addr'
	proto_config_add_string "userclass"
	proto_config_add_string "vendorclass"
	proto_config_add_array "sendopts:list(string)"
	proto_config_add_boolean delegate
	proto_config_add_int skpriority
	proto_config_add_int "soltimeout"
	proto_config_add_boolean fakeroutes
	proto_config_add_boolean sourcefilter
	proto_config_add_boolean keep_ra_dnslifetime
	proto_config_add_int "ra_holdoff"
	proto_config_add_boolean verbose
	proto_config_add_boolean dynamic
}

proto_dhcpv6_add_prefix() {
	append "$3" "$1"
}

proto_dhcpv6_add_sendopts() {
	[ -n "$1" ] && append "$3" "-x$1"
}

proto_dhcpv6_setup() {
	local config="$1"
	local iface="$2"

	local reqaddress reqprefix clientid reqopts defaultreqopts
	local noslaaconly forceprefix extendprefix norelease
	local noserverunicast noclientfqdn noacceptreconfig iface_dslite
	local iface_map iface_464xlat ip6ifaceid userclass vendorclass
	local delegate zone_dslite zone_map zone_464xlat zone encaplimit_dslite
	local encaplimit_map skpriority soltimeout fakeroutes sourcefilter
	local keep_ra_dnslifetime ra_holdoff verbose mtu_dslite dynamic

	local ip6prefix ip6prefixes

	json_get_vars reqaddress reqprefix clientid reqopts defaultreqopts
	json_get_vars noslaaconly forceprefix extendprefix norelease
	json_get_vars noserverunicast noclientfqdn noacceptreconfig iface_dslite
	json_get_vars iface_map iface_464xlat ip6ifaceid userclass vendorclass
	json_get_vars delegate zone_dslite zone_map zone_464xlat zone encaplimit_dslite
	json_get_vars encaplimit_map skpriority soltimeout fakeroutes sourcefilter
	json_get_vars keep_ra_dnslifetime ra_holdoff verbose mtu_dslite dynamic

	json_for_each_item proto_dhcpv6_add_prefix ip6prefix ip6prefixes

	# Configure
	local sendopts
	local opts=""
	[ -n "$reqaddress" ] && append opts "-N$reqaddress"

	[ -z "$reqprefix" -o "$reqprefix" = "auto" ] && reqprefix=0
	[ "$reqprefix" != "no" ] && append opts "-P$reqprefix"

	[ -z "$clientid" ] && clientid="$(uci_get network @globals[0] dhcp_default_duid)"
	[ -n "$clientid" ] && append opts "-c$clientid"

	[ "$defaultreqopts" = "0" ] && append opts "-R"

	[ "$noslaaconly" = "1" ] && append opts "-S"

	[ "$forceprefix" = "1" ] && append opts "-F"

	[ "$norelease" = "1" ] && append opts "-k"

	[ "$noserverunicast" = "1" ] && append opts "-U"

	[ "$noclientfqdn" = "1" ] && append opts "-f"

	[ "$noacceptreconfig" = "1" ] && append opts "-a"

	[ -z "$ip6ifaceid" ] && json_get_var ip6ifaceid ifaceid
	[ -n "$ip6ifaceid" ] && append opts "-i$ip6ifaceid"

	[ -n "$vendorclass" ] && append opts "-V$vendorclass"

	[ -n "$userclass" ] && append opts "-u$userclass"

	[ "$keep_ra_dnslifetime" = "1" ] && append opts "-L"

	[ -n "$skpriority" ] && append opts "-K$skpriority"

	[ -n "$ra_holdoff" ] && append opts "-m$ra_holdoff"

	[ "$verbose" = "1" ] && append opts "-v"

	json_for_each_item proto_dhcpv6_add_sendopts sendopts opts

	# Dynamically add OROs to support loaded packages.
	json_load "$(ubus call network get_proto_handlers)"
	json_get_var handler_map map
	json_get_var handler_dslite dslite

	[ -n "$handler_dslite" ] && append reqopts "64"
	[ -n "$handler_map" ] && append reqopts "94"
	[ -n "$handler_map" ] && append reqopts "95"
	[ -n "$handler_map" ] && append reqopts "96"

	local opt
	for opt in $reqopts; do
		append opts "-r$opt"
	done

	append opts "-t${soltimeout:-120}"

	[ -n "$ip6prefixes" ] && proto_export "USERPREFIX=$ip6prefixes"
	[ -n "$iface_dslite" ] && proto_export "IFACE_DSLITE=$iface_dslite"
	[ -n "$mtu_dslite" ] && proto_export "MTU_DSLITE=$mtu_dslite"
	[ -n "$iface_map" ] && proto_export "IFACE_MAP=$iface_map"
	[ -n "$iface_464xlat" ] && proto_export "IFACE_464XLAT=$iface_464xlat"
	[ "$delegate" = "0" ] && proto_export "IFACE_DSLITE_DELEGATE=0"
	[ "$delegate" = "0" ] && proto_export "IFACE_MAP_DELEGATE=0"
	[ "$delegate" = "0" ] && proto_export "IFACE_464XLAT_DELEGATE=0"
	[ -n "$zone_dslite" ] && proto_export "ZONE_DSLITE=$zone_dslite"
	[ -n "$zone_map" ] && proto_export "ZONE_MAP=$zone_map"
	[ -n "$zone_464xlat" ] && proto_export "ZONE_464XLAT=$zone_464xlat"
	[ -n "$zone" ] && proto_export "ZONE=$zone"
	[ -n "$encaplimit_dslite" ] && proto_export "ENCAPLIMIT_DSLITE=$encaplimit_dslite"
	[ -n "$encaplimit_map" ] && proto_export "ENCAPLIMIT_MAP=$encaplimit_map"
	[ "$fakeroutes" != "0" ] && proto_export "FAKE_ROUTES=1"
	[ "$sourcefilter" = "0" ] && proto_export "NOSOURCEFILTER=1"
	[ "$extendprefix" = "1" ] && proto_export "EXTENDPREFIX=1"

	if [ "$dynamic" = 0 ]; then
		proto_export "DYNAMIC=0"
	else
		proto_export "DYNAMIC=1"
	fi

	proto_export "INTERFACE=$config"
	proto_run_command "$config" odhcp6c \
		-s /lib/netifd/dhcpv6.script \
		$opts $iface
}

proto_dhcpv6_renew() {
	local interface="$1"
	# SIGUSR1 forces odhcp6c to renew its lease
	local sigusr1="$(kill -l SIGUSR1)"
	[ -n "$sigusr1" ] && proto_kill_command "$interface" $sigusr1
}

proto_dhcpv6_teardown() {
	local interface="$1"
	proto_kill_command "$interface"
}

add_protocol dhcpv6

