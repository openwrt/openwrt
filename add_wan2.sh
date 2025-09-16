#!/bin/bash
# 一键把 lan4 改成 wan2，并增加默认 wan2 配置
# 适用于 mt7981b-nradio-c8-668gl.dts

DTS="target/linux/mediatek/dts/mt7981b-nradio-c8-668gl.dts"
NETWORK="package/base-files/files/etc/config/network"

echo "[1/2] 修改 DTS (${DTS}) ..."
if grep -q 'label = "lan4"' "$DTS"; then
    sed -i 's/label = "lan4"/label = "wan2"/' "$DTS"
    echo "✅ 已把 lan4 改成 wan2"
else
    echo "⚠️ 未找到 lan4，可能已经修改过"
fi

echo "[2/2] 生成默认网络配置 (${NETWORK}) ..."
mkdir -p "$(dirname "$NETWORK")"

cat > "$NETWORK" <<'EOF'
config interface 'loopback'
	option device 'lo'
	option proto 'static'
	option ipaddr '127.0.0.1'
	option netmask '255.0.0.0'

config globals 'globals'
	option ula_prefix 'fdxx:xxxx:xxxx::/48'

config device
	option name 'br-lan'
	option type 'bridge'
	list ports 'lan1'
	list ports 'lan2'
	list ports 'lan3'

config interface 'lan'
	option device 'br-lan'
	option proto 'static'
	option ipaddr '192.168.1.1'
	option netmask '255.255.255.0'
	option ip6assign '60'

config interface 'wan'
	option device 'wan'
	option proto 'dhcp'

config interface 'wan6'
	option device 'wan'
	option proto 'dhcpv6'

config interface 'wan2'
	option device 'wan2'
	option proto 'dhcp'

config interface 'wan2_6'
	option device 'wan2'
	option proto 'dhcpv6'
EOF

echo "✅ 默认网络配置已写入"

echo ""
echo "🎉 完成！现在可以重新编译固件，开机后会有 WAN + WAN2。"
