#!/bin/sh
# bluewan-ipv6-setup.sh
# 自动配置 PPPoE (BLUE4WAN) IPv6

PPPOE_IF="BLUE4WAN"
LAN_IF="lan"

echo "[bluewan-ipv6] 检查 PPPoE 接口: $PPPOE_IF"

# 确保 PPPoE 接口存在
if ! uci show network.$PPPOE_IF >/dev/null 2>&1; then
    echo "[bluewan-ipv6] 接口 $PPPOE_IF 不存在，退出"
    exit 1
fi

# 1. 开启 IPv6 支持
uci set network.$PPPOE_IF.ipv6='1'

# 2. 删除多余的 DHCPv6 客户端接口 (BLUE4WAN6)
if uci show network | grep -q "network.BLUE4WAN6"; then
    uci delete network.BLUE4WAN6
    echo "[bluewan-ipv6] 已删除多余接口 BLUE4WAN6"
fi

# 3. 确保 LAN 支持 IPv6 前缀分配
uci set network.$LAN_IF.ipv6='1'
uci set network.$LAN_IF.delegate='1'

# 4. 提交并重启网络
uci commit network
/etc/init.d/network restart

echo "[bluewan-ipv6] 已完成 PPPoE IPv6 配置"
exit 0
