#!/bin/sh

# 从 /etc/config/bluewan-auto 读取配置
ENABLE=$(uci -q get bluewan-auto.settings.enable)
[ "$ENABLE" != "1" ] && exit 0

WAN_IF="BLUE4WAN"
WAN6_IF="BLUE4WAN6"
DEVICE=$(uci -q get bluewan-auto.settings.iface || echo "lan4")
USERNAME=$(uci -q get bluewan-auto.settings.username)
PASSWORD=$(uci -q get bluewan-auto.settings.password)

# ========= 配置 IPv4 PPPoE =========
uci -q delete network.$WAN_IF
uci set network.$WAN_IF="interface"
uci set network.$WAN_IF.proto="pppoe"
uci set network.$WAN_IF.device="$DEVICE"
[ -n "$USERNAME" ] && uci set network.$WAN_IF.username="$USERNAME"
[ -n "$PASSWORD" ] && uci set network.$WAN_IF.password="$PASSWORD"
uci set network.$WAN_IF.ipv6="auto"

# ========= 配置 IPv6 DHCPv6 =========
uci -q delete network.$WAN6_IF
uci set network.$WAN6_IF="interface"
uci set network.$WAN6_IF.proto="dhcpv6"
uci set network.$WAN6_IF.device="$DEVICE"

# ========= 自动加入 WAN 防火墙区 =========
WAN_IDX=$(uci show firewall | grep "=zone" | grep "'wan'" | cut -d'.' -f2 | head -n1)
if [ -n "$WAN_IDX" ]; then
    for iface in $WAN_IF $WAN6_IF; do
        if ! uci -q get firewall.$WAN_IDX.network | grep -q "$iface"; then
            uci add_list firewall.$WAN_IDX.network="$iface"
            echo "[bluewan-setup] 已将 $iface 加入 wan 防火墙区"
        fi
    done
fi

# ========= 应用配置 =========
uci commit network
uci commit firewall

/etc/init.d/network reload
/etc/init.d/firewall reload

echo "[bluewan-setup] 配置完成"
exit 0

