#!/bin/sh
IFNAME="BLUE4WAN"
IFNAMEV6="BLUE4WANV6"
filep="/usr/bin/blue4wan.conf"

if [ -f "$filep" ]; then
    echo "back"
    exit 0
fi

# 检查是否存在名为BLUE4的网络接口
if ! ip link show | grep -q "BLUE4"; then
    echo "Network interface BLUE4 not found, exiting."
    exit 1
fi

setup_blue4_interface() {
    # 配置 BLUE4WAN IPv4 接口
    uci delete network.$IFNAME
    uci set network.$IFNAME="interface"
    uci set network.$IFNAME.ifname="BLUE4"
    uci set network.$IFNAME.proto='dhcp'
    uci add_list firewall.@zone[1].network=$IFNAME

    # 配置 BLUE4WANV6 IPv6 接口
    uci delete network.$IFNAMEV6
    uci set network.$IFNAMEV6="interface"
    uci set network.$IFNAMEV6.ifname="BLUE4"
    uci set network.$IFNAMEV6.proto='dhcpv6'
    uci add_list firewall.@zone[1].network=$IFNAMEV6

    # 提交更改
    uci commit network
    uci commit firewall

    # 创建标志文件，防止重复执行
    touch "$filep"
    echo "over"
}

setup_blue4_interface
