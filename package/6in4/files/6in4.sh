# 6in4.sh - IPv6-in-IPv4 tunnel backend
# Copyright (c) 2010 OpenWrt.org

find_6in4_wanif() {
	local if=$(ip -4 r l e 0/0); if="${if#default via * dev }"; if="${if%% *}"
	[ -n "$if" ] && grep -qs "^ *$if:" /proc/net/dev && echo "$if"
}

find_6in4_wanip() {
	local ip=$(ip -4 a s dev "$1"); ip="${ip#*inet }"
	echo "${ip%%/[0-9]* brd *}"
}

# Hook into scan_interfaces() to synthesize a .device option
# This is needed for /sbin/ifup to properly dispatch control
# to setup_interface_6in4() even if no .ifname is set in
# the configuration.
scan_6in4() {
	config_set "$1" device "6in4-$1"
}

coldplug_interface_6in4() {
	setup_interface_6in4 "6in4-$1" "$1"
}

setup_interface_6in4() {
	local iface="$1"
	local cfg="$2"
	local link="6in4-$cfg"

	local local4
	config_get local4 "$cfg" ipaddr

	local remote4
	config_get remote4 "$cfg" peeraddr

	local local6
	config_get local6 "$cfg" ip6addr

	local mtu
	config_get mtu "$cfg" mtu

	local ttl
	config_get ttl "$cfg" ttl

	local defaultroute
	config_get_bool defaultroute "$cfg" defaultroute 1

	# If local4 is unset, guess local IPv4 address from the
	# interface used by the default route.
	[ -z "$local4" ] && {
		local wanif=$(find_6in4_wanif)
		[ -n "$wanif" ] && {
			local4=$(find_6in4_wanip "$wanif")
			uci_set_state network "$cfg" wan_device "$wanif"
		}
	}

	[ -n "$local4" ] && {
		# creating the tunnel below will trigger a net subsystem event
		# prevent it from touching or iface by disabling .auto here
		uci_set_state network "$cfg" ifname $link
		uci_set_state network "$cfg" auto 0

		ip tunnel add $link mode sit remote $remote4 local $local4 ttl 255
		ip link set $link up
		ip link set mtu ${mtu:-1280} dev $link
		ip tunnel change $link ttl ${ttl:-64}
		ip addr add $local6 dev $link

		uci_set_state network "$cfg" ipaddr $local4
		uci_set_state network "$cfg" ip6addr $local6

		[ "$defaultroute" = 1 ] && {
			ip -6 route add ::/0 dev $link
			uci_set_state network "$cfg" defaultroute 1
		}

		env -i ACTION="ifup" INTERFACE="$cfg" DEVICE="$link" PROTO=6in4 /sbin/hotplug-call "iface" &
	} || {
		echo "Cannot determine local IPv4 address for 6in4 tunnel $cfg - skipping"
	}
}

stop_interface_6in4() {
	local cfg="$1"
	local link="6in4-$cfg"

	local local6=$(uci_get_state network "$cfg" ip6addr)
	local defaultroute=$(uci_get_state network "$cfg" defaultroute)

	grep -qs "^ *$link:" /proc/net/dev && {
		env -i ACTION="ifdown" INTERFACE="$cfg" DEVICE="$link" PROTO=6in4 /sbin/hotplug-call "iface" &

		[ "$defaultroute" = "1" ] && {
			ip -6 route del ::/0 dev $link
		}

		ip addr del $local6 dev $link
		ip link set $link down
		ip tunnel del $link
	}
}
