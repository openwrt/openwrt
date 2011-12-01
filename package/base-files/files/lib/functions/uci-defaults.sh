#!/bin/sh
# Copyright (C) 2011 OpenWrt.org

ucidef_set_interface_loopback() {
	uci batch <<EOF
set network.loopback='interface'
set network.loopback.ifname='lo'
set network.loopback.proto='static'
set network.loopback.ipaddr='127.0.0.1'
set network.loopback.netmask='255.0.0.0'
EOF
}

ucidef_set_interface_raw() {
	local cfg=$1
	local ifname=$2

	uci batch <<EOF
set network.$cfg='interface'
set network.$cfg.ifname='$ifname'
set network.$cfg.proto='none'
EOF
}

ucidef_set_interface_lan() {
	local ifname=$1

	uci batch <<EOF
set network.lan='interface'
set network.lan.ifname='$ifname'
set network.lan.type='bridge'
set network.lan.proto='static'
set network.lan.ipaddr='192.168.1.1'
set network.lan.netmask='255.255.255.0'
EOF
}

ucidef_set_interface_wan() {
	local ifname=$1

	uci batch <<EOF
set network.wan='interface'
set network.wan.ifname='$ifname'
set network.wan.proto='dhcp'
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

