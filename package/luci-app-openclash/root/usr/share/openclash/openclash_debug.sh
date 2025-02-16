#!/bin/bash
. /lib/functions.sh
. /usr/share/openclash/ruby.sh

set_lock() {
   exec 885>"/tmp/lock/openclash_debug.lock" 2>/dev/null
   flock -x 885 2>/dev/null
}

del_lock() {
   flock -u 885 2>/dev/null
   rm -rf "/tmp/lock/openclash_debug.lock" 2>/dev/null
}

ipk_v()
{
   if [ -x "/bin/opkg" ]; then
      echo $(opkg status "$1" 2>/dev/null |grep 'Version' |awk -F ': ' '{print $2}' 2>/dev/null)
   elif [ -x "/usr/bin/apk" ]; then
      echo $(apk list "$1" 2>/dev/null |grep 'installed' | grep -oE '\d+(\.\d+)*' | head -1)
   fi
}

set_lock

DEBUG_LOG="/tmp/openclash_debug.log"
LOGTIME=$(echo $(date "+%Y-%m-%d %H:%M:%S"))
enable_custom_dns=$(uci -q get openclash.config.enable_custom_dns)
rule_source=$(uci -q get openclash.config.rule_source)
enable_custom_clash_rules=$(uci -q get openclash.config.enable_custom_clash_rules) 
ipv6_enable=$(uci -q get openclash.config.ipv6_enable)
ipv6_dns=$(uci -q get openclash.config.ipv6_dns)
enable_redirect_dns=$(uci -q get openclash.config.enable_redirect_dns)
disable_masq_cache=$(uci -q get openclash.config.disable_masq_cache)
proxy_mode=$(uci -q get openclash.config.proxy_mode)
intranet_allowed=$(uci -q get openclash.config.intranet_allowed)
enable_udp_proxy=$(uci -q get openclash.config.enable_udp_proxy)
enable_rule_proxy=$(uci -q get openclash.config.enable_rule_proxy)
en_mode=$(uci -q get openclash.config.en_mode)
RAW_CONFIG_FILE=$(uci -q get openclash.config.config_path)
CONFIG_FILE="/etc/openclash/$(uci -q get openclash.config.config_path |awk -F '/' '{print $5}' 2>/dev/null)"
core_model=$(uci -q get openclash.config.core_version)
if [ -x "/bin/opkg" ]; then
   cpu_model=$(opkg status libc 2>/dev/null |grep 'Architecture' |awk -F ': ' '{print $2}' 2>/dev/null)
elif [ -x "/usr/bin/apk" ]; then
   cpu_model=$(apk list libc 2>/dev/null|awk '{print $2}')
fi
core_meta_version=$(/etc/openclash/core/clash_meta -v 2>/dev/null |awk -F ' ' '{print $3}' |head -1 2>/dev/null)
servers_update=$(uci -q get openclash.config.servers_update)
mix_proxies=$(uci -q get openclash.config.mix_proxies)
op_version=$(ipk_v "luci-app-openclash")
china_ip_route=$(uci -q get openclash.config.china_ip_route)
common_ports=$(uci -q get openclash.config.common_ports)
router_self_proxy=$(uci -q get openclash.config.router_self_proxy)
core_type=$(uci -q get openclash.config.core_type || echo "Dev")
da_password=$(uci -q get openclash.config.dashboard_password)
cn_port=$(uci -q get openclash.config.cn_port)
lan_interface_name=$(uci -q get openclash.config.lan_interface_name || echo "0")
if [ "$lan_interface_name" = "0" ]; then
   lan_ip=$(uci -q get network.lan.ipaddr |awk -F '/' '{print $1}' 2>/dev/null || ip address show $(uci -q -p /tmp/state get network.lan.ifname || uci -q -p /tmp/state get network.lan.device) | grep -w "inet"  2>/dev/null |grep -Eo 'inet [0-9\.]+' | awk '{print $2}' |head -1 || ip addr show 2>/dev/null | grep -w 'inet' | grep 'global' | grep 'brd' | grep -Eo 'inet [0-9\.]+' | awk '{print $2}' | head -n 1)
else
   lan_ip=$(ip address show $lan_interface_name | grep -w "inet"  2>/dev/null |grep -Eo 'inet [0-9\.]+' | awk '{print $2}' |head -1)
fi
dnsmasq_default_resolvfile=$(uci -q get openclash.config.default_resolvfile)

if [ -z "$RAW_CONFIG_FILE" ] || [ ! -f "$RAW_CONFIG_FILE" ]; then
   for file_name in /etc/openclash/config/*
   do
      if [ -f "$file_name" ]; then
         RAW_CONFIG_FILE=$file_name
         CONFIG_NAME=$(echo "$RAW_CONFIG_FILE" |awk -F '/' '{print $5}' 2>/dev/null)
         CONFIG_FILE="/etc/openclash/$CONFIG_NAME"
         break
      fi
   done
fi

ts_cf()
{
	if [ "$1" = "0" ] || [ -z "$1" ]; then
	   echo "停用"
	else
	   echo "启用"
   fi
}

ts_re()
{
	if [ -z "$1" ]; then
	   echo "未安装"
	else
	   echo "已安装"
  fi
}

dns_re()
{
   if [ "$1" = "1" ]; then
	   echo "Dnsmasq 转发"
   elif [ "$1" = "2" ]; then
	   echo "Firewall 转发"
   else
      echo "停用"
   fi
}

echo "OpenClash 调试日志" > "$DEBUG_LOG"
cat >> "$DEBUG_LOG" <<-EOF

生成时间: $LOGTIME
插件版本: $op_version
隐私提示: 上传此日志前请注意检查、屏蔽公网IP、节点、密码等相关敏感信息

\`\`\`
EOF

cat >> "$DEBUG_LOG" <<-EOF

#===================== 系统信息 =====================#

主机型号: $(cat /tmp/sysinfo/model 2>/dev/null)
固件版本: $(cat /usr/lib/os-release 2>/dev/null |grep OPENWRT_RELEASE 2>/dev/null |awk -F '"' '{print $2}' 2>/dev/null)
LuCI版本: $(ipk_v "luci")
内核版本: $(uname -r 2>/dev/null)
处理器架构: $cpu_model

#此项有值时,如不使用IPv6,建议到网络-接口-lan的设置中禁用IPV6的DHCP
IPV6-DHCP: $(uci -q get dhcp.lan.dhcpv6)

DNS劫持: $(dns_re "$enable_redirect_dns")
#DNS劫持为Dnsmasq时，此项结果应仅有配置文件的DNS监听地址
Dnsmasq转发设置: $(uci -q get dhcp.@dnsmasq[0].server)
EOF

cat >> "$DEBUG_LOG" <<-EOF

#===================== 依赖检查 =====================#

dnsmasq-full: $(ts_re "$(ipk_v "dnsmasq-full")")
dnsmasq-full(ipset): $(ts_re "$(dnsmasq --version |grep -v no-ipset |grep ipset)")
dnsmasq-full(nftset): $(ts_re "$(dnsmasq --version |grep nftset)")
bash: $(ts_re "$(ipk_v "bash")")
curl: $(ts_re "$(ipk_v "curl")")
ca-bundle: $(ts_re "$(ipk_v "ca-bundle")")
ipset: $(ts_re "$(ipk_v "ipset")")
ip-full: $(ts_re "$(ipk_v "ip-full")")
ruby: $(ts_re "$(ipk_v "ruby")")
ruby-yaml: $(ts_re "$(ipk_v "ruby-yaml")")
ruby-psych: $(ts_re "$(ipk_v "ruby-psych")")
ruby-pstore: $(ts_re "$(ipk_v "ruby-pstore")")
kmod-tun(TUN模式): $(ts_re "$(ipk_v "kmod-tun")")
luci-compat(Luci >= 19.07): $(ts_re "$(ipk_v "luci-compat")")
kmod-inet-diag(PROCESS-NAME): $(ts_re "$(ipk_v "kmod-inet-diag")")
unzip: $(ts_re "$(ipk_v "unzip")")
EOF
if [ -n "$(command -v fw4)" ]; then
cat >> "$DEBUG_LOG" <<-EOF
kmod-nft-tproxy: $(ts_re "$(ipk_v kmod-nft-tproxy)")
EOF
else
cat >> "$DEBUG_LOG" <<-EOF
iptables-mod-tproxy: $(ts_re "$(ipk_v "iptables-mod-tproxy")")
kmod-ipt-tproxy: $(ts_re "$(ipk_v "kmod-ipt-tproxy")")
iptables-mod-extra: $(ts_re "$(ipk_v "iptables-mod-extra")")
kmod-ipt-extra: $(ts_re "$(ipk_v "kmod-ipt-extra")")
kmod-ipt-nat: $(ts_re "$(ipk_v "kmod-ipt-nat")")
EOF
fi

#core
cat >> "$DEBUG_LOG" <<-EOF

#===================== 内核检查 =====================#

EOF
if pidof clash >/dev/null; then
cat >> "$DEBUG_LOG" <<-EOF
运行状态: 运行中
运行内核：$core_type
进程pid: $(pidof clash)
运行用户: $(ps |grep "/etc/openclash/clash" |grep -v grep |awk '{print $2}' 2>/dev/null)
EOF
else
cat >> "$DEBUG_LOG" <<-EOF
运行状态: 未运行
EOF
fi
if [ "$core_model" = "0" ]; then
   core_model="未选择架构"
fi
cat >> "$DEBUG_LOG" <<-EOF
已选择的架构: $core_model

#下方无法显示内核版本号时请确认您的内核版本是否正确或者有无权限
EOF

cat >> "$DEBUG_LOG" <<-EOF

Meta内核版本: $core_meta_version
EOF

if [ ! -f "/etc/openclash/core/clash_meta" ]; then
cat >> "$DEBUG_LOG" <<-EOF
Meta内核文件: 不存在
EOF
else
cat >> "$DEBUG_LOG" <<-EOF
Meta内核文件: 存在
EOF
fi
if [ ! -x "/etc/openclash/core/clash_meta" ]; then
cat >> "$DEBUG_LOG" <<-EOF
Meta内核运行权限: 否
EOF
else
cat >> "$DEBUG_LOG" <<-EOF
Meta内核运行权限: 正常
EOF
fi

cat >> "$DEBUG_LOG" <<-EOF

#===================== 插件设置 =====================#

当前配置文件: $RAW_CONFIG_FILE
启动配置文件: $CONFIG_FILE
运行模式: $en_mode
默认代理模式: $proxy_mode
UDP流量转发(tproxy): $(ts_cf "$enable_udp_proxy")
自定义DNS: $(ts_cf "$enable_custom_dns")
IPV6代理: $(ts_cf "$ipv6_enable")
IPV6-DNS解析: $(ts_cf "$ipv6_dns")
禁用Dnsmasq缓存: $(ts_cf "$disable_masq_cache")
自定义规则: $(ts_cf "$enable_custom_clash_rules")
仅允许内网: $(ts_cf "$intranet_allowed")
仅代理命中规则流量: $(ts_cf "$enable_rule_proxy")
仅允许常用端口流量: $(ts_cf "$common_ports")
绕过中国大陆IP: $(ts_cf "$china_ip_route")
路由本机代理: $(ts_cf "$router_self_proxy")

#启动异常时建议关闭此项后重试
混合节点: $(ts_cf "$mix_proxies")
保留配置: $(ts_cf "$servers_update")
EOF

cat >> "$DEBUG_LOG" <<-EOF

#启动异常时建议关闭此项后重试
第三方规则: $(ts_cf "$rule_source")
EOF


if [ "$enable_custom_clash_rules" -eq 1 ]; then
cat >> "$DEBUG_LOG" <<-EOF

#===================== 自定义规则 一 =====================#
EOF
cat /etc/openclash/custom/openclash_custom_rules.list >> "$DEBUG_LOG"

cat >> "$DEBUG_LOG" <<-EOF

#===================== 自定义规则 二 =====================#
EOF
cat /etc/openclash/custom/openclash_custom_rules_2.list >> "$DEBUG_LOG"
fi

cat >> "$DEBUG_LOG" <<-EOF

#===================== 配置文件 =====================#

EOF
if [ -f "$CONFIG_FILE" ]; then
   ruby_read "$CONFIG_FILE" ".select {|x| 'proxies' != x and 'proxy-providers' != x }.to_yaml" 2>/dev/null >> "$DEBUG_LOG"
else
   ruby_read "$RAW_CONFIG_FILE" ".select {|x| 'proxies' != x and 'proxy-providers' != x }.to_yaml" 2>/dev/null >> "$DEBUG_LOG"
fi

sed -i '/^ \{0,\}secret:/d' "$DEBUG_LOG" 2>/dev/null

#custom overwrite
cat >> "$DEBUG_LOG" <<-EOF

#===================== 自定义覆写设置 =====================#

EOF

cat /etc/openclash/custom/openclash_custom_overwrite.sh >> "$DEBUG_LOG" 2>/dev/null

#firewall
cat >> "$DEBUG_LOG" <<-EOF

#===================== 自定义防火墙设置 =====================#

EOF

cat /etc/openclash/custom/openclash_custom_firewall_rules.sh >> "$DEBUG_LOG" 2>/dev/null

cat >> "$DEBUG_LOG" <<-EOF

#===================== IPTABLES 防火墙设置 =====================#

#IPv4 NAT chain

EOF
iptables-save -t nat >> "$DEBUG_LOG" 2>/dev/null

cat >> "$DEBUG_LOG" <<-EOF

#IPv4 Mangle chain

EOF
iptables-save -t mangle >> "$DEBUG_LOG" 2>/dev/null

cat >> "$DEBUG_LOG" <<-EOF

#IPv4 Filter chain

EOF
iptables-save -t filter >> "$DEBUG_LOG" 2>/dev/null

cat >> "$DEBUG_LOG" <<-EOF

#IPv6 NAT chain

EOF
ip6tables-save -t nat >> "$DEBUG_LOG" 2>/dev/null

cat >> "$DEBUG_LOG" <<-EOF

#IPv6 Mangle chain

EOF
ip6tables-save -t mangle >> "$DEBUG_LOG" 2>/dev/null

cat >> "$DEBUG_LOG" <<-EOF

#IPv6 Filter chain

EOF
ip6tables-save -t filter >> "$DEBUG_LOG" 2>/dev/null

if [ -n "$(command -v fw4)" ]; then
cat >> "$DEBUG_LOG" <<-EOF

#===================== NFTABLES 防火墙设置 =====================#

EOF
   for nft in "input" "forward" "dstnat" "srcnat" "nat_output" "mangle_prerouting" "mangle_output"; do
      nft list chain inet fw4 "$nft" >> "$DEBUG_LOG" 2>/dev/null
   done >/dev/null 2>&1
   for nft in "openclash" "openclash_mangle" "openclash_mangle_output" "openclash_output" "openclash_post" "openclash_wan_input" "openclash_dns_hijack" "openclash_dns_redirect" "openclash_v6" "openclash_mangle_v6" "openclash_mangle_output_v6" "openclash_output_v6" "openclash_post_v6" "openclash_wan6_input"; do
      nft list chain inet fw4 "$nft" >> "$DEBUG_LOG" 2>/dev/null
   done >/dev/null 2>&1
fi

cat >> "$DEBUG_LOG" <<-EOF

#===================== IPSET状态 =====================#

EOF
ipset list -t >> "$DEBUG_LOG"

cat >> "$DEBUG_LOG" <<-EOF

#===================== 路由表状态 =====================#

EOF
echo "#IPv4" >> "$DEBUG_LOG"
echo "" >> "$DEBUG_LOG"
echo "#route -n" >> "$DEBUG_LOG"
route -n >> "$DEBUG_LOG" 2>/dev/null
echo "" >> "$DEBUG_LOG"
echo "#ip route list" >> "$DEBUG_LOG"
ip route list >> "$DEBUG_LOG" 2>/dev/null
echo "" >> "$DEBUG_LOG"
echo "#ip rule show" >> "$DEBUG_LOG"
ip rule show >> "$DEBUG_LOG" 2>/dev/null
echo "" >> "$DEBUG_LOG"
echo "#IPv6" >> "$DEBUG_LOG"
echo "" >> "$DEBUG_LOG"
echo "#route -A inet6" >> "$DEBUG_LOG"
route -A inet6 >> "$DEBUG_LOG" 2>/dev/null
echo "" >> "$DEBUG_LOG"
echo "#ip -6 route list" >> "$DEBUG_LOG"
ip -6 route list >> "$DEBUG_LOG" 2>/dev/null
echo "" >> "$DEBUG_LOG"
echo "#ip -6 rule show" >> "$DEBUG_LOG"
ip -6 rule show >> "$DEBUG_LOG" 2>/dev/null

if [ "$en_mode" != "fake-ip" ] && [ "$en_mode" != "redir-host" ]; then
cat >> "$DEBUG_LOG" <<-EOF

#===================== Tun设备状态 =====================#

EOF
ip tuntap list >> "$DEBUG_LOG" 2>/dev/null
fi

cat >> "$DEBUG_LOG" <<-EOF

#===================== 端口占用状态 =====================#

EOF
netstat -nlp |grep clash >> "$DEBUG_LOG" 2>/dev/null

cat >> "$DEBUG_LOG" <<-EOF

#===================== 测试本机DNS查询(www.baidu.com) =====================#

EOF
nslookup www.baidu.com >> "$DEBUG_LOG" 2>/dev/null

cat >> "$DEBUG_LOG" <<-EOF

#===================== 测试内核DNS查询(www.instagram.com) =====================#

EOF
/usr/share/openclash/openclash_debug_dns.lua "www.instagram.com" >> "$DEBUG_LOG" 2>/dev/null

cat >> "$DEBUG_LOG" <<-EOF

Dnsmasq 当前默认 resolv 文件：$dnsmasq_default_resolvfile
EOF

if [ -s "/tmp/resolv.conf.auto" ]; then
cat >> "$DEBUG_LOG" <<-EOF

#===================== /tmp/resolv.conf.auto =====================#

EOF
cat /tmp/resolv.conf.auto >> "$DEBUG_LOG"
fi

if [ -s "/tmp/resolv.conf.d/resolv.conf.auto" ]; then
cat >> "$DEBUG_LOG" <<-EOF

#===================== /tmp/resolv.conf.d/resolv.conf.auto =====================#

EOF
cat /tmp/resolv.conf.d/resolv.conf.auto >> "$DEBUG_LOG"
fi

cat >> "$DEBUG_LOG" <<-EOF

#===================== 测试本机网络连接(www.baidu.com) =====================#

EOF
curl -SsI -m 5 www.baidu.com >> "$DEBUG_LOG" 2>/dev/null

cat >> "$DEBUG_LOG" <<-EOF

#===================== 测试本机网络下载(raw.githubusercontent.com) =====================#

EOF
VERSION_URL="https://raw.githubusercontent.com/vernesong/OpenClash/refs/heads/master/LICENSE"
if pidof clash >/dev/null; then
   curl -SsIL -m 3 --retry 2 "$VERSION_URL" >> "$DEBUG_LOG" 2>/dev/null
else
   curl -SsIL -m 3 --retry 2 "$VERSION_URL" >> "$DEBUG_LOG" 2>/dev/null
fi

cat >> "$DEBUG_LOG" <<-EOF

#===================== 最近运行日志(自动切换为Debug模式) =====================#

EOF

if pidof clash >/dev/null; then
   curl -SsL -m 3 -H "Content-Type: application/json" -H "Authorization: Bearer ${da_password}" -XPATCH http://${lan_ip}:${cn_port}/configs -d '{"log-level": "debug"}' >/dev/null
   sleep 10
fi

tail -n 100 "/tmp/openclash.log" >> "$DEBUG_LOG" 2>/dev/null
cat >> "$DEBUG_LOG" <<-EOF

#===================== 最近运行日志获取完成(自动切换为silent模式) =====================#

EOF
if pidof clash >/dev/null; then
   curl -SsL -m 3 -H "Content-Type: application/json" -H "Authorization: Bearer ${da_password}" -XPATCH http://${lan_ip}:${cn_port}/configs -d '{"log-level": "silent"}' >/dev/null
fi

cat >> "$DEBUG_LOG" <<-EOF

#===================== 活动连接信息 =====================#

EOF
/usr/share/openclash/openclash_debug_getcon.lua

cat >> "$DEBUG_LOG" <<-EOF

\`\`\`
EOF

sed -i -E 's/(([0-9]{1,3}\.){2})[0-9]{1,3}\.[0-9]{1,3}/\1*\.*/g' "$DEBUG_LOG" 2>/dev/null

sed -i -E 's/(:[0-9a-fA-F]{1,4}){3}/:*:*:*/' "$DEBUG_LOG" 2>/dev/null

sed -i 's/Downloading URL【[^】]*】/Downloading URL【*】/g' "$DEBUG_LOG" 2>/dev/null

del_lock