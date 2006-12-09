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
			[ "$ifc" = "$1" ] && {
				echo "$ifn"
				return 0
			}
		done
	done

	return 1;
}

scan_interfaces() {
	local mode iftype iface ifname device
	interfaces=
	config_cb() {
		case "$1" in
			interface)
				config_set "$2" auto 1
			;;
		esac
		config_get iftype "$CONFIG_SECTION" TYPE
		case "$iftype" in
			interface)
				config_get proto "$CONFIG_SECTION" proto
				append interfaces "$CONFIG_SECTION"
				config_get iftype "$CONFIG_SECTION" type
				config_get ifname "$CONFIG_SECTION" ifname
				config_set "$CONFIG_SECTION" device "$ifname"
				case "$iftype" in
					bridge)
						config_set "$CONFIG_SECTION" ifnames "$ifname"
						config_set "$CONFIG_SECTION" ifname br-"$CONFIG_SECTION"
					;;
				esac
				( type "scan_$proto" ) >/dev/null 2>/dev/null && eval "scan_$proto '$CONFIG_SECTION'"
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
	local proto
	local macaddr

	[ -n "$config" ] || {
		config=$(find_config "$iface")
		[ "$?" = 0 ] || return 1
	}

	proto="${3:-$(config_get "$config" proto)}"
	config_get iftype "$config" type
	
	ifconfig "$iface" 2>/dev/null >/dev/null && {
		# make sure the interface is removed from any existing bridge and brought down
		ifconfig "$iface" down
		unbridge "$iface"
	}

	# Setup VLAN interfaces
	add_vlan "$iface"

	# Setup bridging
	case "$iftype" in
		bridge)
			ifconfig "$iface" up 2>/dev/null >/dev/null
			ifconfig "br-$config" 2>/dev/null >/dev/null && {
				$DEBUG brctl addif "br-$config" "$iface"
				return 0
			} || {
				$DEBUG brctl addbr "br-$config"
				$DEBUG brctl setfd "br-$config" 0
				$DEBUG brctl addif "br-$config" "$iface"
				iface="br-$config"
			}
		;;
	esac
	
	# Interface settings
	config_get mtu "$config" mtu
	config_get macaddr "$config" macaddr
	$DEBUG ifconfig "$iface" ${macaddr:+hw ether "$macaddr"} ${mtu:+mtu $mtu} up

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
			$DEBUG ifconfig "$iface" inet6 add "$ip6addr" 
			[ -z "$gateway" ] || route add default gw "$gateway"
			[ -z "$dns" -o -f /tmp/resolv.conf ] || {
				for ns in $dns; do
					echo "nameserver $ns" >> /tmp/resolv.conf
				done
			}

			env -i ACTION="ifup" INTERFACE="$config" DEVICE="$iface" PROTO=static /sbin/hotplug "iface" &
		;;
		dhcp)
			# prevent udhcpc from starting more than once
			lock "/var/lock/dhcp-$iface"
			pid="$(cat "$pidfile" 2>/dev/null)"
			[ -d "/proc/$pid" ] && grep udhcpc "/proc/${pid}/cmdline" >/dev/null 2>/dev/null && {
				lock -u "/var/lock/dhcp-$iface"
				return 0
			}

			config_get ipaddr "$config" ipaddr
			config_get netmask "$config" netmask
			config_get hostname "$config" hostname
			config_get proto1 "$config" proto

			[ -z "$ipaddr" ] || \
				$DEBUG ifconfig "$iface" "$ipaddr" ${netmask:+netmask "$netmask"}

			# don't stay running in background if dhcp is not the main proto on the interface (e.g. when using pptp)
			[ "$proto1" != "$proto" ] && dhcpopts="-n -q"
			$DEBUG eval udhcpc -t 0 -i "$iface" ${ipaddr:+-r $ipaddr} ${hostname:+-H $hostname} -b -p "$pidfile" ${dhcpopts:- -R &}
			lock -u "/var/lock/dhcp-$iface"
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

unbridge() {
	local dev="$1"
	local brdev

	brctl show | grep "$dev" >/dev/null && {
		# interface is still part of a bridge, correct that

		for brdev in $(brctl show | awk '$2 ~ /^[0-9].*\./ { print $1 }'); do
			brctl delif "$brdev" "$dev" 2>/dev/null >/dev/null
		done
	}
}
