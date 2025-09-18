#!/bin/sh

ENABLE=$(uci get bluewan-auto.settings.enable 2>/dev/null)
[ "$ENABLE" != "1" ] && exit 0

WAN_IF="BLUE4WAN"
WAN6_IF="BLUE4WAN6"
DEVICE=$(uci get bluewan-auto.settings.iface 2>/dev/null || echo "lan4")
USERNAME=$(uci get bluewan-auto.settings.username 2>/dev/null)
PASSWORD=$(uci get bluewan-auto.settings.password 2>/dev/null)

# 配置 IPv4 PPPoE
uci -q delete network.$WAN_IF
uci set network.$WAN_IF="interface"
uci set network.$WAN_IF.proto="pppoe"
uci set network.$WAN_IF.device="$DEVICE"
uci set network.$WAN_IF.username="$USERNAME"
uci set network.$WAN_IF.password="$PASSWORD"
uci set network.$WAN_IF.ipv6="auto"

# 配置 IPv6 DHCPv6
uci -q delete network.$WAN6_IF
uci set network.$WAN6_IF="interface"
uci set network.$WAN6_IF.proto="dhcpv6"
uci set network.$WAN6_IF.device="$DEVICE"

# 自动加入 WAN 防火墙区
WAN_IDX=$(uci show firewall | grep "=zone" | grep "'wan'" | cut -d'.' -f2 | head -n1)
if [ -n "$WAN_IDX" ]; then
    uci add_list firewall.$WAN_IDX.network="$WAN_IF"
    uci add_list firewall.$WAN_IDX.network="$WAN6_IF"
fi

uci commit network
uci commit firewall

/etc/init.d/network reload
/etc/init.d/firewall reload
