#!/bin/sh 

log() {
	logger -t "Firewall Settings : " "$@"
}

sleep 5
LANOPENDNS=$(uci get openvpn.settings.lanopendns)
if [ -z $LANOPENDNS ]; then
	LANOPENDNS="0"
fi
LANGOOGLE=$(uci get openvpn.settings.langoogle)
if [ -z $LANGOOGLE ]; then
	LANGOOGLE="0"
fi
WANOPENDNS=$(uci get openvpn.settings.wanopendns)
if [ -z $WANOPENDNS ]; then
	WANOPENDNS="0"
fi
WANGOOGLE=$(uci get openvpn.settings.wangoogle)
if [ -z $WANGOOGLE ]; then
	WANGOOGLE="0"
fi

if [ $LANOPENDNS = "1" -a $LANGOOGLE = "0" ]; then
	uci del dhcp.lan.dhcp_option
	uci add_list dhcp.lan.dhcp_option='6,208.67.222.222,208.67.220.220'
fi
if [ $LANOPENDNS = "0" -a $LANGOOGLE = "1" ]; then
	uci del dhcp.lan.dhcp_option
	uci add_list dhcp.lan.dhcp_option='6,8.8.8.8,8.8.4.4'
fi
if [ $LANOPENDNS = "1" -a $LANGOOGLE = "1" ]; then
	uci del dhcp.lan.dhcp_option
	uci add_list dhcp.lan.dhcp_option='6,8.8.8.8,8.8.4.4'
	uci add_list dhcp.lan.dhcp_option='6,208.67.222.222,208.67.220.220'
fi
if [ $LANOPENDNS = "0" -a $LANGOOGLE = "0" ]; then
	uci del dhcp.lan.dhcp_option
fi

if [ $WANOPENDNS = "1" -a $WANGOOGLE = "0" ]; then
	uci set network.wan.peerdns='0'
	uci del network.wan.dns
	uci add_list network.wan.dns='208.67.222.222'
	uci add_list network.wan.dns='208.67.220.220'
fi
if [ $WANOPENDNS = "0" -a $WANGOOGLE = "1" ]; then
	uci set network.wan.peerdns='0'
	uci del network.wan.dns
	uci add_list network.wan.dns='8.8.8.8'
	uci add_list network.wan.dns='8.8.4.4'
fi
if [ $WANOPENDNS = "1" -a $WANGOOGLE = "1" ]; then
	uci set network.wan.peerdns='0'
	uci del network.wan.dns
	uci add_list network.wan.dns='8.8.8.8'
	uci add_list network.wan.dns='8.8.4.4'
	uci add_list network.wan.dns='208.67.222.222'
	uci add_list network.wan.dns='208.67.220.220'
fi
if [ $WANOPENDNS = "0" -a $WANGOOGLE = "0" ]; then
	uci set network.wan.peerdns='1'
	uci del network.wan.dns
fi
uci commit
/etc/init.d/network restart