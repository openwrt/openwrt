#!/bin/sh
# Copyright (C) 2006 OpenWrt.org

# DEBUG="echo"

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

# sort the device list, drop duplicates
sort_list() {
	local arg="$*"
	(
		for item in $arg; do
			echo "$item"
		done
	) | sort -u
}

# Create the interface, if necessary.
# Return status 0 indicates that the setup_interface() call should continue
# Return status 1 means that everything is set up already.

prepare_interface() {
	local iface="$1"
	local config="$2"
	local vifmac="$3"

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
		[ "$proto" = none ] || ifconfig "$iface" 0.0.0.0

		# Change interface MAC address if requested
		[ -n "$vifmac" ] && {
			ifconfig "$iface" down
			ifconfig "$iface" hw ether "$vifmac" up
		}
	}

	# Setup VLAN interfaces
	add_vlan "$iface" && return 1
	ifconfig "$iface" 2>/dev/null >/dev/null || return 0

	# Setup bridging
	local iftype
	config_get iftype "$config" type
	case "$iftype" in
		bridge)
			[ -x /usr/sbin/brctl ] && {
				ifconfig "br-$config" 2>/dev/null >/dev/null && {
					local newdevs devices
					config_get devices "$config" device
					for dev in $(sort_list "$devices" "$iface"); do
						append newdevs "$dev"
					done
					uci_set_state network "$config" device "$newdevs"
					$DEBUG brctl addif "br-$config" "$iface"
					# Bridge existed already. No further processing necesary
				} || {
					local stp
					config_get_bool stp "$config" stp 0
					$DEBUG brctl addbr "br-$config"
					$DEBUG brctl setfd "br-$config" 0
					$DEBUG ifconfig "br-$config" up
					$DEBUG brctl addif "br-$config" "$iface"
					$DEBUG brctl stp "br-$config" $stp
					# Creating the bridge here will have triggered a hotplug event, which will
					# result in another setup_interface() call, so we simply stop processing
					# the current event at this point.
				}
				ifconfig "$iface" up 2>/dev/null >/dev/null
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
	uci_set_state network "$config" ifname "$ifname"
	uci_set_state network "$config" device "$device"
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

	local gateway ip6gw dns bcast
	config_get gateway "$config" gateway
	config_get ip6gw "$config" ip6gw
	config_get dns "$config" dns
	config_get bcast "$config" broadcast

	[ -z "$ipaddr" ] || $DEBUG ifconfig "$iface" "$ipaddr" netmask "$netmask" broadcast "${bcast:-+}"
	[ -z "$ip6addr" ] || $DEBUG ifconfig "$iface" add "$ip6addr"
	[ -z "$gateway" ] || $DEBUG route add default gw "$gateway" dev "$iface"
	[ -z "$ip6gw" ] || $DEBUG route -A inet6 add default gw "$ip6gw" dev "$iface"
	[ -z "$dns" ] || {
		for ns in $dns; do
			grep "$ns" /tmp/resolv.conf.auto 2>/dev/null >/dev/null || {
				echo "nameserver $ns" >> /tmp/resolv.conf.auto
			}
		done
	}

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

	iface="$iface:$ctr"
	set_interface_ifname "$config" "$iface"

	local proto
	config_get proto "$config" proto "static"
	case "${proto}" in
		static)
			setup_interface_static "$iface" "$config"
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
	grep "$iface:" /proc/net/dev > /dev/null && {
		local mtu macaddr
		config_get mtu "$config" mtu
		config_get macaddr "$config" macaddr
		[ -n "$macaddr" ] && $DEBUG ifconfig "$iface" down
		$DEBUG ifconfig "$iface" ${macaddr:+hw ether "$macaddr"} ${mtu:+mtu $mtu} up
	}
	set_interface_ifname "$config" "$iface"

	pidfile="/var/run/$iface.pid"
	[ -n "$proto" ] || config_get proto "$config" proto
	case "$proto" in
		static)
			setup_interface_static "$iface" "$config"
		;;
		dhcp)
			# prevent udhcpc from starting more than once
			lock "/var/lock/dhcp-$iface"
			local pid="$(cat "$pidfile" 2>/dev/null)"
			if [ -d "/proc/$pid" ] && grep udhcpc "/proc/${pid}/cmdline" >/dev/null 2>/dev/null; then
				lock -u "/var/lock/dhcp-$iface"
			else
				local ipaddr netmask hostname proto1 clientid
				config_get ipaddr "$config" ipaddr
				config_get netmask "$config" netmask
				config_get hostname "$config" hostname
				config_get proto1 "$config" proto
				config_get clientid "$config" clientid

				[ -z "$ipaddr" ] || \
					$DEBUG ifconfig "$iface" "$ipaddr" ${netmask:+netmask "$netmask"}

				# don't stay running in background if dhcp is not the main proto on the interface (e.g. when using pptp)
				local dhcpopts
				[ ."$proto1" != ."$proto" ] && dhcpopts="-n -q"
				$DEBUG eval udhcpc -t 0 -i "$iface" ${ipaddr:+-r $ipaddr} ${hostname:+-H $hostname} ${clientid:+-c $clientid} -b -p "$pidfile" ${dhcpopts:- -R &}
				lock -u "/var/lock/dhcp-$iface"
			fi
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
	[ "$proto" = none ] || {
		for ifn in `ifconfig | grep "^$iface:" | awk '{print $1}'`; do
			ifconfig "$ifn" down
		done
	}

	local aliases
	config_set "$config" aliases ""
	config_set "$config" alias_count 0
	config_foreach setup_interface_alias alias "$config" "$iface"
	config_get aliases "$config" aliases
	[ -z "$aliases" ] || uci_set_state network "$config" aliases "$aliases"
}

unbridge() {
	local dev="$1"
	local brdev

	[ -x /usr/sbin/brctl ] || return 0
	brctl show | grep "$dev" >/dev/null && {
		# interface is still part of a bridge, correct that

		for brdev in $(brctl show | awk '$2 ~ /^[0-9].*\./ { print $1 }'); do
			brctl delif "$brdev" "$dev" 2>/dev/null >/dev/null
		done
	}
}
