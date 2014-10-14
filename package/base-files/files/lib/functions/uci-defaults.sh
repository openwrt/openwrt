#!/bin/sh
# Copyright (C) 2011 OpenWrt.org

UCIDEF_LEDS_CHANGED=0

ucidef_set_led_netdev() {
	local cfg="led_$1"
	local name=$2
	local sysfs=$3
	local dev=$4

	uci -q get system.$cfg && return 0

	uci batch <<EOF
set system.$cfg='led'
set system.$cfg.name='$name'
set system.$cfg.sysfs='$sysfs'
set system.$cfg.trigger='netdev'
set system.$cfg.dev='$dev'
set system.$cfg.mode='link tx rx'
EOF
	UCIDEF_LEDS_CHANGED=1
}

ucidef_set_led_usbdev() {
	local cfg="led_$1"
	local name=$2
	local sysfs=$3
	local dev=$4

	uci -q get system.$cfg && return 0

	uci batch <<EOF
set system.$cfg='led'
set system.$cfg.name='$name'
set system.$cfg.sysfs='$sysfs'
set system.$cfg.trigger='usbdev'
set system.$cfg.dev='$dev'
set system.$cfg.interval='50'
EOF
	UCIDEF_LEDS_CHANGED=1
}

ucidef_set_led_wlan() {
	local cfg="led_$1"
	local name=$2
	local sysfs=$3
	local trigger=$4

	uci -q get system.$cfg && return 0

	uci batch <<EOF
set system.$cfg='led'
set system.$cfg.name='$name'
set system.$cfg.sysfs='$sysfs'
set system.$cfg.trigger='$trigger'
EOF
	UCIDEF_LEDS_CHANGED=1
}

ucidef_set_led_switch() {
	local cfg="led_$1"
	local name=$2
	local sysfs=$3
	local trigger=$4
	local port_mask=$5

	uci -q get system.$cfg && return 0

	uci batch <<EOF
set system.$cfg='led'
set system.$cfg.name='$name'
set system.$cfg.sysfs='$sysfs'
set system.$cfg.trigger='$trigger'
set system.$cfg.port_mask='$port_mask'
EOF
	UCIDEF_LEDS_CHANGED=1
}

ucidef_set_led_default() {
	local cfg="led_$1"
	local name=$2
	local sysfs=$3
	local default=$4

	uci -q get system.$cfg && return 0

	uci batch <<EOF
set system.$cfg='led'
set system.$cfg.name='$name'
set system.$cfg.sysfs='$sysfs'
set system.$cfg.default='$default'
EOF
	UCIDEF_LEDS_CHANGED=1
}

ucidef_set_led_rssi() {
	local cfg="led_$1"
	local name=$2
	local sysfs=$3
	local iface=$4
	local minq=$5
	local maxq=$6
	local offset=$7
	local factor=$8

	uci -q get system.$cfg && return 0

	uci batch <<EOF
set system.$cfg='led'
set system.$cfg.name='$name'
set system.$cfg.sysfs='$sysfs'
set system.$cfg.trigger='rssi'
set system.$cfg.iface='rssid_$iface'
set system.$cfg.minq='$minq'
set system.$cfg.maxq='$maxq'
set system.$cfg.offset='$offset'
set system.$cfg.factor='$factor'
EOF
	UCIDEF_LEDS_CHANGED=1
}

ucidef_set_rssimon() {
	local dev="$1"
	local refresh="$2"
	local threshold="$3"

	local cfg="rssid_$dev"

	uci -q get system.$cfg && return 0

	uci batch <<EOF
set system.$cfg='rssid'
set system.$cfg.dev='$dev'
set system.$cfg.refresh='$refresh'
set system.$cfg.threshold='$threshold'
EOF
	UCIDEF_LEDS_CHANGED=1
}

ucidef_commit_leds()
{
	[ "$UCIDEF_LEDS_CHANGED" = "1" ] && uci commit system
}

ucidef_set_interface_loopback() {
	uci batch <<EOF
set network.loopback='interface'
set network.loopback.ifname='lo'
set network.loopback.proto='static'
set network.loopback.ipaddr='127.0.0.1'
set network.loopback.netmask='255.0.0.0'
set network.globals='globals'
set network.globals.ula_prefix='auto'
EOF
}

ucidef_set_interface_raw() {
	local cfg=$1
	local ifname=$2
	local proto=${3:-"none"}

	uci batch <<EOF
set network.$cfg='interface'
set network.$cfg.ifname='$ifname'
set network.$cfg.proto='$proto'
EOF
}

ucidef_set_interface_lan() {
	local ifname=$1

	uci batch <<EOF
set network.lan='interface'
set network.lan.ifname='$ifname'
set network.lan.force_link=1
set network.lan.type='bridge'
set network.lan.proto='static'
set network.lan.ipaddr='192.168.1.1'
set network.lan.netmask='255.255.255.0'
set network.lan.ip6assign='60'
EOF
}

ucidef_set_interface_wan() {
	local ifname=$1

	uci batch <<EOF
set network.wan='interface'
set network.wan.ifname='$ifname'
set network.wan.proto='dhcp'
set network.wan6='interface'
set network.wan6.ifname='$ifname'
set network.wan6.proto='dhcpv6'
EOF
}

ucidef_set_interfaces_lan_wan() {
	local lan_ifname=$1
	local wan_ifname=$2

	ucidef_set_interface_lan "$lan_ifname"
	ucidef_set_interface_wan "$wan_ifname"
}

ucidef_set_interface_macaddr() {
	local ifname=$1
	local mac=$2

	uci batch <<EOF
set network.$ifname.macaddr='$mac'
EOF
}

ucidef_add_switch() {
	local name=$1
	local reset=$2
	local enable=$3
	uci batch <<EOF
add network switch
set network.@switch[-1].name='$name'
set network.@switch[-1].reset='$reset'
set network.@switch[-1].enable_vlan='$enable'
EOF
}

ucidef_add_switch_vlan() {
	local device=$1
	local vlan=$2
	local ports=$3
	uci batch <<EOF
add network switch_vlan
set network.@switch_vlan[-1].device='$device'
set network.@switch_vlan[-1].vlan='$vlan'
set network.@switch_vlan[-1].ports='$ports'
EOF
}

ucidef_add_switch_port() {
	local device=$1
	local port=$2
	uci batch <<EOF
add network switch_port
set network.@switch_port[-1].device='$device'
set network.@switch_port[-1].port='$port'
EOF
}

