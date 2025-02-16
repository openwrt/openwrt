#!/bin/bash
. /lib/functions.sh
. /usr/share/openclash/log.sh

set_lock() {
   exec 875>"/tmp/lock/openclash_proxies_get.lock" 2>/dev/null
   flock -x 875 2>/dev/null
}

del_lock() {
   flock -u 875 2>/dev/null
   rm -rf "/tmp/lock/openclash_proxies_get.lock"
}

CONFIG_FILE=$(uci -q get openclash.config.config_path)
CONFIG_NAME=$(echo "$CONFIG_FILE" |awk -F '/' '{print $5}' 2>/dev/null)
UPDATE_CONFIG_FILE=$(uci -q get openclash.config.config_update_path)
UPDATE_CONFIG_NAME=$(echo "$UPDATE_CONFIG_FILE" |awk -F '/' '{print $5}' 2>/dev/null)
LOG_FILE="/tmp/openclash.log"
set_lock

if [ ! -z "$UPDATE_CONFIG_FILE" ]; then
   CONFIG_FILE="$UPDATE_CONFIG_FILE"
   CONFIG_NAME="$UPDATE_CONFIG_NAME"
fi

if [ -z "$CONFIG_FILE" ]; then
   for file_name in /etc/openclash/config/*
   do
      if [ -f "$file_name" ]; then
         CONFIG_FILE=$file_name
         CONFIG_NAME=$(echo "$CONFIG_FILE" |awk -F '/' '{print $5}' 2>/dev/null)
         break
      fi
   done
fi

if [ -z "$CONFIG_NAME" ]; then
   CONFIG_FILE="/etc/openclash/config/config.yaml"
   CONFIG_NAME="config.yaml"
fi

BACKUP_FILE="/etc/openclash/backup/$(echo "$CONFIG_FILE" |awk -F '/' '{print $5}' 2>/dev/null)"

if [ ! -s "$CONFIG_FILE" ] && [ ! -s "$BACKUP_FILE" ]; then
   del_lock
   exit 0
elif [ ! -s "$CONFIG_FILE" ] && [ -s "$BACKUP_FILE" ]; then
   mv "$BACKUP_FILE" "$CONFIG_FILE"
fi

CFG_FILE="/etc/config/openclash"
match_servers="/tmp/match_servers.list"
match_provider="/tmp/match_provider.list"
servers_update=$(uci -q get openclash.config.servers_update)
servers_if_update=$(uci -q get openclash.config.servers_if_update)

cfg_new_servers_groups_check()
{
   
   if [ -z "$1" ]; then
      return
   fi
   
   config_foreach cfg_group_name "groups" "$1"
}

cfg_group_name()
{
   local section="$1"
   local name config
   config_get "name" "$section" "name" ""
   config_get "config" "$section" "config" ""

   if [ -z "$config" ]; then
      return
   fi
   
   if [ "$config" != "$CONFIG_NAME" ] && [ "$config" != "all" ]; then
      return
   fi

   if [ -z "$name" ]; then
	    return
   fi

   if [ "$name" = "$2" ]; then
      config_group_exist=$(( $config_group_exist + 1 ))
   fi
}

#判断当前配置文件策略组信息是否包含指定策略组
config_group_exist=0
if [ -z "$(uci -q get openclash.config.new_servers_group)" ]; then
   config_group_exist=2
elif [ "$(uci -q get openclash.config.new_servers_group)" = "all" ]; then
   config_group_exist=1
else
   config_load "openclash"
   config_list_foreach "config" "new_servers_group" cfg_new_servers_groups_check

   if [ "$config_group_exist" -ne 0 ]; then
      config_group_exist=1
   else
      config_group_exist=0
   fi
fi

yml_provider_name_get()
{
   local section="$1"
   local name config
   config_get "name" "$section" "name" ""
   config_get "config" "$section" "config" ""
   if [ -n "$name" ] && [ "$config" = "$CONFIG_NAME" ]; then
      echo "$provider_nums.$name" >>"$match_provider"
   fi
   provider_nums=$(( $provider_nums + 1 ))
}

yml_servers_name_get()
{
	local section="$1"
   local name config
   config_get "name" "$section" "name" ""
   config_get "config" "$section" "config" ""
   if [ -n "$name" ] && [ "$config" = "$CONFIG_NAME" ]; then
      echo "$server_num.$name" >>"$match_servers"
   fi
   server_num=$(( $server_num + 1 ))
}

LOG_OUT "Start Getting【$CONFIG_NAME】Proxy-providers Setting..."

echo "" >"$match_provider"
provider_nums=0
config_load "openclash"
config_foreach yml_provider_name_get "proxy-provider"
	   
LOG_OUT "Start Getting【$CONFIG_NAME】Proxies Setting..."

echo "" >"$match_servers"
server_num=0
config_load "openclash"
config_foreach yml_servers_name_get "servers"

#获取代理集信息
ruby -ryaml -rYAML -I "/usr/share/openclash" -E UTF-8 -e "
   begin
      Value = YAML.load_file('$CONFIG_FILE');
   rescue Exception => e
      YAML.LOG('Error: Load File Failed,【' + e.message + '】');
   end;

   threads = [];
   threads_prv = [];
   threads_pr = [];
   threads_uci = [];
   uci_commands = [];

   if not Value.key?('proxy-providers') or Value['proxy-providers'].nil? then
      Value['proxy-providers'] = {};
   end;

   if not Value.key?('proxies') or Value['proxies'].nil? then
      Value['proxies'] = [];
   end;

   Value['proxy-providers'].each do |x,y|
      threads_pr << Thread.new {
         begin
            YAML.LOG('Start Getting【${CONFIG_NAME} - ' + y['type'].to_s + ' - ' + x.to_s + '】Proxy-provider Setting...');
            #代理集存在时获取代理集编号
            cmd = 'grep -F \'.' + x + '\' ${match_provider} 2>/dev/null |awk -F \".\" \'{print \$1}\'';
            provider_nums = %x(#{cmd}).chomp;
            if not provider_nums.empty? then
               cmd = 'sed -i \"/^' + provider_nums + '\./c\\#match#\" $match_provider 2>/dev/null';
               system(cmd);
               uci_set='uci -q set openclash.@proxy-provider[' + provider_nums + '].';
               uci_get='uci -q get openclash.@proxy-provider[' + provider_nums + '].';
               uci_add='uci -q add_list openclash.@proxy-provider[' + provider_nums + '].';
               uci_del='uci -q delete openclash.@proxy-provider[' + provider_nums + '].';
               cmd = uci_get + 'manual';
               if not provider_nums then
                  uci_commands << uci_set + 'manual=0';
               end;
               uci_commands << uci_set + 'type=\"' + y['type'].to_s + '\"';
            else
               #代理集不存在时添加新代理集
               uci_name_tmp=%x{uci -q add openclash proxy-provider 2>&1}.chomp;
               uci_set='uci -q set openclash.' + uci_name_tmp + '.';
               uci_add='uci -q add_list openclash.' + uci_name_tmp + '.';
               uci_del='uci -q delete openclash.' + uci_name_tmp + '.';
               
               if '$config_group_exist' == 0 and '$servers_if_update' == '1' and '$servers_update' == 1 then
                  uci_commands << uci_set + 'enabled=0';
               else
                  uci_commands << uci_set + 'enabled=1';
               end;
               uci_commands << uci_set + 'manual=0';
               uci_commands << uci_set + 'config=\"$CONFIG_NAME\"';
               next unless x && y['type'];
               uci_commands << uci_set + 'name=\"' + x.to_s + '\"';
               uci_commands << uci_set + 'type=\"' + y['type'].to_s + '\"';
            end;
            
            threads_prv << Thread.new{
               #path
               if y.key?('path') then
                  if y['type'] == 'http' then
                     uci_commands << uci_set + 'path=\"./proxy_provider/' + x + '.yaml\"'
                  else
                     uci_commands << uci_set + 'path=\"' + y['path'].to_s + '\"'
                  end
               end
            };
            
            threads_prv << Thread.new{
               #gen_url
               if y.key?('url') then
                  uci_commands << uci_set + 'provider_url=\"' + y['url'].to_s + '\"'
               end
            };
            
            threads_prv << Thread.new{
               #gen_interval
               if y.key?('interval') then
                  uci_commands << uci_set + 'provider_interval=\"' + y['interval'].to_s + '\"'
               end
            };
            
            threads_prv << Thread.new{
               #filter
               if y.key?('filter') then
                  uci_commands << uci_set + 'provider_filter=\"' + y['filter'].to_s + '\"'
               end
            };
            
            threads_prv << Thread.new{
               #che_enable
               if y.key?('health-check') then
                  if y['health-check'].key?('enable') then
                     uci_commands << uci_set + 'health_check=\"' + y['health-check']['enable'].to_s + '\"'
                  end
               end
            };
            
            threads_prv << Thread.new{
               #che_url
               if y.key?('health-check') then
                  if y['health-check'].key?('url') then
                     uci_commands << uci_set + 'health_check_url=\"' + y['health-check']['url'].to_s + '\"'
                  end
               end
            };
            
            threads_prv << Thread.new{
               #che_interval
               if y.key?('health-check') then
                  if y['health-check'].key?('interval') then
                     uci_commands << uci_set + 'health_check_interval=\"' + y['health-check']['interval'].to_s + '\"'
                  end
               end
            };

            threads_prv << Thread.new{
               #加入策略组
               if '$servers_if_update' == '1' and '$config_group_exist' == '1' and '$servers_update' == '1' and provider_nums.empty? then
                  #新代理集且设置默认策略组时加入指定策略组
                  new_provider_groups = %x{uci get openclash.config.new_servers_group}.chomp.split(\"'\").map { |x| x.strip }.reject { |x| x.empty? };
                  new_provider_groups.each do |x|
                     uci_commands << uci_add + 'groups=\"^' + x + '$\"'
                  end
               elsif '$servers_if_update' != '1' then
                  threads_agr = [];
                  cmd = uci_del + 'groups >/dev/null 2>&1';
                  system(cmd);
                  Value['proxy-groups'].each{
                  |z|
                     threads_agr << Thread.new {
                        if z.key?('use') then
                           z['use'].each{
                           |v|
                           if v == x then
                              uci_commands << uci_add + 'groups=\"^' + z['name'] + '$\"'
                              break
                           end
                           }
                        end
                     };
                  };
                  threads_agr.each(&:join)
               end;
            };
            threads_prv.each(&:join)
         rescue Exception => e
            YAML.LOG('Error: Resolve Proxy-providers Failed,【${CONFIG_NAME} - ' + x + ': ' + e.message + '】');
         end;
      };
   end;
   
   Value['proxies'].each do |x|
      threads_pr << Thread.new {
         begin
            YAML.LOG('Start Getting【${CONFIG_NAME} - ' + x['type'].to_s + ' - ' + x['name'].to_s + '】Proxy Setting...');
            #节点存在时获取节点编号
            cmd = 'grep -F \'.' + x['name'].to_s + '\' ${match_servers} 2>/dev/null |awk -F \".\" \'{print \$1}\'';
            server_num = %x(#{cmd}).chomp;
            if not server_num.empty? then
               #更新已有节点
               cmd = 'sed -i \"/^' + server_num + '\./c\\#match#\" $match_servers 2>/dev/null';
               system(cmd);
               uci_set='uci -q set openclash.@servers[' + server_num + '].';
               uci_get='uci -q get openclash.@servers[' + server_num + '].';
               uci_add='uci -q add_list openclash.@servers[' + server_num + '].';
               uci_del='uci -q delete openclash.@servers[' + server_num + '].';
               cmd = uci_get + 'manual';
               if not server_num then
                  uci_commands << uci_set + 'manual=0';
               end;
            else
               #添加新节点
               uci_name_tmp=%x{uci -q add openclash servers 2>&1}.chomp;
               uci_set='uci -q set openclash.' + uci_name_tmp + '.';
               uci_add='uci -q add_list openclash.' + uci_name_tmp + '.';
               uci_del='uci -q delete openclash.' + uci_name_tmp + '.';
               if '$config_group_exist' == 0 and '$servers_if_update' == '1' and '$servers_update' == 1 then
                  uci_commands << uci_set + 'enabled=0';
               else
                  uci_commands << uci_set + 'enabled=1';
               end;
               uci_commands << uci_set + 'manual=0';
               uci_commands << uci_set + 'config=\"$CONFIG_NAME\"';
               next unless x['name'] && x['type'];
               if x.key?('name') then
                  uci_commands << uci_set + 'name=\"' + x['name'].to_s + '\"';
               end;
            end;

            threads << Thread.new{
               #type
               if x.key?('type') then
                  uci_commands << uci_set + 'type=\"' + x['type'].to_s + '\"';
               end
            };

            threads << Thread.new{
               #server
               if x.key?('server') then
                  uci_commands << uci_set + 'server=\"' + x['server'].to_s + '\"'
               end
            };

            threads << Thread.new{
               #port
               if x.key?('port') then
                  uci_commands << uci_set + 'port=\"' + x['port'].to_s + '\"'
               end
            };

            threads << Thread.new{
               #udp
               if x.key?('udp') then
                  uci_commands << uci_set + 'udp=\"' + x['udp'].to_s + '\"'
               end
            };
            
            threads << Thread.new{
               #interface-name
               if x.key?('interface-name') then
                  uci_commands << uci_set + 'interface_name=\"' + x['interface-name'].to_s + '\"'
               end
            };
            
            threads << Thread.new{
               #routing-mark
               if x.key?('routing-mark') then
                  uci_commands << uci_set + 'routing_mark=\"' + x['routing-mark'].to_s + '\"'
               end;
            };

            threads << Thread.new{
               #ip_version
               if x.key?('ip-version') then
                  uci_commands << uci_set + 'ip_version=\"' + x['ip-version'].to_s + '\"'
               end
            };

            threads << Thread.new{
               #TFO
               if x.key?('tfo') then
                  uci_commands << uci_set + 'tfo=\"' + x['tfo'].to_s + '\"'
               end
            };

            if x['type'] == 'ss' then
               threads << Thread.new{
                  #cipher
                  if x.key?('cipher') then
                     uci_commands << uci_set + 'cipher=\"' + x['cipher'].to_s + '\"'
                  end
               };

               threads << Thread.new{
                  #udp-over-tcp
                  if x.key?('udp-over-tcp') then
                     uci_commands << uci_set + 'udp_over_tcp=\"' + x['udp-over-tcp'].to_s + '\"'
                  end
               };

               threads << Thread.new{
                  #Multiplex
                  if x.key?('smux') then
                     if x['smux'].key?('enabled') then
                        uci_commands << uci_set + 'multiplex=\"' + x['smux']['enabled'].to_s + '\"'
                     end;
                     #multiplex_protocol
                     if x['smux'].key?('protocol') then
                        uci_commands << uci_set + 'multiplex_protocol=\"' + x['smux']['protocol'].to_s + '\"'
                     end;
                     #multiplex_max_connections
                     if x['smux'].key?('max-connections') then
                        uci_commands << uci_set + 'multiplex_max_connections=\"' + x['smux']['max-connections'].to_s + '\"'
                     end;
                     #multiplex_min_streams
                     if x['smux'].key?('min-streams') then
                        uci_commands << uci_set + 'multiplex_min_streams=\"' + x['smux']['min-streams'].to_s + '\"'
                     end;
                     #multiplex_max_streams
                     if x['smux'].key?('max-streams') then
                        uci_commands << uci_set + 'multiplex_max_streams=\"' + x['smux']['max-streams'].to_s + '\"'
                     end;
                     #multiplex_padding
                     if x['smux'].key?('padding') then
                        uci_commands << uci_set + 'multiplex_padding=\"' + x['smux']['padding'].to_s + '\"'
                     end;
                     #multiplex_statistic
                     if x['smux'].key?('statistic') then
                        uci_commands << uci_set + 'multiplex_statistic=\"' + x['smux']['statistic'].to_s + '\"'
                     end;
                     #multiplex_only_tcp
                     if x['smux'].key?('only-tcp') then
                        uci_commands << uci_set + 'multiplex_only_tcp=\"' + x['smux']['only-tcp'].to_s + '\"'
                     end;
                  end;
               };

               threads << Thread.new{
                  #plugin-opts
                  if x.key?('plugin-opts') then
                     #mode
                     if x['plugin-opts'].key?('mode') then
                        uci_commands << uci_set + 'obfs=\"' + x['plugin-opts']['mode'].to_s + '\"'
                     else
                        uci_commands << uci_set + 'obfs=none'
                     end
                     #host:
                     if x['plugin-opts'].key?('host') then
                        uci_commands << uci_set + 'host=\"' + x['plugin-opts']['host'].to_s + '\"'
                     end
                     #fingerprint
                     if x['plugin-opts'].key?('fingerprint') then
                        uci_commands << uci_set + 'fingerprint=\"' + x['plugin-opts']['fingerprint'].to_s + '\"'
                     end
                     if x['plugin'].to_s == 'v2ray-plugin' then
                        #path
                        if x['plugin-opts'].key?('path') then
                           uci_commands << uci_set + 'path=\"' + x['plugin-opts']['path'].to_s + '\"'
                        end
                        #mux
                        if x['plugin-opts'].key?('mux') then
                           uci_commands << uci_set + 'mux=\"' + x['plugin-opts']['mux'].to_s + '\"'
                        end
                        #headers
                        if x['plugin-opts'].key?('headers') then
                           if x['plugin-opts']['headers'].key?('custom') then
                              uci_commands << uci_set + 'custom=\"' + x['plugin-opts']['headers']['custom'].to_s + '\"'
                           end
                        end
                        #tls
                        if x['plugin-opts'].key?('tls') then
                           uci_commands << uci_set + 'tls=\"' + x['plugin-opts']['tls'].to_s + '\"'
                        end
                        #skip-cert-verify
                        if x['plugin-opts'].key?('skip-cert-verify') then
                           uci_commands << uci_set + 'skip_cert_verify=\"' + x['plugin-opts']['skip-cert-verify'].to_s + '\"'
                        end
                     end;
                     if x['plugin'].to_s == 'shadow-tls' then
                        uci_commands << uci_set + 'obfs=\"' + x['plugin'].to_s + '\"'
                        #password
                        if x['plugin-opts'].key?('password') then
                           uci_commands << uci_set + 'obfs_password=\"' + x['plugin-opts']['password'].to_s + '\"'
                        end
                     end;
                     if x['plugin'].to_s == 'restls' then
                        uci_commands << uci_set + 'obfs=\"' + x['plugin'].to_s + '\"'
                        #password
                        if x['plugin-opts'].key?('password') then
                           uci_commands << uci_set + 'obfs_password=\"' + x['plugin-opts']['password'].to_s + '\"'
                        end
                        #version-hint
                        if x['plugin-opts'].key?('version-hint') then
                           uci_commands << uci_set + 'obfs_version_hint=\"' + x['plugin-opts']['version-hint'].to_s + '\"'
                        end
                        #restls-script
                        if x['plugin-opts'].key?('restls-script') then
                           uci_commands << uci_set + 'obfs_restls_script=\"' + x['plugin-opts']['restls-script'].to_s + '\"'
                        end
                     end;
                  end
               };
            end;

            if x['type'] == 'ssr' then
               threads << Thread.new{
               #cipher
               if x.key?('cipher') then
                  if x['cipher'].to_s == 'none' then
                     uci_commands << uci_set + 'cipher_ssr=dummy'
                  else
                     uci_commands << uci_set + 'cipher_ssr=\"' + x['cipher'].to_s + '\"'
                  end
               end
               };
               
               threads << Thread.new{
               #obfs
               if x.key?('obfs') then
                  uci_commands << uci_set + 'obfs_ssr=\"' + x['obfs'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #protocol
               if x.key?('protocol') then
                  uci_commands << uci_set + 'protocol=\"' + x['protocol'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #obfs-param
               if x.key?('obfs-param') then
                  uci_commands << uci_set + 'obfs_param=\"' + x['obfs-param'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #protocol-param
               if x.key?('protocol-param') then
                  uci_commands << uci_set + 'protocol_param=\"' + x['protocol-param'].to_s + '\"'
               end
               };
            end;

            if x['type'] == 'vmess' then
               threads << Thread.new{
               #uuid
               if x.key?('uuid') then
                  uci_commands << uci_set + 'uuid=\"' + x['uuid'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #alterId
               if x.key?('alterId') then
                  uci_commands << uci_set + 'alterId=\"' + x['alterId'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #cipher
               if x.key?('cipher') then
                  uci_commands << uci_set + 'securitys=\"' + x['cipher'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #xudp
               if x.key?('xudp') then
                  uci_commands << uci_set + 'xudp=\"' + x['xudp'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #packet_encoding
               if x.key?('packet-encoding') then
                  uci_commands << uci_set + 'packet_encoding=\"' + x['packet-encoding'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #GlobalPadding
               if x.key?('global-padding') then
                  uci_commands << uci_set + 'global_padding=\"' + x['global-padding'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #authenticated_length
               if x.key?('authenticated-length') then
                  uci_commands << uci_set + 'authenticated_length=\"' + x['authenticated-length'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #tls
               if x.key?('tls') then
                  uci_commands << uci_set + 'tls=\"' + x['tls'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #skip-cert-verify
               if x.key?('skip-cert-verify') then
                  uci_commands << uci_set + 'skip_cert_verify=\"' + x['skip-cert-verify'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #servername
               if x.key?('servername') then
                  uci_commands << uci_set + 'servername=\"' + x['servername'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #fingerprint
               if x.key?('fingerprint') then
                  uci_commands << uci_set + 'fingerprint=\"' + x['fingerprint'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #client_fingerprint
               if x.key?('client-fingerprint') then
                  uci_commands << uci_set + 'client_fingerprint=\"' + x['client-fingerprint'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #network:
               if x.key?('network') then
                  if x['network'].to_s == 'ws'
                     uci_commands << uci_set + 'obfs_vmess=websocket'
                     #ws-path:
                     if x.key?('ws-path') then
                        uci_commands << uci_set + 'ws_opts_path=\"' + x['ws-path'].to_s + '\"'
                     end
                     #Host:
                     if x.key?('ws-headers') then
                        cmd = uci_del + 'ws_opts_headers >/dev/null 2>&1'
                        system(cmd)
                        x['ws-headers'].keys.each{
                        |v|
                           uci_commands << uci_add + 'ws_opts_headers=\"' + v.to_s + ': '+ x['ws-headers'][v].to_s + '\"'
                        }
                     end
                     #ws-opts-path:
                     if x.key?('ws-opts') then
                        if x['ws-opts'].key?('path') then
                           uci_commands << uci_set + 'ws_opts_path=\"' + x['ws-opts']['path'].to_s + '\"'
                        end
                        #ws-opts-headers:
                        if x['ws-opts'].key?('headers') then
                           cmd = uci_del + 'ws_opts_headers >/dev/null 2>&1'
                           system(cmd)
                           x['ws-opts']['headers'].keys.each{
                           |v|
                              uci_commands << uci_add + 'ws_opts_headers=\"' + v.to_s + ': '+ x['ws-opts']['headers'][v].to_s + '\"'
                           }
                        end
                        #max-early-data:
                        if x['ws-opts'].key?('max-early-data') then
                           uci_commands << uci_set + 'max_early_data=\"' + x['ws-opts']['max-early-data'].to_s + '\"'
                        end
                        #early-data-header-name:
                        if x['ws-opts'].key?('early-data-header-name') then
                           uci_commands << uci_set + 'early_data_header_name=\"' + x['ws-opts']['early-data-header-name'].to_s + '\"'
                        end
                     end
                  elsif x['network'].to_s == 'http'
                     uci_commands << uci_set + 'obfs_vmess=http'
                     if x.key?('http-opts') then
                        if x['http-opts'].key?('path') then
                           cmd = uci_del + 'http_path >/dev/null 2>&1'
                           system(cmd)
                           x['http-opts']['path'].each{
                           |x|
                           uci_commands << uci_add + 'http_path=\"' + x.to_s + '\"'
                           }
                        end
                        if x['http-opts'].key?('headers') then
                           if x['http-opts']['headers'].key?('Connection') then
                              if x['http-opts']['headers']['Connection'].include?('keep-alive') then
                                 uci_commands << uci_set + 'keep_alive=true'
                              else
                                 uci_commands << uci_set + 'keep_alive=false'
                              end
                           end
                        end
                     end
                  elsif x['network'].to_s == 'h2'
                     uci_commands << uci_set + 'obfs_vmess=h2'
                     if x.key?('h2-opts') then
                        if x['h2-opts'].key?('host') then
                           cmd = uci_del + 'h2_host >/dev/null 2>&1'
                           system(cmd)
                           x['h2-opts']['host'].each{
                           |x|
                           uci_commands << uci_add + 'h2_host=\"' + x.to_s + '\"'
                           }
                        end
                        if x['h2-opts'].key?('path') then
                           uci_commands << uci_set + 'h2_path=\"' + x['h2-opts']['path'].to_s + '\"'
                        end
                     end
                  elsif x['network'].to_s == 'grpc'
                     #grpc-service-name
                     uci_commands << uci_set + 'obfs_vmess=grpc'
                     if x.key?('grpc-opts') then
                        if x['grpc-opts'].key?('grpc-service-name') then
                           uci_commands << uci_set + 'grpc_service_name=\"' + x['grpc-opts']['grpc-service-name'].to_s + '\"'
                        end
                     end
                  else
                     uci_commands << uci_set + 'obfs_vmess=none'
                  end
               end
               };
            end;

            #Mieru
            if x['type'] == 'mieru' then
               threads << Thread.new{
               #port-range
               if x.key?('port-range') then
                  uci_commands << uci_set + 'port_range=\"' + x['port-range'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #username
               if x.key?('username') then
                  uci_commands << uci_set + 'username=\"' + x['username'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #transport
               if x.key?('transport') then
                  uci_commands << uci_set + 'transport=\"' + x['transport'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #multiplexing
               if x.key?('multiplexing') then
                  uci_commands << uci_set + 'multiplexing=\"' + x['multiplexing'].to_s + '\"'
               end
               };
            end;

            #Tuic
            if x['type'] == 'tuic' then
               threads << Thread.new{
               #tc_ip
               if x.key?('ip') then
                  uci_commands << uci_set + 'tc_ip=\"' + x['ip'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #tc_token
               if x.key?('token') then
                  uci_commands << uci_set + 'tc_token=\"' + x['token'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #heartbeat_interval
               if x.key?('heartbeat-interval') then
                  uci_commands << uci_set + 'heartbeat_interval=\"' + x['heartbeat-interval'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #tc_alpn
               if x.key?('alpn') then
                  cmd = uci_del + 'tc_alpn >/dev/null 2>&1'
                  system(cmd)
                  x['alpn'].each{
                  |x|
                     uci_commands << uci_add + 'tc_alpn=\"' + x.to_s + '\"'
                  }
               end;
               };

               threads << Thread.new{
               #disable_sni
               if x.key?('disable-sni') then
                  uci_commands << uci_set + 'disable_sni=\"' + x['disable-sni'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #reduce_rtt
               if x.key?('reduce-rtt') then
                  uci_commands << uci_set + 'reduce_rtt=\"' + x['reduce-rtt'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #fast_open
               if x.key?('fast-open') then
                  uci_commands << uci_set + 'fast_open=\"' + x['fast-open'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #request_timeout
               if x.key?('request-timeout') then
                  uci_commands << uci_set + 'request_timeout=\"' + x['request-timeout'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #udp_relay_mode
               if x.key?('udp-relay-mode') then
                  uci_commands << uci_set + 'udp_relay_mode=\"' + x['udp-relay-mode'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #congestion_controller
               if x.key?('congestion-controller') then
                  uci_commands << uci_set + 'congestion_controller=\"' + x['congestion-controller'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #max_udp_relay_packet_size
               if x.key?('max-udp-relay-packet-size') then
                  uci_commands << uci_set + 'max_udp_relay_packet_size=\"' + x['max-udp-relay-packet-size'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #max-open-streams
               if x.key?('max-open-streams') then
                  uci_commands << uci_set + 'max_open_streams=\"' + x['max-open-streams'].to_s + '\"'
               end
               };
            end;

            #WireGuard
            if x['type'] == 'wireguard' then
               threads << Thread.new{
               #wg_ip
               if x.key?('ip') then
                  uci_commands << uci_set + 'wg_ip=\"' + x['ip'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #wg_ipv6
               if x.key?('ipv6') then
                  uci_commands << uci_set + 'wg_ipv6=\"' + x['ipv6'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #private_key
               if x.key?('private-key') then
                  uci_commands << uci_set + 'private_key=\"' + x['private-key'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #public_key
               if x.key?('public-key') then
                  uci_commands << uci_set + 'public_key=\"' + x['public-key'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #preshared_key
               if x.key?('preshared-key') then
                  uci_commands << uci_set + 'preshared_key=\"' + x['preshared-key'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #wg_mtu
               if x.key?('mtu') then
                  uci_commands << uci_set + 'wg_mtu=\"' + x['mtu'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #wg_dns
               if x.key?('dns') then
                  cmd =  uci_del + 'wg_dns >/dev/null 2>&1'
                  system(cmd)
                  x['dns'].each{
                  |x|
                     uci_commands << uci_add + 'wg_dns=\"' + x.to_s + '\"'
                  }
               end;
               };
            end;

            if x['type'] == 'hysteria' then
               #hysteria
               threads << Thread.new{
               #hysteria_protocol
               if x.key?('protocol') then
                  uci_commands << uci_set + 'hysteria_protocol=\"' + x['protocol'].to_s + '\"'
               end
               };
            end;

            if x['type'] == 'hysteria2' then
               #hysteria2
               threads << Thread.new{
               #hysteria2_protocol
               if x.key?('protocol') then
                  uci_commands << uci_set + 'hysteria2_protocol=\"' + x['protocol'].to_s + '\"'
               end
               };
            end;

            if x['type'] == 'hysteria' or x['type'] == 'hysteria2' then
               #hysteria  hysteria2
               threads << Thread.new{
               #hysteria_up
               if x.key?('up') then
                  uci_commands << uci_set + 'hysteria_up=\"' + x['up'].to_s + '\"'
               end
               };

               #hysteria  hysteria2
               threads << Thread.new{
               #hysteria_down
               if x.key?('down') then
                  uci_commands << uci_set + 'hysteria_down=\"' + x['down'].to_s + '\"'
               end
               };

               #hysteria  hysteria2
               threads << Thread.new{
               #skip-cert-verify
               if x.key?('skip-cert-verify') then
                  uci_commands << uci_set + 'skip_cert_verify=\"' + x['skip-cert-verify'].to_s + '\"'
               end
               };

               #hysteria  hysteria2
               threads << Thread.new{
               #sni
               if x.key?('sni') then
                  uci_commands << uci_set + 'sni=\"' + x['sni'].to_s + '\"'
               end
               };

               #hysteria  hysteria2
               threads << Thread.new{
               #alpn
               if x.key?('alpn') then
                  cmd = uci_del + 'hysteria_alpn >/dev/null 2>&1'
                  system(cmd)
                  if x['alpn'].class.to_s != 'Array' then
                     uci_commands << uci_add + 'hysteria_alpn=\"' + x['alpn'].to_s + '\"'
                  else
                     x['alpn'].each{
                     |x|
                        uci_commands << uci_add + 'hysteria_alpn=\"' + x.to_s + '\"'
                     }
                  end
               end;
               };

               #hysteria
               threads << Thread.new{
               #recv_window_conn
               if x.key?('recv-window-conn') then
                  uci_commands << uci_set + 'recv_window_conn=\"' + x['recv-window-conn'].to_s + '\"'
               end
               };

               #hysteria
               threads << Thread.new{
               #recv_window
               if x.key?('recv-window') then
                  uci_commands << uci_set + 'recv_window=\"' + x['recv-window'].to_s + '\"'
               end
               };

               #hysteria2
               threads << Thread.new{
               #initial_stream_receive_window
               if x.key?('initial-stream-receive-window') then
                  uci_commands << uci_set + 'initial_stream_receive_window=\"' + x['initial-stream-receive-window'].to_s + '\"'
               end
               };

               #hysteria2
               threads << Thread.new{
               #max_stream_receive_window
               if x.key?('max-stream-receive-window') then
                  uci_commands << uci_set + 'max_stream_receive_window=\"' + x['max-stream-receive-window'].to_s + '\"'
               end
               };

               #hysteria2
               threads << Thread.new{
               #initial_connection_receive_window
               if x.key?('initial-connection-receive-window') then
                  uci_commands << uci_set + 'initial_connection_receive_window=\"' + x['initial-connection-receive-window'].to_s + '\"'
               end
               };

               #hysteria2
               threads << Thread.new{
               #max_connection_receive_window
               if x.key?('max-connection-receive-window') then
                  uci_commands << uci_set + 'max_connection_receive_window=\"' + x['max-connection-receive-window'].to_s + '\"'
               end
               };

               #hysteria  hysteria2
               threads << Thread.new{
               #hysteria_obfs
               if x.key?('obfs') then
                  uci_commands << uci_set + 'hysteria_obfs=\"' + x['obfs'].to_s + '\"'
               end
               };

               #hysteria  hysteria2
               threads << Thread.new{
               #hysteria_obfs_password
               if x.key?('obfs-password') then
                  uci_commands << uci_set + 'hysteria_obfs_password=\"' + x['obfs-password'].to_s + '\"'
               end
               };

               #hysteria
               threads << Thread.new{
               #hysteria_auth
               if x.key?('auth') then
                  uci_commands << uci_set + 'hysteria_auth=\"' + x['auth'].to_s + '\"'
               end
               };

               #hysteria
               threads << Thread.new{
               #hysteria_auth_str
               if x.key?('auth-str') then
                  uci_commands << uci_set + 'hysteria_auth_str=\"' + x['auth-str'].to_s + '\"'
               end
               };

               #hysteria  hysteria2
               threads << Thread.new{
               #hysteria_ca
               if x.key?('ca') then
                  uci_commands << uci_set + 'hysteria_ca=\"' + x['ca'].to_s + '\"'
               end
               };

               #hysteria  hysteria2
               threads << Thread.new{
               #hysteria_ca_str
               if x.key?('ca-str') then
                  uci_commands << uci_set + 'hysteria_ca_str=\"' + x['ca-str'].to_s + '\"'
               end
               };

               #hysteria
               threads << Thread.new{
               #disable_mtu_discovery
               if x.key?('disable-mtu-discovery') then
                  uci_commands << uci_set + 'disable_mtu_discovery=\"' + x['disable-mtu-discovery'].to_s + '\"'
               end
               };

               #hysteria
               threads << Thread.new{
               #fast_open
               if x.key?('fast-open') then
                  uci_commands << uci_set + 'fast_open=\"' + x['fast-open'].to_s + '\"'
               end
               };

               #hysteria  hysteria2
               threads << Thread.new{
               #fingerprint
               if x.key?('fingerprint') then
                  uci_commands << uci_set + 'fingerprint=\"' + x['fingerprint'].to_s + '\"'
               end
               };

               #hysteria  hysteria2
               threads << Thread.new{
               #ports
               if x.key?('ports') then
                  uci_commands << uci_set + 'ports=\"' + x['ports'].to_s + '\"'
               end
               };

               #hysteria  hysteria2
               threads << Thread.new{
               #hop-interval
               if x.key?('hop-interval') then
                  uci_commands << uci_set + 'hop_interval=\"' + x['hop-interval'].to_s + '\"'
               end
               };
            end;

            if x['type'] == 'vless' then
               threads << Thread.new{
               #uuid
               if x.key?('uuid') then
                  uci_commands << uci_set + 'uuid=\"' + x['uuid'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #tls
               if x.key?('tls') then
                  uci_commands << uci_set + 'tls=\"' + x['tls'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #skip-cert-verify
               if x.key?('skip-cert-verify') then
                  uci_commands << uci_set + 'skip_cert_verify=\"' + x['skip-cert-verify'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #servername
               if x.key?('servername') then
                  uci_commands << uci_set + 'servername=\"' + x['servername'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #flow
               if x.key?('flow') then
                  uci_commands << uci_set + 'vless_flow=\"' + x['flow'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #network:
               if x.key?('network') then
                  if x['network'].to_s == 'ws'
                     uci_commands << uci_set + 'obfs_vless=ws'
                     #ws-opts-path:
                     if x.key?('ws-opts') then
                        if x['ws-opts'].key?('path') then
                           uci_commands << uci_set + 'ws_opts_path=\"' + x['ws-opts']['path'].to_s + '\"'
                        end
                        #ws-opts-headers:
                        if x['ws-opts'].key?('headers') then
                           cmd = uci_del + 'ws_opts_headers >/dev/null 2>&1'
                           system(cmd)
                           x['ws-opts']['headers'].keys.each{
                           |v|
                              uci_commands << uci_add + 'ws_opts_headers=\"' + v.to_s + ': '+ x['ws-opts']['headers'][v].to_s + '\"'
                           }
                        end
                     end
                  elsif x['network'].to_s == 'grpc'
                     #grpc-service-name
                     uci_commands << uci_set + 'obfs_vless=grpc'
                     if x.key?('grpc-opts') then
                        if x['grpc-opts'].key?('grpc-service-name') then
                           uci_commands << uci_set + 'grpc_service_name=\"' + x['grpc-opts']['grpc-service-name'].to_s + '\"'
                        end
                     end
                     if x.key?('reality-opts') then
                        if x['reality-opts'].key?('public-key') then
                           uci_commands << uci_set + 'reality_public_key=\"' + x['reality-opts']['public-key'].to_s + '\"'
                        end
                        if x['reality-opts'].key?('short-id') then
                           uci_commands << uci_set + 'reality_short_id=\"' + x['reality-opts']['short-id'].to_s + '\"'
                        end
                     end
                  elsif x['network'].to_s == 'tcp'
                     uci_commands << uci_set + 'obfs_vless=tcp'
                     if x.key?('reality-opts') then
                        if x['reality-opts'].key?('public-key') then
                           uci_commands << uci_set + 'reality_public_key=\"' + x['reality-opts']['public-key'].to_s + '\"'
                        end
                        if x['reality-opts'].key?('short-id') then
                           uci_commands << uci_set + 'reality_short_id=\"' + x['reality-opts']['short-id'].to_s + '\"'
                        end
                     end
                  end
               end
               };

               threads << Thread.new{
               #xudp
               if x.key?('xudp') then
                  uci_commands << uci_set + 'xudp=\"' + x['xudp'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #packet-addr
               if x.key?('packet-addr') then
                  uci_commands << uci_set + 'packet_addr=\"' + x['packet-addr'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #packet_encoding
               if x.key?('packet-encoding') then
                  uci_commands << uci_set + 'packet_encoding=\"' + x['packet-encoding'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #fingerprint
               if x.key?('fingerprint') then
                  uci_commands << uci_set + 'fingerprint=\"' + x['fingerprint'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #client_fingerprint
               if x.key?('client-fingerprint') then
                  uci_commands << uci_set + 'client_fingerprint=\"' + x['client-fingerprint'].to_s + '\"'
               end
               };
            end;

            if x['type'] == 'snell' then
               threads << Thread.new{
               if x.key?('obfs-opts') then
                  if x['obfs-opts'].key?('mode') then
                     uci_commands << uci_set + 'obfs_snell=\"' + x['obfs-opts']['mode'].to_s + '\"'
                  else
                     uci_commands << uci_set + 'obfs_snell=none'
                  end
                  if x['obfs-opts'].key?('host') then
                     uci_commands << uci_set + 'host=\"' + x['obfs-opts']['host'].to_s + '\"'
                  end
               end
               };
               
               threads << Thread.new{
               if x.key?('psk') then
                  uci_commands << uci_set + 'psk=\"' + x['psk'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               if x.key?('version') then
                  uci_commands << uci_set + 'snell_version=\"' + x['version'].to_s + '\"'
               end
               };
            end;

            if x['type'] == 'socks5' or x['type'] == 'http' then
               threads << Thread.new{
               if x.key?('username') then
                  uci_commands << uci_set + 'auth_name=\"' + x['username'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               if x.key?('password') then
                  uci_commands << uci_set + 'auth_pass=\"' + x['password'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #tls
               if x.key?('tls') then
                  uci_commands << uci_set + 'tls=\"' + x['tls'].to_s + '\"'
               end
               };
               
               threads << Thread.new{
               #skip-cert-verify
               if x.key?('skip-cert-verify') then
                  uci_commands << uci_set + 'skip_cert_verify=\"' + x['skip-cert-verify'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #http-headers:
               if x.key?('headers') then
                  cmd = uci_del + 'http_headers >/dev/null 2>&1'
                  system(cmd)
                  x['headers'].keys.each{
                  |v|
                     uci_commands << uci_add + 'http_headers=\"' + v.to_s + ': '+ x['headers'][v].to_s + '\"'
                  }
               end
               };

               threads << Thread.new{
               #fingerprint
               if x.key?('fingerprint') then
                  uci_commands << uci_set + 'fingerprint=\"' + x['fingerprint'].to_s + '\"'
               end
               };
            else
               threads << Thread.new{
               if x.key?('password') then
                  uci_commands << uci_set + 'password=\"' + x['password'].to_s + '\"'
               end
               };
            end;

            if x['type'] == 'http' or x['type'] == 'trojan' then
               threads << Thread.new{
               if x.key?('sni') then
                  uci_commands << uci_set + 'sni=\"' + x['sni'].to_s + '\"'
               end
               };
            end;

            if x['type'] == 'trojan' then
               threads << Thread.new{
               #alpn
               if x.key?('alpn') then
                  alpn = uci_del + 'alpn >/dev/null 2>&1'
                  system(alpn)
                  x['alpn'].each{
                  |x|
                     uci_commands << uci_add + 'alpn=\"' + x.to_s + '\"'
                  }
                  end
               };
               
               threads << Thread.new{
               #grpc-service-name
               if x.key?('grpc-opts') then
                  uci_commands << uci_set + 'obfs_trojan=grpc'
                  if x['grpc-opts'].key?('grpc-service-name') then
                     uci_commands << uci_set + 'grpc_service_name=\"' + x['grpc-opts']['grpc-service-name'].to_s + '\"'
                  end
               end
               };
               
               threads << Thread.new{
               if x.key?('ws-opts') then
                  uci_commands << uci_set + 'obfs_trojan=ws'
                  #trojan_ws_path
                  if x['ws-opts'].key?('path') then
                     uci_commands << uci_set + 'trojan_ws_path=\"' + x['ws-opts']['path'].to_s + '\"'
                  end
                  #trojan_ws_headers
                  if x['ws-opts'].key?('headers') then
                     cmd = uci_del + 'trojan_ws_headers >/dev/null 2>&1'
                     system(cmd)
                     x['ws-opts']['headers'].keys.each{
                     |v|
                        uci_commands << uci_add + 'trojan_ws_headers=\"' + v.to_s + ': '+ x['ws-opts']['headers'][v].to_s + '\"'
                     }
                  end
               end
               };
               
               threads << Thread.new{
               #skip-cert-verify
               if x.key?('skip-cert-verify') then
                  uci_commands << uci_set + 'skip_cert_verify=\"' + x['skip-cert-verify'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #fingerprint
               if x.key?('fingerprint') then
                  uci_commands << uci_set + 'fingerprint=\"' + x['fingerprint'].to_s + '\"'
               end
               };

               threads << Thread.new{
               #client_fingerprint
               if x.key?('client-fingerprint') then
                  uci_commands << uci_set + 'client_fingerprint=\"' + x['client-fingerprint'].to_s + '\"'
               end
               };
            end;

            #加入策略组
            threads << Thread.new{
               #加入策略组
               if '$servers_if_update' == '1' and '$config_group_exist' == '1' and '$servers_update' == '1' and server_num.empty? then
                  #新代理且设置默认策略组时加入指定策略组
                  new_provider_groups = %x{uci get openclash.config.new_servers_group}.chomp.split(\"'\").map { |x| x.strip }.reject { |x| x.empty? };
                  new_provider_groups.each do |x|
                     uci_commands << uci_add + 'groups=\"^' + x + '$\"'
                  end
               elsif '$servers_if_update' != '1' then
                  threads_gr = [];
                  cmd = uci_del + 'groups >/dev/null 2>&1';
                  system(cmd);
                  Value['proxy-groups'].each{
                  |z|
                     threads_gr << Thread.new{
                        if z.key?('proxies') then
                           z['proxies'].each{
                           |v|
                           if v == x['name'] then
                              uci_commands << uci_add + 'groups=\"^' + z['name'] + '$\"'
                              break
                           end
                           }
                        end;
                     };
                  };
                  #relay
                  cmd = uci_del + 'relay_groups >/dev/null 2>&1';
                  system(cmd);
                  Value['proxy-groups'].each{
                  |z|
                     threads_gr << Thread.new{
                        if z['type'] == 'relay' and z.key?('proxies') then
                           z['proxies'].each{
                           |u|
                           if u == x['name'] then
                              uci_commands << uci_add + 'relay_groups=\"' + z['name'] + '#relay#' + z['proxies'].index(x['name']) + '\"'
                              break
                           end
                           }
                        end;
                     };
                  };
                  threads_gr.each(&:join);
               end;
            };
            threads.each(&:join);
         rescue Exception => e
            YAML.LOG('Error: Resolve Proxies Failed,【${CONFIG_NAME} - '+ x['type'] + ' - ' + x['name'] + ': ' + e.message + '】');
         end;
      };
   end;
   threads_pr.each(&:join);
   batch_size = 30;
   (0...uci_commands.length).step(batch_size) do |i|
      threads_uci << Thread.new{
         system(uci_commands[i, batch_size].join('; '));
      };
   end;
   threads_uci.each(&:join);
   system('uci -q commit openclash');
" 2>/dev/null >> $LOG_FILE

#删除订阅中已不存在的代理集
if [ "$servers_if_update" = "1" ]; then
   LOG_OUT "Deleting【$CONFIG_NAME】Proxy-providers That no Longer Exists in Subscription"
   sed -i '/#match#/d' "$match_provider" 2>/dev/null
   cat $match_provider 2>/dev/null|awk -F '.' '{print $1}' |sort -rn |while read line
   do
   if [ -z "$line" ]; then
         continue
      fi
      if [ "$(uci get openclash.@proxy-provider["$line"].manual)" = "0" ] && [ "$(uci get openclash.@proxy-provider["$line"].config)" = "$CONFIG_NAME" ]; then
         uci delete openclash.@proxy-provider["$line"] 2>/dev/null
      fi
   done
fi

#删除订阅中已不存在的节点
if [ "$servers_if_update" = "1" ]; then
     LOG_OUT "Deleting【$CONFIG_NAME】Proxies That no Longer Exists in Subscription"
     sed -i '/#match#/d' "$match_servers" 2>/dev/null
     cat $match_servers |awk -F '.' '{print $1}' |sort -rn |while read -r line
     do
        if [ -z "$line" ]; then
           continue
        fi
        if [ "$(uci -q get openclash.@servers["$line"].manual)" = "0" ] && [ "$(uci -q get openclash.@servers["$line"].config)" = "$CONFIG_NAME" ]; then
           uci -q delete openclash.@servers["$line"]
        fi
     done 2>/dev/null
fi

uci -q set openclash.config.servers_if_update=0
uci -q commit openclash
LOG_OUT "Config File【$CONFIG_NAME】Read Successful!"
SLOG_CLEAN
rm -rf /tmp/match_servers.list 2>/dev/null
rm -rf /tmp/match_provider.list 2>/dev/null
del_lock
