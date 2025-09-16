#!/bin/bash
# 一键修改 DTS 并生成完整 network 配置（保留 ula_prefix 和 packet_steering）
# 并支持交互输入 PPPoE 宽带账号密码
# 适用于 mt7981b-nradio-c8-668gl

DTS="target/linux/mediatek/dts/mt7981b-nradio-c8-668gl.dts"
NETWORK="package/base-files/files/etc/config/network"

# 交互输入宽带账号密码
read -p "请输入 WAN1 (PPPoE) 宽带账号: " PPPoE_USER
read -s -p "请输入 WAN1 (PPPoE) 密码: " PPPoE_PASS
echo ""

echo "[1/3] 修改 DTS: lan4 -> wan1 ..."
if grep -q 'label = "lan4"' "$DTS"; then
    sed -i 's/label = "lan4"/label = "wan1"/' "$DTS"
    echo "✅ DTS 修改完成"
else
    echo "⚠️ 未找到 lan4，可能已经修改过"
fi

echo "[2/3] 生成完整 network 配置 ..."
mkdir -p "$(dirname "$NETWORK")"

cat > "$NETWORK" <<EOF
config interface 'loopback'
	option device 'lo'
	option proto 'static'
	option ipaddr '127.0.0.1'
	option netmask '255.0.0.0'

config globals 'globals'
	option ula_prefix 'fd5a:75a8:22c2::/48'
	option packet_steering '1'

# LAN 网口桥接
config device
	option name 'br-lan'
	option type 'bridge'
	list ports 'lan1'
	list ports 'lan2'
	list ports 'lan3'

config interface 'lan'
	option device 'br-lan'
	option proto 'static'
	option ipaddr '192.168.66.1'
	option netmask '255.255.255.0'
	option ip6assign '64'

# WAN 口 (DHCP)
config device
	option name 'eth1'
	option macaddr ':'

config interface 'wan'
	option device 'eth1'
	option proto 'dhcp'

config interface 'wan6'
	option device 'eth1'
	option proto 'dhcpv6'

# WAN1 / PPPoE 拨号口
config interface 'wan1'
	option proto 'pppoe'
	option device 'wan1'
	option username '$PPPoE_USER'
	option password '$PPPoE_PASS'
	option ipv6 'auto'
	option ip6assign '64'
EOF

echo "✅ 完整 network 配置生成完成，已写入账号和密码"

echo "[3/3] 完成！"
echo "📌 下一步：重新编译固件：make -j\$(nproc)"
echo "开机后将有 LAN + WAN + WAN1（PPPoE 拨号）"
