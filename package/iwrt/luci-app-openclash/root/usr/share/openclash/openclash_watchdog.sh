#!/bin/sh
. /usr/share/openclash/log.sh
. /lib/functions.sh
. /usr/share/openclash/openclash_ps.sh



LOG_FILE="/tmp/openclash.log"
CONFIG_FILE="/etc/openclash/$(uci -q get openclash.config.config_path |awk -F '/' '{print $5}' 2>/dev/null)"
ipv6_enable=$(uci -q get openclash.config.ipv6_enable || echo 0)
enable_redirect_dns=$(uci -q get openclash.config.enable_redirect_dns)
dns_port=$(uci -q get openclash.config.dns_port)
disable_masq_cache=$(uci -q get openclash.config.disable_masq_cache)
cfg_update_interval=$(uci -q get openclash.config.config_update_interval || echo 60)
log_size=$(uci -q get openclash.config.log_size || echo 1024)
router_self_proxy=$(uci -q get openclash.config.router_self_proxy || echo 1)
stream_auto_select_interval=$(uci -q get openclash.config.stream_auto_select_interval || echo 30)
skip_proxy_address=$(uci -q get openclash.config.skip_proxy_address || echo 0)
CFG_UPDATE_INT=1
SKIP_PROXY_ADDRESS=1
SKIP_PROXY_ADDRESS_INTERVAL=30
STREAM_AUTO_SELECT=1
FW4=$(command -v fw4)

## Skip Proxies Address
skip_proxies_address()
{
ruby -ryaml -rYAML -I "/usr/share/openclash" -E UTF-8 -e "
begin
   Value = YAML.load_file('$CONFIG_FILE');
rescue Exception => e
   YAML.LOG('Error: Load File Failed,【' + e.message + '】');
end;
begin
   threads = [];
   threadsp = [];
   set_commands = [];
   reg = /([0-9a-zA-Z-]{1,}\.)+([a-zA-Z]{2,})/;
   reg4 = /^((\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])$/;
   reg6 = /^(?:(?:(?:[0-9A-Fa-f]{1,4}:){7}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){6}:[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){5}:([0-9A-Fa-f]{1,4}:)?[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){4}:([0-9A-Fa-f]{1,4}:){0,2}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){3}:([0-9A-Fa-f]{1,4}:){0,3}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){2}:([0-9A-Fa-f]{1,4}:){0,4}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){6}((\b((25[0-5])|(1\d{2})|(2[0-4]\d)|(\d{1,2}))\b)\.){3}(\b((25[0-5])|(1\d{2})|(2[0-4]\d)|(\d{1,2}))\b))|(([0-9A-Fa-f]{1,4}:){0,5}:((\b((25[0-5])|(1\d{2})|(2[0-4]\d)|(\d{1,2}))\b)\.){3}(\b((25[0-5])|(1\d{2})|(2[0-4]\d)|(\d{1,2}))\b))|(::([0-9A-Fa-f]{1,4}:){0,5}((\b((25[0-5])|(1\d{2})|(2[0-4]\d)|(\d{1,2}))\b)\.){3}(\b((25[0-5])|(1\d{2})|(2[0-4]\d)|(\d{1,2}))\b))|([0-9A-Fa-f]{1,4}::([0-9A-Fa-f]{1,4}:){0,5}[0-9A-Fa-f]{1,4})|(::([0-9A-Fa-f]{1,4}:){0,6}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){1,7}:))|\[(?:(?:(?:[0-9A-Fa-f]{1,4}:){7}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){6}:[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){5}:([0-9A-Fa-f]{1,4}:)?[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){4}:([0-9A-Fa-f]{1,4}:){0,2}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){3}:([0-9A-Fa-f]{1,4}:){0,3}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){2}:([0-9A-Fa-f]{1,4}:){0,4}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){6}((\b((25[0-5])|(1\d{2})|(2[0-4]\d)|(\d{1,2}))\b)\.){3}(\b((25[0-5])|(1\d{2})|(2[0-4]\d)|(\d{1,2}))\b))|(([0-9A-Fa-f]{1,4}:){0,5}:((\b((25[0-5])|(1\d{2})|(2[0-4]\d)|(\d{1,2}))\b)\.){3}(\b((25[0-5])|(1\d{2})|(2[0-4]\d)|(\d{1,2}))\b))|(::([0-9A-Fa-f]{1,4}:){0,5}((\b((25[0-5])|(1\d{2})|(2[0-4]\d)|(\d{1,2}))\b)\.){3}(\b((25[0-5])|(1\d{2})|(2[0-4]\d)|(\d{1,2}))\b))|([0-9A-Fa-f]{1,4}::([0-9A-Fa-f]{1,4}:){0,5}[0-9A-Fa-f]{1,4})|(::([0-9A-Fa-f]{1,4}:){0,6}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){1,7}:))\]$/i;
   if Value.key?('proxies') or Value.key?('proxy-providers') then
      firewall_v = '$FW4';
      if firewall_v.empty? then
         firewall_v = 'ipt'
      else
         firewall_v = 'nft'
      end;
      ips = Array.new;
      servers = Array.new;
      if Value.key?('proxies') and not Value['proxies'].nil? then
         Value['proxies'].each do
            |i|
            threads << Thread.new {
               if i['server'] then
                  if servers.include?(i['server']) then
                     next;
                  end; 
                  if i['server'] =~ reg then
                     servers = servers.push(i['server']).uniq
                     syscall = '/usr/share/openclash/openclash_debug_dns.lua 2>/dev/null \"' + i['server'] + '\" \"true\"'
                     result = IO.popen(syscall).read.split(/\n+/)
                     if result then
                        ips = ips | result
                     end;
                  else
                     ips = ips.push(i['server']).uniq
                  end;
               end;
            };
         end;
      end;
      if Value.key?('proxy-providers') and not Value['proxy-providers'].nil? then
         Value['proxy-providers'].values.each do
            |i,path|
            threads << Thread.new {
               if i['path'] and not i['path'].empty? then
                  if i['path'].split('/')[0] == '.' then
                     path = '/etc/openclash/'+i['path'].split('./')[1]
                  else
                     path = i['path']
                  end;
                  if File::exist?(path) then
                     if YAML.load_file(path).class == String then
                        YAML.LOG('Warning: Unsupported format, Proxies Address Skip Function Ignore Proxy-providers File【' + path + '】');
                        next
                     end;
                     if YAML.load_file(path).key?('proxies') and not YAML.load_file(path)['proxies'].nil? then
                        YAML.load_file(path)['proxies'].each do
                           |j|
                           if j['server'] then
                              threadsp << Thread.new {
                                 if servers.include?(j['server']) then
                                    next;
                                 end;
                                 if j['server'] =~ reg then
                                    servers = servers.push(j['server']).uniq
                                    syscall = '/usr/share/openclash/openclash_debug_dns.lua 2>/dev/null \"' + j['server'] + '\" \"true\"'
                                    result = IO.popen(syscall).read.split(/\n+/)
                                    if result then
                                       ips = ips | result
                                    end;
                                 else
                                    ips = ips.push(j['server']).uniq
                                 end;
                              };
                           end;
                        end;
                     end;
                  end;
               end;
               if not i['path'] and i['type'] == 'inline' and i['payload'] and not i['payload'].empty? then
                  Value['payload'].each do
                     |k|
                     threadsp << Thread.new {
                        if k['server'] then
                           if servers.include?(k['server']) then
                              next;
                           end; 
                           if k['server'] =~ reg then
                              servers = servers.push(k['server']).uniq
                              syscall = '/usr/share/openclash/openclash_debug_dns.lua 2>/dev/null \"' + k['server'] + '\" \"true\"'
                              result = IO.popen(syscall).read.split(/\n+/)
                              if result then
                                 ips = ips | result
                              end;
                           else
                              ips = ips.push(k['server']).uniq
                           end;
                        end;
                     };
                  end;
               end;
               threadsp.each(&:join);
            };
         end;
      end;
      threads.each(&:join);
      #Add ip skip
      if ips and not ips.empty? then
         threads.clear;
         ips.each do
            |ip|
            threads << Thread.new {
               if ip and ip =~ reg4 then
                  if firewall_v == 'nft' then
                     set_commands << 'nft add element inet fw4 localnetwork { \"' + ip + '\" } 2>/dev/null'
                  else
                     set_commands << 'ipset add localnetwork \"' + ip + '\" 2>/dev/null'
                  end;
               elsif ip and ip =~ reg6 then
                  if firewall_v == 'nft' then
                     set_commands << 'nft add element inet fw4 localnetwork6 { \"' + ip + '\" } 2>/dev/null'
                  else
                     set_commands << 'ipset add localnetwork6 \"' + ip + '\" 2>/dev/null'
                  end;
               end;
            };
         end;
         threads.each(&:join);
         system(set_commands.join('; '));
      end;
   end;
rescue Exception => e
   YAML.LOG('Error: Set Proxies Address Skip Failed,【' + e.message + '】');
end" 2>/dev/null >> $LOG_FILE
}

while :;
do
   cfg_update=$(uci -q get openclash.config.auto_update)
   cfg_update_mode=$(uci -q get openclash.config.config_auto_update_mode)
   cfg_update_interval_now=$(uci -q get openclash.config.config_update_interval || echo 60)
   stream_auto_select=$(uci -q get openclash.config.stream_auto_select || echo 0)
   stream_auto_select_interval_now=$(uci -q get openclash.config.stream_auto_select_interval || echo 30)
   stream_auto_select_netflix=$(uci -q get openclash.config.stream_auto_select_netflix || echo 0)
   stream_auto_select_disney=$(uci -q get openclash.config.stream_auto_select_disney || echo 0)
   stream_auto_select_hbo_max=$(uci -q get openclash.config.stream_auto_select_hbo_max || echo 0)
   stream_auto_select_tvb_anywhere=$(uci -q get openclash.config.stream_auto_select_tvb_anywhere || echo 0)
   stream_auto_select_prime_video=$(uci -q get openclash.config.stream_auto_select_prime_video || echo 0)
   stream_auto_select_ytb=$(uci -q get openclash.config.stream_auto_select_ytb || echo 0)
   stream_auto_select_dazn=$(uci -q get openclash.config.stream_auto_select_dazn || echo 0)
   stream_auto_select_paramount_plus=$(uci -q get openclash.config.stream_auto_select_paramount_plus || echo 0)
   stream_auto_select_discovery_plus=$(uci -q get openclash.config.stream_auto_select_discovery_plus || echo 0)
   stream_auto_select_bilibili=$(uci -q get openclash.config.stream_auto_select_bilibili || echo 0)
   stream_auto_select_google_not_cn=$(uci -q get openclash.config.stream_auto_select_google_not_cn || echo 0)
   stream_auto_select_openai=$(uci -q get openclash.config.stream_auto_select_openai || echo 0)
   upnp_lease_file=$(uci -q get upnpd.config.upnp_lease_file)

#wait for core start complete
while ( [ -n "$(unify_ps_pids "/etc/init.d/openclash")" ] )
do
   sleep 1
done >/dev/null 2>&1

## Porxy history
   /usr/share/openclash/openclash_history_get.sh

## Log File Size Manage:
   LOGSIZE=`ls -l /tmp/openclash.log |awk '{print int($5/1024)}'`
   if [ "$LOGSIZE" -gt "$log_size" ]; then
   : > /tmp/openclash.log
   LOG_OUT "Watchdog: Log Size Limit, Clean Up All Log Records..."
   fi

## 转发顺序
   if [ -z "$FW4" ]; then
      nat_last_line=$(iptables -t nat -nL PREROUTING --line-number |awk '{print $1}' 2>/dev/null |awk 'END {print}' |sed -n '$p')
      nat_op_line=$(iptables -t nat -nL PREROUTING --line-number |grep -E "openclash|OpenClash" |grep -Ev "DNS|dns" 2>/dev/null |awk '{print $1}' 2>/dev/null |head -1)
      man_last_line=$(iptables -t mangle -nL PREROUTING --line-number |awk '{print $1}' 2>/dev/null |awk 'END {print}' |sed -n '$p')
      man_op_line=$(iptables -t mangle -nL PREROUTING --line-number |grep -E "openclash|OpenClash" |grep -Ev "DNS|dns" 2>/dev/null |awk '{print $1}' 2>/dev/null |head -1)
      if ([ "$nat_last_line" != "$nat_op_line" ] && [ -n "$nat_op_line" ]) || ([ "$man_last_line" != "$man_op_line" ] && [ -n "$man_op_line" ]); then
         LOG_OUT "Watchdog: Setting Firewall For Rules Order..."
         /etc/init.d/openclash reload "firewall"
      fi
   fi
   if [ -n "$FW4" ]; then
      nat_last_handle=$(nft -a list chain inet fw4 dstnat |awk -F '# handle ' '{print$2}' 2>/dev/null |tr -s '\n' |sed -n '$p')
      nat_op_handle=$(nft -a list chain inet fw4 dstnat |grep -E "openclash|OpenClash" |grep -Ev "DNS|dns" |awk -F '# handle ' '{print$2}' 2>/dev/null |tail -1)
      man_last_handle=$(nft -a list chain inet fw4 mangle_prerouting |awk -F '# handle ' '{print$2}' 2>/dev/null |tr -s '\n' |sed -n '$p')
      man_op_handle=$(nft -a list chain inet fw4 mangle_prerouting |grep -E "openclash|OpenClash" |grep -Ev "DNS|dns" |awk -F '# handle ' '{print$2}' 2>/dev/null |tail -1)
      if ([ "$nat_last_handle" != "$nat_op_handle" ] && [ -n "$nat_op_handle" ]) || ([ "$man_last_handle" != "$man_op_handle" ] && [ -n "$man_op_handle" ]); then
         LOG_OUT "Watchdog: Setting Firewall For Rules Order..."
         /etc/init.d/openclash reload "firewall"
      fi
   fi

## Localnetwork 刷新
   wan_ip4s=$(/usr/share/openclash/openclash_get_network.lua "wanip" 2>/dev/null)
   wan_ip6s=$(ifconfig | grep 'inet6 addr' | awk '{print $3}' 2>/dev/null)
   lan_ip4s=$(/usr/share/openclash/openclash_get_network.lua "lan_cidr" 2>/dev/null)
   lan_ip6s=$(/usr/share/openclash/openclash_get_network.lua "lan_cidr6" 2>/dev/null)
   if [ -n "$FW4" ]; then
      if [ -n "$wan_ip4s" ]; then
         for wan_ip4 in $wan_ip4s; do
            nft add element inet fw4 localnetwork { "$wan_ip4" } 2>/dev/null
         done
      fi
      if [ -n "$lan_ip4s" ]; then
         for lan_ip4 in $lan_ip4s; do
            nft add element inet fw4 localnetwork { "$lan_ip4" } 2>/dev/null
         done
      fi

      if [ "$ipv6_enable" -eq 1 ]; then
         if [ -n "$wan_ip6s" ]; then
            for wan_ip6 in $wan_ip6s; do
               nft add element inet fw4 localnetwork6 { "$wan_ip6" } 2>/dev/null
            done
         fi
         if [ -n "$lan_ip6s" ]; then
            for lan_ip6 in $lan_ip6s; do
               nft add element inet fw4 localnetwork6 { "$lan_ip6" } 2>/dev/null
            done
         fi
      fi
   else
      if [ -n "$wan_ip4s" ]; then
         for wan_ip4 in $wan_ip4s; do
            ipset add localnetwork "$wan_ip4" 2>/dev/null
         done
      fi
      if [ -n "$lan_ip4s" ]; then
         for lan_ip4 in $lan_ip4s; do
            ipset add localnetwork "$lan_ip4" 2>/dev/null
         done
      fi
      if [ "$ipv6_enable" -eq 1 ]; then
         if [ -n "$wan_ip6s" ]; then
            for wan_ip6 in $wan_ip6s; do
               ipset add localnetwork6 "$wan_ip6" 2>/dev/null
            done
         fi
         if [ -n "$lan_ip6s" ]; then
            for lan_ip6 in $lan_ip6s; do
               ipset add localnetwork6 "$lan_ip6" 2>/dev/null
            done
         fi
      fi
   fi

## UPNP
   if [ -f "$upnp_lease_file" ]; then
      #del
      if [ -n "$FW4" ]; then
         for i in `$(nft list chain inet fw4 openclash_upnp |grep "return")`
         do
            upnp_ip=$(echo "$i" |awk -F 'ip saddr ' '{print $2}' |awk  '{print $1}')
            upnp_dp=$(echo "$i" |awk -F 'sport ' '{print $2}' |awk  '{print $1}')
            upnp_type=$(echo "$i" |awk -F 'sport ' '{print $1}' |awk  '{print $4}' |tr '[a-z]' '[A-Z]')
            if [ -n "$upnp_ip" ] && [ -n "$upnp_dp" ] && [ -n "$upnp_type" ]; then
               if [ -z "$(cat "$upnp_lease_file" |grep "$upnp_ip" |grep "$upnp_dp" |grep "$upnp_type")" ]; then
                  handle=$(nft -a list chain inet fw4 openclash_upnp |grep "$i" |awk -F '# handle ' '{print$2}')
                  nft delete rule inet fw4 openclash_upnp handle ${handle}
               fi
            fi
         done >/dev/null 2>&1
      else
         for i in `$(iptables -t mangle -nL openclash_upnp |grep "RETURN")`
         do
            upnp_ip=$(echo "$i" |awk '{print $4}')
            upnp_dp=$(echo "$i" |awk -F 'spt:' '{print $2}')
            upnp_type=$(echo "$i" |awk '{print $2}' |tr '[a-z]' '[A-Z]')
            if [ -n "$upnp_ip" ] && [ -n "$upnp_dp" ] && [ -n "$upnp_type" ]; then
               if [ -z "$(cat "$upnp_lease_file" |grep "$upnp_ip" |grep "$upnp_dp" |grep "$upnp_type")" ]; then
                  iptables -t mangle -D openclash_upnp -p "$upnp_type" -s "$upnp_ip" --sport "$upnp_dp" -j RETURN 2>/dev/null
               fi
            fi
         done >/dev/null 2>&1
      fi
      #add
      if [ -s "$upnp_lease_file" ] && [ -n "$(iptables --line-numbers -t nat -xnvL openclash_upnp 2>/dev/null)"] || [ -n "$(nft list chain inet fw4 openclash_upnp 2>/dev/null)"]; then
         cat "$upnp_lease_file" |while read -r line
         do
            if [ -n "$line" ]; then
               upnp_ip=$(echo "$line" |awk -F ':' '{print $3}')
               upnp_dp=$(echo "$line" |awk -F ':' '{print $4}')
               upnp_type=$(echo "$line" |awk -F ':' '{print $1}' |tr '[A-Z]' '[a-z]')
               if [ -n "$upnp_ip" ] && [ -n "$upnp_dp" ] && [ -n "$upnp_type" ]; then
                  if [ -n "$FW4" ]; then
                     if [ -z "$(nft list chain inet fw4 openclash_upnp |grep "$upnp_ip" |grep "$upnp_dp" |grep "$upnp_type")" ]; then
                        nft add rule inet fw4 openclash_upnp ip saddr { "$upnp_ip" } "$upnp_type" sport "$upnp_dp" counter return 2>/dev/null
                     fi
                  else
                     if [ -z "$(iptables -t mangle -nL openclash_upnp |grep "$upnp_ip" |grep "$upnp_dp" |grep "$upnp_type")" ]; then
                        iptables -t mangle -A openclash_upnp -p "$upnp_type" -s "$upnp_ip" --sport "$upnp_dp" -j RETURN 2>/dev/null
                     fi
                  fi
               fi
            fi
         done >/dev/null 2>&1
      fi
   fi

## Skip Proxies Address
   if [ "$skip_proxy_address" -eq 1 ]; then
      if [ "$SKIP_PROXY_ADDRESS" -eq 1 ] || [ "$(expr "$SKIP_PROXY_ADDRESS" % "$SKIP_PROXY_ADDRESS_INTERVAL")" -eq 0 ]; then
         skip_proxies_address
         let SKIP_PROXY_ADDRESS++
      else
         let SKIP_PROXY_ADDRESS++
      fi
   fi

## DNS转发劫持
   if [ "$enable_redirect_dns" = "1" ]; then
      if [ -z "$(uci -q get dhcp.@dnsmasq[0].server |grep "$dns_port")" ] || [ ! -z "$(uci -q get dhcp.@dnsmasq[0].server |awk -F ' ' '{print $2}')" ]; then
         LOG_OUT "Watchdog: Force Reset DNS Hijack..."
         uci -q del dhcp.@dnsmasq[-1].server
         uci -q add_list dhcp.@dnsmasq[0].server=127.0.0.1#"$dns_port"
         uci -q delete dhcp.@dnsmasq[0].resolvfile
         uci -q set dhcp.@dnsmasq[0].noresolv=1
         [ "$disable_masq_cache" -eq 1 ] && {
         	uci -q set dhcp.@dnsmasq[0].cachesize=0
         }
         uci -q commit dhcp
         /etc/init.d/dnsmasq restart >/dev/null 2>&1
      fi
   fi

## 配置文件循环更新
   if [ "$cfg_update" -eq 1 ] && [ "$cfg_update_mode" -eq 1 ]; then
      [ "$cfg_update_interval" -ne "$cfg_update_interval_now" ] && CFG_UPDATE_INT=0 && cfg_update_interval="$cfg_update_interval_now"
      if [ "$CFG_UPDATE_INT" -ne 0 ]; then
         [ "$(expr "$CFG_UPDATE_INT" % "$cfg_update_interval_now")" -eq 0 ] && /usr/share/openclash/openclash.sh
      fi
      CFG_UPDATE_INT=$(expr "$CFG_UPDATE_INT" + 1)
   fi

##Dler Cloud Checkin
   /usr/share/openclash/openclash_dler_checkin.lua >/dev/null 2>&1

##STREAMING_UNLOCK_CHECK
   if [ "$stream_auto_select" -eq 1 ] && [ "$router_self_proxy" -eq 1 ]; then
      [ "$stream_auto_select_interval" -ne "$stream_auto_select_interval_now" ] && STREAM_AUTO_SELECT=1 && stream_auto_select_interval="$stream_auto_select_interval_now"
      if [ "$STREAM_AUTO_SELECT" -ne 0 ]; then
         if [ "$(expr "$STREAM_AUTO_SELECT" % "$stream_auto_select_interval_now")" -eq 0 ] || [ "$STREAM_AUTO_SELECT" -eq 1 ]; then
            if [ "$stream_auto_select_netflix" -eq 1 ]; then
               LOG_OUT "Tip: Start Auto Select Proxy For Netflix Unlock..."
               /usr/share/openclash/openclash_streaming_unlock.lua "Netflix" >> $LOG_FILE
            fi
            if [ "$stream_auto_select_disney" -eq 1 ]; then
               LOG_OUT "Tip: Start Auto Select Proxy For Disney Plus Unlock..."
               /usr/share/openclash/openclash_streaming_unlock.lua "Disney Plus" >> $LOG_FILE
            fi
            if [ "$stream_auto_select_google_not_cn" -eq 1 ]; then
               LOG_OUT "Tip: Start Auto Select Proxy For Google Not CN Unlock..."
               /usr/share/openclash/openclash_streaming_unlock.lua "Google" >> $LOG_FILE
            fi
            if [ "$stream_auto_select_ytb" -eq 1 ]; then
               LOG_OUT "Tip: Start Auto Select Proxy For YouTube Premium Unlock..."
               /usr/share/openclash/openclash_streaming_unlock.lua "YouTube Premium" >> $LOG_FILE
            fi
            if [ "$stream_auto_select_prime_video" -eq 1 ]; then
               LOG_OUT "Tip: Start Auto Select Proxy For Amazon Prime Video Unlock..."
               /usr/share/openclash/openclash_streaming_unlock.lua "Amazon Prime Video" >> $LOG_FILE
            fi
            if [ "$stream_auto_select_hbo_max" -eq 1 ]; then
               LOG_OUT "Tip: Start Auto Select Proxy For HBO Max Unlock..."
               /usr/share/openclash/openclash_streaming_unlock.lua "HBO Max" >> $LOG_FILE
            fi
            if [ "$stream_auto_select_tvb_anywhere" -eq 1 ]; then
               LOG_OUT "Tip: Start Auto Select Proxy For TVB Anywhere+ Unlock..."
               /usr/share/openclash/openclash_streaming_unlock.lua "TVB Anywhere+" >> $LOG_FILE
            fi
            if [ "$stream_auto_select_dazn" -eq 1 ]; then
               LOG_OUT "Tip: Start Auto Select Proxy For DAZN Unlock..."
               /usr/share/openclash/openclash_streaming_unlock.lua "DAZN" >> $LOG_FILE
            fi
            if [ "$stream_auto_select_paramount_plus" -eq 1 ]; then
               LOG_OUT "Tip: Start Auto Select Proxy For Paramount Plus Unlock..."
               /usr/share/openclash/openclash_streaming_unlock.lua "Paramount Plus" >> $LOG_FILE
            fi
            if [ "$stream_auto_select_discovery_plus" -eq 1 ]; then
               LOG_OUT "Tip: Start Auto Select Proxy For Discovery Plus Unlock..."
               /usr/share/openclash/openclash_streaming_unlock.lua "Discovery Plus" >> $LOG_FILE
            fi
            if [ "$stream_auto_select_bilibili" -eq 1 ]; then
               LOG_OUT "Tip: Start Auto Select Proxy For Bilibili Unlock..."
               /usr/share/openclash/openclash_streaming_unlock.lua "Bilibili" >> $LOG_FILE
            fi
            if [ "$stream_auto_select_openai" -eq 1 ]; then
               LOG_OUT "Tip: Start Auto Select Proxy For OpenAI Unlock..."
               /usr/share/openclash/openclash_streaming_unlock.lua "OpenAI" >> $LOG_FILE
            fi
         fi
      fi
      STREAM_AUTO_SELECT=$(expr "$STREAM_AUTO_SELECT" + 1)
   elif [ "$router_self_proxy" != "1" ] && [ "$stream_auto_select" -eq 1 ]; then
      LOG_OUT "Error: Streaming Unlock Could not Work Because of Router-Self Proxy Disabled, Exiting..."
   fi
   
   SLOG_CLEAN
   sleep 60
done 2>/dev/null
