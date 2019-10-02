#!/bin/sh

enable_redirect_dns=$(uci get openclash.config.enable_redirect_dns 2>/dev/null)
dns_port=$(uci get openclash.config.dns_port 2>/dev/null)

while :;
do
   LOGTIME=$(date "+%Y-%m-%d %H:%M:%S")
   enable=$(uci get openclash.config.enable)
   
if [ "$enable" -eq 1 ]; then
	if ! pidof clash >/dev/null; then
	   echo "${LOGTIME} Watchdog: OpenClash Problem, Restart " >>/tmp/openclash.log
	   nohup /etc/init.d/openclash restart &
	   exit 0
  fi
else
   exit 0
fi
## Log File Size Manage:
    
    LOGSIZE=`ls -l /tmp/openclash.log |awk '{print int($5/1024)}'`
    if [ "$LOGSIZE" -gt 90 ]; then 
       echo "[$LOGTIME] Watchdog: Size Limit, Clean Up All Log Records." > /tmp/openclash.log
    fi
    
## 端口转发重启
   zone_line=`iptables -t nat -nL PREROUTING --line-number |grep "zone" 2>/dev/null |awk '{print $1}' 2>/dev/null |awk 'END {print}'`
   op_line=`iptables -t nat -nL PREROUTING --line-number |grep "openclash" 2>/dev/null |awk '{print $1}' 2>/dev/null |head -1`
   if [ "$zone_line" -gt "$op_line" ]; then
      nohup /etc/init.d/openclash restart &
      echo "[$LOGTIME] Watchdog: Restart For Enable Firewall Redirect." >> /tmp/openclash.log
      exit 0
   fi
   
## DNS转发劫持
   if [ "$enable_redirect_dns" != "0" ]; then
      if [ -z "$(uci get dhcp.@dnsmasq[0].server 2>/dev/null |grep "$dns_port")" ] || [ ! -z "$(uci get dhcp.@dnsmasq[0].server 2>/dev/null |awk -F ' ' '{print $2}')" ]; then
         echo "[$LOGTIME] Watchdog: Force Reset DNS Hijack." >> /tmp/openclash.log
         uci del dhcp.@dnsmasq[-1].server >/dev/null 2>&1
         uci add_list dhcp.@dnsmasq[0].server=127.0.0.1#"$dns_port"
         uci delete dhcp.@dnsmasq[0].resolvfile
         uci set dhcp.@dnsmasq[0].noresolv=1
         uci set dhcp.@dnsmasq[0].cachesize=0
         uci commit dhcp
         /etc/init.d/dnsmasq restart >/dev/null 2>&1
      fi
   fi

   sleep 60
done 2>/dev/null
