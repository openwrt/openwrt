# 6to4.sh - IPv6-in-IPv4 tunnel backend
# Copyright (c) 2010 OpenWrt.org

find_6to4_wanif() {
	local if=$(ip -4 r l e 0.0.0.0/0); if="${if#default* dev }"; if="${if%% *}"
	[ -n "$if" ] && grep -qs "^ *$if:" /proc/net/dev && echo "$if"
}

find_6to4_wanip() {
	local ip=$(ip -4 a s dev "$1"); ip="${ip#*inet }"
	echo "${ip%%[^0-9.]*}"
}

find_6to4_prefix() {
	local ip4="$1"
	local oIFS="$IFS"; IFS="."; set -- $ip4; IFS="$oIFS"

	printf "2002:%02x%02x:%02x%02x\n" $1 $2 $3 $4
}

# Hook into scan_interfaces() to synthesize a .device option
# This is needed for /sbin/ifup to properly dispatch control
# to setup_interface_6to4() even if no .ifname is set in
# the configuration.
scan_6to4() {
	config_set "$1" device "6to4-$1"
}

coldplug_interface_6to4() {
	setup_interface_6to4 "6to4-$1" "$1"
}

setup_interface_6to4() {
	local iface="$1"
	local cfg="$2"
	local link="6to4-$cfg"

	local local4=$(uci_get network "$cfg" ipaddr)

	local mtu
	config_get mtu "$cfg" mtu

	local ttl
	config_get ttl "$cfg" ttl

	local metric
	config_get metric "$cfg" metric

	local defaultroute
	config_get_bool defaultroute "$cfg" defaultroute 1

	local wanif=$(find_6to4_wanif)
	[ -z "$wanif" ] && {
		logger -t "$link" "Cannot find wan interface - aborting"
		return
	}

	local wancfg=$(find_config "$wanif")
	[ -z "$wancfg" ] && {
		logger -t "$link" "Cannot find wan network - aborting"
		return
	}

	# If local4 is unset, guess local IPv4 address from the
	# interface used by the default route.
	[ -z "$local4" ] && {
		[ -n "$wanif" ] && {
			local4=$(find_6to4_wanip "$wanif")
			uci_set_state network "$cfg" wan_device "$wanif"
		}
	}

	[ -n "$local4" ] && {
		logger -t "$link" "Starting ..."

		# creating the tunnel below will trigger a net subsystem event
		# prevent it from touching or iface by disabling .auto here
		uci_set_state network "$cfg" ifname $link
		uci_set_state network "$cfg" auto 0

		# find our local prefix
		local prefix6=$(find_6to4_prefix "$local4")
		local local6="$prefix6::1/16"

		logger -t "$link" " * IPv4 address is $local4"
		logger -t "$link" " * IPv6 address is $local6"
		ip tunnel add $link mode sit remote any local $local4 ttl ${ttl:-64}
		ip link set $link up
		ip link set mtu ${mtu:-1280} dev $link
		ip addr add $local6 dev $link

		uci_set_state network "$cfg" ipaddr $local4
		uci_set_state network "$cfg" ip6addr $local6

		[ "$defaultroute" = 1 ] && {
			logger -t "$link" " * Adding default route"
			ip -6 route add 2000::/3 via ::192.88.99.1 metric ${metric:-1} dev $link
			uci_set_state network "$cfg" defaultroute 1
		}

		# find delegation target
		local adv_interface
		config_get adv_interface "$cfg" adv_interface

		local adv_ifname
		config_get adv_ifname "${adv_interface:-lan}" ifname

		local adv_subnet=$(uci_get network "$cfg" adv_subnet)

		grep -qs "^ *$adv_ifname:" /proc/net/dev && {
			local subnet6="$prefix6:${adv_subnet:-1}::1/64"

			logger -t "$link" " * Advertising IPv6 subnet $subnet6 on ${adv_interface:-lan} ($adv_ifname)"
			ip -6 addr add $subnet6 dev $adv_ifname
			uci_set_state network "$cfg" adv_subnet "$subnet6"
			uci_set_state network "$cfg" adv_ifname "$adv_ifname"

			[ -f /etc/config/radvd ] && /etc/init.d/radvd enabled && {
				local sid="6to4_$cfg"

				uci -q batch <<-EOF
					revert radvd.iface_$sid
					revert radvd.prefix_$sid
					set radvd.iface_$sid=interface
					set radvd.iface_$sid.ignore=0
					set radvd.iface_$sid.interface=${adv_interface:-lan}
					set radvd.iface_$sid.IgnoreIfMissing=1
					set radvd.iface_$sid.AdvSendAdvert=1
					set radvd.iface_$sid.MaxRtrAdvInterval=30
					set radvd.prefix_$sid=prefix
					set radvd.prefix_$sid.ignore=0
					set radvd.prefix_$sid.interface=${adv_interface:-lan}
					set radvd.prefix_$sid.prefix=0:0:0:${adv_subnet:-1}::/64
					set radvd.prefix_$sid.AdvOnLink=1
					set radvd.prefix_$sid.AdvAutonomous=1
					set radvd.prefix_$sid.AdvValidLifetime=300
					set radvd.prefix_$sid.AdvPreferredLifetime=120
					set radvd.prefix_$sid.Base6to4Interface=$wancfg
				EOF

				/etc/init.d/radvd restart
			}
		}

		logger -t "$link" "... started"

		env -i ACTION="ifup" INTERFACE="$cfg" DEVICE="$link" PROTO=6to4 /sbin/hotplug-call "iface" &
	} || {
		echo "Cannot determine local IPv4 address for 6to4 tunnel $cfg - skipping"
	}
}

stop_interface_6to4() {
	local cfg="$1"
	local link="6to4-$cfg"

	local local6=$(uci_get_state network "$cfg" ip6addr)
	local defaultroute=$(uci_get_state network "$cfg" defaultroute)

	local adv_subnet=$(uci_get_state network "$cfg" adv_subnet)
	local adv_ifname=$(uci_get_state network "$cfg" adv_ifname)

	grep -qs "^ *$link:" /proc/net/dev && {
		logger -t "$link" "Shutting down ..."
		env -i ACTION="ifdown" INTERFACE="$cfg" DEVICE="$link" PROTO=6to4 /sbin/hotplug-call "iface" &

		[ -n "$adv_subnet" ] && [ -n "$adv_ifname" ] && {
			local sid="6to4_$cfg"

			uci -q batch <<-EOF
				revert radvd.iface_$sid
				revert radvd.prefix_$sid
			EOF

			/etc/init.d/radvd enabled && /etc/init.d/radvd restart

			ip -6 addr del $adv_subnet dev $adv_ifname
		}

		[ "$defaultroute" = "1" ] && {
			ip -6 route del 2000::/3 via ::192.88.99.1 dev $link metric 1
		}

		ip addr del $local6 dev $link
		ip link set $link down
		ip tunnel del $link

		logger -t "$link" "... stopped"
	}
}
