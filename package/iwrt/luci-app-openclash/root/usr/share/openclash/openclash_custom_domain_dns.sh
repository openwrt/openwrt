#!/bin/sh
. /usr/share/openclash/log.sh

set_lock() {
   exec 883>"/tmp/lock/openclash_cus_domian.lock" 2>/dev/null
   flock -x 883 2>/dev/null
}

del_lock() {
   flock -u 883 2>/dev/null
   rm -rf "/tmp/lock/openclash_cus_domian.lock"
}

set_lock

# 获取默认的 DNSMASQ 配置 ID
DEFAULT_DNSMASQ_CFGID="$(uci -q show "dhcp.@dnsmasq[0]" | awk 'NR==1 {split($0, conf, /[.=]/); print conf[2]}')"
# 从 conf-dir 行中提取配置目录路径
if [ -f "/tmp/etc/dnsmasq.conf.$DEFAULT_DNSMASQ_CFGID" ]; then
   DNSMASQ_CONF_DIR="$(awk -F '=' '/^conf-dir=/ {print $2}' "/tmp/etc/dnsmasq.conf.$DEFAULT_DNSMASQ_CFGID")"
else
   DNSMASQ_CONF_DIR="/tmp/dnsmasq.d"
fi
# 设置 DNSMASQ_CONF_DIR，并去除路径末尾的斜杠
DNSMASQ_CONF_DIR=${DNSMASQ_CONF_DIR%*/}
rm -rf ${DNSMASQ_CONF_DIR}/dnsmasq_openclash_custom_domain.conf >/dev/null 2>&1
if [ "$(uci get openclash.config.enable_custom_domain_dns_server 2>/dev/null)" = "1" ] && [ "$(uci get openclash.config.enable_redirect_dns 2>/dev/null)" = "1" ]; then
   LOG_OUT "Setting Secondary DNS Server List..."

   custom_domain_dns_server=$(uci get openclash.config.custom_domain_dns_server 2>/dev/null)
   [ -z "$custom_domain_dns_server" ] && {
	   custom_domain_dns_server="114.114.114.114"
	}

   if [ -s "/etc/openclash/custom/openclash_custom_domain_dns.list" ]; then
      mkdir -p ${DNSMASQ_CONF_DIR}
      awk -v tag="$custom_domain_dns_server" '!/^$/&&!/^#/{printf("server=/%s/"'tag'"\n",$0)}' /etc/openclash/custom/openclash_custom_domain_dns.list >>${DNSMASQ_CONF_DIR}/dnsmasq_openclash_custom_domain.conf 2>/dev/null
   fi
fi

del_lock
