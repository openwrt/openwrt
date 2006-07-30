#!/bin/sh
# Copyright (C) 2006 OpenWrt.org

# DEBUG="echo"

find_config() {
	local type iface ifn
	for ifn in $interfaces; do
		config_get type "$ifn" type
		config_get iface "$ifn" ifname
		case "$type" in
			bridge)
				config_get iface "$ifn" ifnames
			;;
		esac
		for ifc in $iface; do
			[ "$ifc" = "$1" ] && {
				echo "$ifn"
				return 0
			}
		done
	done

	return 1;
}

scan_interfaces() {
	local mode type iface
	interfaces=
	config_cb() {
		config_get type "$CONFIG_SECTION" TYPE
		case "$type" in
			interface)
				config_get type "$CONFIG_SECTION" type
				config_get mode "$CONFIG_SECTION" proto
				case "$type" in
					bridge)
						config_get iface "$CONFIG_SECTION" ifname
						iface="${iface:-br-$CONFIG_SECTION}"
						config_set "$CONFIG_SECTION" ifname "$iface"
					;;
				esac
				append interfaces "$CONFIG_SECTION"
				( type "scan_$mode" ) >/dev/null 2>/dev/null && eval "scan_$mode '$CONFIG_SECTION'"
			;;
		esac
	}
	config_load network
}

add_vlan() {
	local vif="${1%\.*}"
	
	[ "$1" = "$vif" ] || ifconfig "$1" >/dev/null 2>/dev/null || {
		ifconfig "$vif" up 2>/dev/null >/dev/null || add_vlan "$vif"
		$DEBUG vconfig add "$vif" "${1##*\.}"
	}
}

setup_interface() {
	local iface="$1"
	local config="$2"
	local proto="$3"

	[ -n "$config" ] || {
		config=$(find_config "$iface")
		[ "$?" = 0 ] || return 1
	}

	[ -n "$proto" ] || {
		config_get proto "$config" proto
	}

	config_get iftype "$config" type
	
	# Setup VLAN interfaces
	add_vlan "$iface"

	# Setup bridging
	case "$iftype" in
		bridge)
			config_get bridge_ifname "$config" ifname
			ifconfig "$iface" up 2>/dev/null >/dev/null
			ifconfig "$bridge_ifname" 2>/dev/null >/dev/null && {
				$DEBUG brctl addif "$bridge_ifname" "$iface"
				return 0
			} || {
				$DEBUG brctl addbr "$bridge_ifname"
				$DEBUG brctl setfd "$bridge_ifname" 0
				$DEBUG brctl addif "$bridge_ifname" "$iface"
				iface="$bridge_ifname"
			}
		;;
	esac
	
	# Interface settings
	config_get mtu "$config" mtu
	$DEBUG ifconfig "$iface" ${mtu:+mtu $mtu} up

	pidfile="/var/run/$iface.pid"
	case "$proto" in
		static)
			config_get ipaddr "$config" ipaddr
			config_get netmask "$config" netmask
			[ -z "$ipaddr" -o -z "$netmask" ] && return 1
			
			config_get ip6addr "$config" ip6addr
			config_get gateway "$config" gateway
			config_get dns "$config" dns
			
			$DEBUG ifconfig "$iface" "$ipaddr" netmask "$netmask"
			[ -z "$gateway" ] || route add default gw "$gateway"
			[ -z "$dns" -o -f /tmp/resolv.conf ] || {
				for ns in $dns; do
					echo "nameserver $ns" >> /tmp/resolv.conf
				done
			}

			env -i ACTION="ifup" INTERFACE="config" DEVICE="$iface" PROTO=static /sbin/hotplug "iface" &
		;;
		dhcp)
			pid="$(cat "$pidfile" 2>/dev/null)"
			[ -n "$pid" -a -d "/proc/$pid" ] && kill -9 "$pid"

			config_get ipaddr "$config" ipaddr
			config_get netmask "$config" netmask
			config_get hostname "$config" hostname
			config_get proto1 "$config" proto

			[ -z "$ipaddr" ] || \
				$DEBUG ifconfig "$iface" "$ipaddr" ${netmask:+netmask "$netmask"}

			# don't stay running in background if dhcp is not the main proto on the interface (e.g. when using pptp)
			[ "$proto1" != "$proto" ] && dhcpopts="-n -q"
			$DEBUG udhcpc -i "$iface" ${ipaddr:+-r $ipaddr} ${hostname:+-H $hostname} -b -p "$pidfile" ${dhcpopts:- -R &}
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

