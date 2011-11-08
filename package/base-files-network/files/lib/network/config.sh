#!/bin/sh
# Copyright (C) 2006-2011 OpenWrt.org

# DEBUG="echo"

do_sysctl() {
	[ -n "$2" ] && \
		sysctl -n -e -w "$1=$2" >/dev/null || \
		sysctl -n -e "$1"
}

map_sysctls() {
	local cfg="$1"
	local ifn="$2"

	local fam
	for fam in ipv4 ipv6; do
		if [ -d /proc/sys/net/$fam ]; then
			local key
			for key in /proc/sys/net/$fam/*/$ifn/*; do
				local val
				config_get val "$cfg" "${fam}_${key##*/}"
				[ -n "$val" ] && echo -n "$val" > "$key"
			done
		fi
	done
}

find_config() {
	local iftype device iface ifaces ifn
	for ifn in $interfaces; do
		config_get iftype "$ifn" type
		config_get iface "$ifn" ifname
		case "$iftype" in
			bridge) config_get ifaces "$ifn" ifnames;;
		esac
		config_get device "$ifn" device
		for ifc in $device $iface $ifaces; do
			[ ."$ifc" = ."$1" ] && {
				echo "$ifn"
				return 0
			}
		done
	done

	return 1;
}

scan_interfaces() {
	local cfgfile="${1:-network}"
	interfaces=
	config_cb() {
		case "$1" in
			interface)
				config_set "$2" auto 1
			;;
		esac
		local iftype ifname device proto
		config_get iftype "$CONFIG_SECTION" TYPE
		case "$iftype" in
			interface)
				append interfaces "$CONFIG_SECTION"
				config_get proto "$CONFIG_SECTION" proto
				config_get iftype "$CONFIG_SECTION" type
				config_get ifname "$CONFIG_SECTION" ifname
				config_get device "$CONFIG_SECTION" device "$ifname"
				config_set "$CONFIG_SECTION" device "$device"
				case "$iftype" in
					bridge)
						config_set "$CONFIG_SECTION" ifnames "$device"
						config_set "$CONFIG_SECTION" ifname br-"$CONFIG_SECTION"
					;;
				esac
				( type "scan_$proto" ) >/dev/null 2>/dev/null && eval "scan_$proto '$CONFIG_SECTION'"
			;;
		esac
	}
	config_load "${cfgfile}"
}

add_vlan() {
	local vif="${1%\.*}"

	[ "$1" = "$vif" ] || ifconfig "$1" >/dev/null 2>/dev/null || {
		ifconfig "$vif" up 2>/dev/null >/dev/null || add_vlan "$vif"
		$DEBUG vconfig add "$vif" "${1##*\.}"
		return 0
	}
	return 1
}

# add dns entries if they are not in resolv.conf yet
add_dns() {
	local cfg="$1"; shift

	remove_dns "$cfg"

	# We may be called by pppd's ip-up which has a nonstandard umask set.
	# Create an empty file here and force its permission to 0644, otherwise
	# dnsmasq will not be able to re-read the resolv.conf.auto .
	[ ! -f /tmp/resolv.conf.auto ] && {
		touch /tmp/resolv.conf.auto
		chmod 0644 /tmp/resolv.conf.auto
	}

	local dns
	local add
	for dns in "$@"; do
		grep -qsE "^nameserver ${dns//./\\.}$" /tmp/resolv.conf.auto || {
			add="${add:+$add }$dns"
			echo "nameserver $dns" >> /tmp/resolv.conf.auto
		}
	done

	[ -n "$cfg" ] && {
		uci_toggle_state network "$cfg" dns "$add"
		uci_toggle_state network "$cfg" resolv_dns "$add"
	}
}

# remove dns entries of the given iface
remove_dns() {
	local cfg="$1"

	[ -n "$cfg" ] && {
		[ -f /tmp/resolv.conf.auto ] && {
			local dns=$(uci_get_state network "$cfg" resolv_dns)
			for dns in $dns; do
				sed -i -e "/^nameserver ${dns//./\\.}$/d" /tmp/resolv.conf.auto
			done
		}

		uci_revert_state network "$cfg" dns
		uci_revert_state network "$cfg" resolv_dns
	}
}

# sort the device list, drop duplicates
sort_list() {
	local arg="$*"
	(
		for item in $arg; do
			echo "$item"
		done
	) | sort -u
}

prepare_interface_bridge() {
	return 0
}

# Create the interface, if necessary.
# Return status 0 indicates that the setup_interface() call should continue
# Return status 1 means that everything is set up already.

prepare_interface() {
	local iface="$1"
	local config="$2"
	local macaddr="$3"

	# if we're called for the bridge interface itself, don't bother trying
	# to create any interfaces here. The scripts have already done that, otherwise
	# the bridge interface wouldn't exist.
	[ "br-$config" = "$iface" -o -e "$iface" ] && return 0;

	ifconfig "$iface" 2>/dev/null >/dev/null && {
		local proto
		config_get proto "$config" proto

		# make sure the interface is removed from any existing bridge and deconfigured,
		# (deconfigured only if the interface is not set to proto=none)
		unbridge "$iface"

		local mtu macaddr txqueuelen
		config_get mtu "$config" mtu
		[ -n "$macaddr" ] || config_get macaddr "$config" macaddr
		config_get txqueuelen "$config" txqueuelen
		[ -n "$macaddr" ] && $DEBUG ifconfig "$iface" down
		$DEBUG ifconfig "$iface" ${macaddr:+hw ether "$macaddr"} ${mtu:+mtu $mtu} ${txqueuelen:+txqueuelen $txqueuelen} up

		[ "$proto" = none ] || ifconfig "$iface" 0.0.0.0

		# Apply sysctl settings
		map_sysctls "$config" "$iface"
	}

	# Setup VLAN interfaces
	add_vlan "$iface" && return 1
	ifconfig "$iface" 2>/dev/null >/dev/null || return 0

	# Setup bridging
	local iftype
	config_get iftype "$config" type
	case "$iftype" in
		bridge)
			local macaddr
			config_get macaddr "$config" macaddr
			[ -x /usr/sbin/brctl ] && {
				ifconfig "br-$config" 2>/dev/null >/dev/null && {
					local newdevs devices
					config_get devices "$config" device
					for dev in $(sort_list "$devices" "$iface"); do
						append newdevs "$dev"
					done
					uci_toggle_state network "$config" device "$newdevs"
					$DEBUG ifconfig "$iface" 0.0.0.0
					$DEBUG do_sysctl "net.ipv6.conf.$iface.disable_ipv6" 1
					$DEBUG brctl addif "br-$config" "$iface"
					# Bridge existed already. No further processing necesary
				} || {
					local stp igmp_snooping
					config_get_bool stp "$config" stp 0
					config_get_bool igmp_snooping "$config" igmp_snooping 1
					$DEBUG brctl addbr "br-$config"
					$DEBUG brctl setfd "br-$config" 0
					$DEBUG ifconfig "$iface" 0.0.0.0
					$DEBUG do_sysctl "net.ipv6.conf.$iface.disable_ipv6" 1
					$DEBUG brctl addif "br-$config" "$iface"
					$DEBUG brctl stp "br-$config" $stp
					[ -z "$macaddr" ] && macaddr="$(cat /sys/class/net/$iface/address)"
					echo $igmp_snooping > /sys/devices/virtual/net/br-$config/bridge/multicast_snooping 2>/dev/null
					$DEBUG ifconfig "br-$config" hw ether $macaddr up
					# Creating the bridge here will have triggered a hotplug event, which will
					# result in another setup_interface() call, so we simply stop processing
					# the current event at this point.
				}
				ifconfig "$iface" ${macaddr:+hw ether "${macaddr}"} up 2>/dev/null >/dev/null
				return 1
			}
		;;
	esac
	return 0
}

set_interface_ifname() {
	local config="$1"
	local ifname="$2"

	local device
	config_get device "$1" device
	uci_toggle_state network "$config" ifname "$ifname"
	uci_toggle_state network "$config" device "$device"
}

setup_interface_none() {
	env -i ACTION="ifup" INTERFACE="$2" DEVICE="$1" PROTO=none /sbin/hotplug-call "iface" &
}

setup_interface_static() {
	local iface="$1"
	local config="$2"

	local ipaddr netmask ip6addr
	config_get ipaddr "$config" ipaddr
	config_get netmask "$config" netmask
	config_get ip6addr "$config" ip6addr
	[ -z "$ipaddr" -o -z "$netmask" ] && [ -z "$ip6addr" ] && return 1

	local gateway ip6gw dns bcast metric
	config_get gateway "$config" gateway
	config_get ip6gw "$config" ip6gw
	config_get dns "$config" dns
	config_get bcast "$config" broadcast
	config_get metric "$config" metric

	case "$ip6addr" in
		*/*) ;;
		*:*) ip6addr="$ip6addr/64" ;;
	esac

	[ -z "$ipaddr" ] || $DEBUG ifconfig "$iface" "$ipaddr" netmask "$netmask" broadcast "${bcast:-+}"
	[ -z "$ip6addr" ] || $DEBUG ifconfig "${iface%:*}" add "$ip6addr"
	[ -z "$gateway" ] || $DEBUG route add default gw "$gateway" ${metric:+metric $metric} dev "$iface"
	[ -z "$ip6gw" ] || $DEBUG route -A inet6 add default gw "$ip6gw" ${metric:+metric $metric} dev "${iface%:*}"
	[ -z "$dns" ] || add_dns "$config" $dns

	config_get type "$config" TYPE
	[ "$type" = "alias" ] && return 0

	env -i ACTION="ifup" INTERFACE="$config" DEVICE="$iface" PROTO=static /sbin/hotplug-call "iface" &
}

setup_interface_alias() {
	local config="$1"
	local parent="$2"
	local iface="$3"

	local cfg
	config_get cfg "$config" interface
	[ "$parent" == "$cfg" ] || return 0

	# parent device and ifname
	local p_device p_type
	config_get p_device "$cfg" device
	config_get p_type   "$cfg" type

	# select alias ifname
	local layer use_iface
	config_get layer "$config" layer 2
	case "$layer:$p_type" in
		# layer 3: e.g. pppoe-wan or pptp-vpn
		3:*)      use_iface="$iface" ;;

		# layer 2 and parent is bridge: e.g. br-wan
		2:bridge) use_iface="br-$cfg" ;;

		# layer 1: e.g. eth0 or ath0
		*)        use_iface="$p_device" ;;
	esac

	# alias counter
	local ctr
	config_get ctr "$parent" alias_count 0
	ctr="$(($ctr + 1))"
	config_set "$parent" alias_count "$ctr"

	# alias list
	local list
	config_get list "$parent" aliases
	append list "$config"
	config_set "$parent" aliases "$list"

	use_iface="$use_iface:$ctr"
	set_interface_ifname "$config" "$use_iface"

	local proto
	config_get proto "$config" proto "static"
	case "${proto}" in
		static)
			setup_interface_static "$use_iface" "$config"
		;;
		*)
			echo "Unsupported type '$proto' for alias config '$config'"
			return 1
		;;
	esac
}

setup_interface() {
	local iface="$1"
	local config="$2"
	local proto="$3"
	local vifmac="$4"

	[ -n "$config" ] || {
		config=$(find_config "$iface")
		[ "$?" = 0 ] || return 1
	}

	prepare_interface "$iface" "$config" "$vifmac" || return 0

	[ "$iface" = "br-$config" ] && {
		# need to bring up the bridge and wait a second for
		# it to switch to the 'forwarding' state, otherwise
		# it will lose its routes...
		ifconfig "$iface" up
		sleep 1
	}

	# Interface settings
	set_interface_ifname "$config" "$iface"

	[ -n "$proto" ] || config_get proto "$config" proto
	case "$proto" in
		static)
			setup_interface_static "$iface" "$config"
		;;
		dhcp)
			# kill running udhcpc instance
			local pidfile="/var/run/dhcp-${iface}.pid"

			SERVICE_PID_FILE="$pidfile" \
			service_stop udhcpc

			local ipaddr netmask hostname proto1 clientid vendorid broadcast reqopts
			config_get ipaddr "$config" ipaddr
			config_get netmask "$config" netmask
			config_get hostname "$config" hostname
			config_get proto1 "$config" proto
			config_get clientid "$config" clientid
			config_get vendorid "$config" vendorid
			config_get_bool broadcast "$config" broadcast 0
			config_get reqopts "$config" reqopts

			[ -z "$ipaddr" ] || \
				$DEBUG ifconfig "$iface" "$ipaddr" ${netmask:+netmask "$netmask"}

			# additional request options
			local opt dhcpopts
			for opt in $reqopts; do
				append dhcpopts "-O $opt"
			done

			# don't stay running in background if dhcp is not the main proto on the interface (e.g. when using pptp)
			[ "$proto1" != "$proto" ] && append dhcpopts "-n -q" || append dhcpopts "-O rootpath -R &"
			[ "$broadcast" = 1 ] && broadcast="-O broadcast" || broadcast=

			SERVICE_PID_FILE="$pidfile" \
			service_start /sbin/udhcpc -t 0 -i "$iface" \
				${ipaddr:+-r $ipaddr} \
				${hostname:+-H $hostname} \
				${clientid:+-c $clientid} \
				${vendorid:+-V $vendorid} \
				-b -p "$pidfile" $broadcast \
				${dhcpopts}
		;;
		none)
			setup_interface_none "$iface" "$config"
		;;
		*)
			if ( eval "type setup_interface_$proto" ) >/dev/null 2>/dev/null; then
				eval "setup_interface_$proto '$iface' '$config' '$proto'"
			else
				echo "Interface type $proto not supported."
				return 1
			fi
		;;
	esac
}

stop_interface_dhcp() {
	local config="$1"

	local ifname
	config_get ifname "$config" ifname

	local lock="/var/lock/dhcp-${ifname}"
	[ -f "$lock" ] && lock -u "$lock"

	remove_dns "$config"

	SERVICE_PID_FILE="/var/run/dhcp-${ifname}.pid" \
	service_stop /sbin/udhcpc

	uci -P /var/state revert "network.$config"
}

unbridge() {
	local dev="$1"
	local brdev

	[ -x /usr/sbin/brctl ] || return 0
	brctl show 2>/dev/null | grep "$dev" >/dev/null && {
		# interface is still part of a bridge, correct that

		for brdev in $(brctl show | awk '$2 ~ /^[0-9].*\./ { print $1 }'); do
			brctl delif "$brdev" "$dev" 2>/dev/null >/dev/null
			do_sysctl "net.ipv6.conf.$dev.disable_ipv6" 0
		done
	}
}
