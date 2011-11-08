# relay.sh - Abstract relayd protocol backend
# Copyright (c) 2011 OpenWrt.org

# Hook into scan_interfaces() to synthesize a .device option
# This is needed for /sbin/ifup to properly dispatch control
# to setup_interface_relay() even if no .ifname is set in
# the configuration.
scan_relay() {
	config_set "$1" device "relay-$1"
}

# No coldplugging needed, relayd will be restarted if one of
# the member interfaces goes up or down
#coldplug_interface_relay() {
#	setup_interface_relay "relay-$1" "$1"
#}

setup_interface_relay() {
	local iface="$1"
	local cfg="$2"
	local link="relay-$cfg"

	local args=""
	local ifaces=""

	resolve_ifname() {
		grep -qs "^ *$1:" /proc/net/dev && {
			append args "-I $1"
			append ifaces "$1"
		}
	}

	resolve_network() {
		local ifn
		config_get ifn "$1" ifname
		resolve_ifname "$ifn"
	}

	local net networks
	config_get networks "$cfg" network
	for net in $networks; do
		resolve_network "$net"
	done

	local ifn ifnames
	config_get ifnames "$cfg" ifname
	for ifn in $ifnames; do
		resolve_ifname "$ifn"
	done

	local ipaddr
	config_get ipaddr "$cfg" ipaddr
	[ -n "$ipaddr" ] && append args "-L $ipaddr"

	local gateway
	config_get gateway "$cfg" gateway
	[ -n "$gateway" ] && append args "-G $gateway"

	local expiry # = 30
	config_get expiry "$cfg" expiry
	[ -n "$expiry" ] && append args "-t $expiry"

	local retry # = 5
	config_get retry "$cfg" retry
	[ -n "$retry" ] && append args "-p $retry"

	local table # = 16800
	config_get table "$cfg" table
	[ -n "$table" ] && append args "-T $table"

	local fwd_bcast # = 1
	config_get_bool fwd_bcast "$cfg" forward_bcast 1
	[ $fwd_bcast -eq 1 ] && append args "-B"

	local fwd_dhcp # = 1
	config_get_bool fwd_dhcp "$cfg" forward_dhcp 1
	[ $fwd_dhcp -eq 1 ] && append args "-D"

	SERVICE_DAEMONIZE=1 \
	SERVICE_WRITE_PID=1 \
	SERVICE_PID_FILE="/var/run/$link.pid" \
	service_start /usr/sbin/relayd $args

	uci_set_state network "$cfg" device "$ifaces"

	env -i ACTION="ifup" DEVICE="$link" INTERFACE="$cfg" PROTO="relay" \
		/sbin/hotplug-call iface
}

stop_interface_relay() {
	local cfg="$1"
	local link="relay-$cfg"

	env -i ACTION="ifdown" DEVICE="$link" INTERFACE="$cfg" PROTO="relay" \
		/sbin/hotplug-call iface

	SERVICE_PID_FILE="/var/run/$link.pid" \
	service_stop /usr/sbin/relayd
}

