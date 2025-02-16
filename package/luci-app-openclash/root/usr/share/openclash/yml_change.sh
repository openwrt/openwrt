#!/bin/sh
. /usr/share/openclash/ruby.sh
. /usr/share/openclash/log.sh
. /lib/functions.sh

LOG_FILE="/tmp/openclash.log"
custom_fakeip_filter=$(uci -q get openclash.config.custom_fakeip_filter || echo 0)
custom_name_policy=$(uci -q get openclash.config.custom_name_policy || echo 0)
custom_host=$(uci -q get openclash.config.custom_host || echo 0)
enable_custom_dns=$(uci -q get openclash.config.enable_custom_dns || echo 0)
append_wan_dns=$(uci -q get openclash.config.append_wan_dns || echo 0)
custom_fallback_filter=$(uci -q get openclash.config.custom_fallback_filter || echo 0)
china_ip_route=$(uci -q get openclash.config.china_ip_route); [[ "$china_ip_route" != "0" && "$china_ip_route" != "1" && "$china_ip_route" != "2" ]] && china_ip_route=0
china_ip6_route=$(uci -q get openclash.config.china_ip6_route); [[ "$china_ip6_route" != "0" && "$china_ip6_route" != "1" && "$china_ip6_route" != "2" ]] && china_ip6_route=0
enable_redirect_dns=$(uci -q get openclash.config.enable_redirect_dns || echo 1)

if [ -z "${11}" ]; then
   en_mode_tun=0
else
   en_mode_tun=${11}
fi

if [ -z "${12}" ]; then
   if [ -n "${31}" ]; then
      stack_type=${31}
   else
      stack_type="system"
   fi
else
   stack_type=${12}
fi

#Use filter to generate cn domain router pass list
if [ "$1" = "fake-ip" ] && [ "$enable_redirect_dns" != "2" ]; then
   if [ "$china_ip_route" != "0" ]; then
      for i in `awk '!/^$/&&!/^#/&&/([0-9a-zA-Z-]{1,}\.)+([a-zA-Z]{2,})/{printf("%s\n",$0)}' /etc/openclash/custom/openclash_custom_chnroute_pass.list`
      do
         if [[ ${i:0:2} -eq "+." ]] || [[ ${i:0:2} -eq "*." ]] || [[ ${i:0:1} -eq "." ]]; then
            echo "$i" >> /tmp/yaml_openclash_fake_filter_include
         else
            echo "+.$i" >> /tmp/yaml_openclash_fake_filter_include
         fi
      done 2>/dev/null
   fi
   if [ "$china_ip6_route" != "0" ]; then
      for i in `awk '!/^$/&&!/^#/&&/([0-9a-zA-Z-]{1,}\.)+([a-zA-Z]{2,})/{printf("%s\n",$0)}' /etc/openclash/custom/openclash_custom_chnroute6_pass.list`
      do
         if [[ ${i:0:2} -eq "+." ]] || [[ ${i:0:2} -eq "*." ]] || [[ ${i:0:1} -eq "." ]]; then
            echo "$i" >> /tmp/yaml_openclash_fake_filter_include
         else
            echo "+.$i" >> /tmp/yaml_openclash_fake_filter_include
         fi
      done 2>/dev/null
   fi
fi

#获取认证信息
yml_auth_get()
{
   local section="$1"
   local enabled username password
   config_get_bool "enabled" "$section" "enabled" "1"
   config_get "username" "$section" "username" ""
   config_get "password" "$section" "password" ""

   if [ "$enabled" = "0" ]; then
      return
   fi

   if [ -z "$username" ] || [ -z "$password" ]; then
      return
   else
      LOG_OUT "Tip: You have seted the authentication of SOCKS5/HTTP(S) proxy with【$username:$password】..."
      echo "  - $username:$password" >>/tmp/yaml_openclash_auth
   fi
}

#添加自定义DNS设置
yml_dns_custom()
{
   if [ "$1" = 1 ] || [ "$3" = 1 ]; then
   	sys_dns_append "$3" "$4"
      config_load "openclash"
      config_foreach yml_dns_get "dns_servers" "$2"
   fi
}

#获取DHCP或接口的DNS并追加
sys_dns_append()
{
   if [ "$1" = 1 ]; then
      wan_dns=$(/usr/share/openclash/openclash_get_network.lua "dns")
      wan6_dns=$(/usr/share/openclash/openclash_get_network.lua "dns6")
      wan_gate=$(/usr/share/openclash/openclash_get_network.lua "gateway")
      wan6_gate=$(/usr/share/openclash/openclash_get_network.lua "gateway6")
      dhcp_iface=$(/usr/share/openclash/openclash_get_network.lua "dhcp")
      pppoe_iface=$(/usr/share/openclash/openclash_get_network.lua "pppoe")
      if [ -z "$dhcp_iface" ] && [ -z "$pppoe_iface" ]; then
         if [ -n "$wan_dns" ]; then
            for i in $wan_dns; do
               echo "    - \"$i\"" >>/tmp/yaml_config.namedns.yaml
            done
         fi
         if [ -n "$wan6_dns" ] && [ "$2" = 1 ]; then
            for i in $wan6_dns; do
               echo "    - \"[${i}]:53\"" >>/tmp/yaml_config.namedns.yaml
            done
         fi
         if [ -n "$wan_gate" ]; then
            for i in $wan_gate; do
                echo "    - \"$i\"" >>/tmp/yaml_config.namedns.yaml
            done
         fi
         if [ -n "$wan6_gate" ] && [ "$2" = 1 ]; then
            for i in $wan6_gate; do
               echo "    - \"[${i}]:53\"" >>/tmp/yaml_config.namedns.yaml
            done
         fi
      else
         if [ -n "$dhcp_iface" ]; then
            for i in $dhcp_iface; do
               echo "    - dhcp://\"$i\"" >>/tmp/yaml_config.namedns.yaml
            done
            if [ -n "$wan_gate" ]; then
               for i in $wan_gate; do
                   echo "    - \"$i\"" >>/tmp/yaml_config.namedns.yaml
               done
            fi
            if [ -n "$wan6_gate" ] && [ "$2" = 1 ]; then
               for i in $wan6_gate; do
                  echo "    - \"[${i}]:53\"" >>/tmp/yaml_config.namedns.yaml
               done
            fi
         fi
         if [ -n "$pppoe_iface" ]; then
            if [ -n "$wan_dns" ]; then
		           for i in $wan_dns; do
		              echo "    - \"$i\"" >>/tmp/yaml_config.namedns.yaml
		           done
		       fi
		       if [ -n "$wan6_dns" ] && [ "$2" = 1 ]; then
		          for i in $wan6_dns; do
		             echo "    - \"[${i}]:53\"" >>/tmp/yaml_config.namedns.yaml
		          done
		       fi
         fi
      fi
      if [ -f "/tmp/yaml_config.namedns.yaml" ] && [ -z "$(grep "^ \{0,\}nameserver:$" /tmp/yaml_config.namedns.yaml 2>/dev/null)" ]; then
         sed -i '1i\  nameserver:'  "/tmp/yaml_config.namedns.yaml"
      fi
   fi
}

#获取自定义DNS设置
yml_dns_get()
{
   local section="$1" regex='^([0-9a-fA-F]{0,4}:){1,7}[0-9a-fA-F]{0,4}$'
   local enabled port type ip group dns_type dns_address interface specific_group node_resolve http3 ecs_subnet ecs_override
   config_get_bool "enabled" "$section" "enabled" "1"
   config_get "port" "$section" "port" ""
   config_get "type" "$section" "type" ""
   config_get "ip" "$section" "ip" ""
   config_get "group" "$section" "group" ""
   config_get "interface" "$section" "interface" ""
   config_get "specific_group" "$section" "specific_group" ""
   config_get_bool "node_resolve" "$section" "node_resolve" "0"
   config_get_bool "direct_nameserver" "$section" "direct_nameserver" "0"
   config_get_bool "http3" "$section" "http3" "0"
   config_get_bool "skip_cert_verify" "$section" "skip_cert_verify" "0"
   config_get_bool "ecs_override" "$section" "ecs_override" "0"
   config_get "ecs_subnet" "$section" "ecs_subnet" ""

   if [ "$enabled" = "0" ]; then
      return
   fi

   if [ -z "$ip" ]; then
      return
   fi
   
   if [[ "$ip" =~ "$regex" ]]; then
      ip="[${ip}]"
   fi

   if [ "$type" = "tcp" ]; then
      dns_type="tcp://"
   elif [ "$type" = "tls" ]; then
      dns_type="tls://"
   elif [ "$type" = "udp" ]; then
      dns_type=""
   elif [ "$type" = "https" ]; then
      dns_type="https://"
   elif [ "$type" = "quic" ]; then
      dns_type="quic://"
   fi

   if [ -n "$port" ] && [ -n "$ip" ]; then
      if [ ${ip%%/*} != ${ip#*/} ]; then
         dns_address="${ip%%/*}:$port/${ip#*/}"
      else
         dns_address="$ip:$port"
      fi
   elif [ -z "$port" ] && [ -n "$ip" ]; then
      dns_address="$ip"
   else
      return
   fi

   if [ "$specific_group" != "Disable" ] && [ -n "$specific_group" ]; then
      group_check=$(ruby -ryaml -rYAML -I "/usr/share/openclash" -E UTF-8 -e "
      begin
         Value = YAML.load_file('$2');
         Value['proxy-groups'].each{
            |x|
            if x['name'] =~ /${specific_group}/ then
               puts x['name'];
               break;
            end;
         };
      rescue Exception => e
         puts 'return';
      end;" 2>/dev/null)

      if [ "$group_check" != "return" ] && [ -n "$group_check" ]; then
         specific_group="#$group_check"
      else
         specific_group=""
      fi
   else
      specific_group=""
   fi

   if [ "$interface" != "Disable" ] && [ -n "$interface" ]; then
      if [ -n "$specific_group" ]; then
         interface="&$interface"
      else
         interface="#$interface"
      fi
   else
      interface=""
   fi

   if [ "$http3" = "1" ]; then
      if [ -n "$specific_group" ] || [ -n "$interface" ]; then
         http3="&h3=true"
      else
         http3="#h3=true"
      fi
   else
      http3=""
   fi

   if [ "$skip_cert_verify" = "1" ]; then
      if [ -n "$specific_group" ] || [ -n "$interface" ] || [ -n "$http3" ]; then
         skip_cert_verify="&skip-cert-verify=true"
      else
         skip_cert_verify="#skip-cert-verify=true"
      fi
   else
      skip_cert_verify=""
   fi

   if [ -n "$ecs_subnet" ]; then
      if [ -n "$specific_group" ] || [ -n "$interface" ] || [ -n "$http3" ] || [ -n "$skip_cert_verify" ]; then
         ecs_subnet="&ecs=$ecs_subnet"
      else
         ecs_subnet="#ecs=$ecs_subnet"
      fi
      if [ "$ecs_override" = "1" ]; then
         ecs_override="&ecs-override=true"
      else
         ecs_override=""
      fi
   else
      ecs_subnet=""
      ecs_override=""
   fi

   if [ "$node_resolve" = "1" ]; then
      if [ -z "$(grep "^ \{0,\}proxy-server-nameserver:$" /tmp/yaml_config.proxynamedns.yaml 2>/dev/null)" ]; then
         echo "  proxy-server-nameserver:" >/tmp/yaml_config.proxynamedns.yaml
      fi
      echo "    - \"$dns_type$dns_address$specific_group$interface$http3$skip_cert_verify$ecs_subnet$ecs_override\"" >>/tmp/yaml_config.proxynamedns.yaml
   fi

   if [ "$direct_nameserver" = "1" ]; then
      if [ -z "$(grep "^ \{0,\}direct-nameserver:$" /tmp/yaml_config.directnamedns.yaml 2>/dev/null)" ]; then
         echo "  direct-nameserver:" >/tmp/yaml_config.directnamedns.yaml
      fi
      echo "    - \"$dns_type$dns_address$specific_group$interface$http3$skip_cert_verify$ecs_subnet$ecs_override\"" >>/tmp/yaml_config.directnamedns.yaml
   fi

   dns_address="$dns_address$specific_group$interface$http3$skip_cert_verify$ecs_subnet$ecs_override"

   if [ -n "$group" ]; then
      if [ "$group" = "nameserver" ]; then
         if [ -z "$(grep "^ \{0,\}nameserver:$" /tmp/yaml_config.namedns.yaml 2>/dev/null)" ]; then
            echo "  nameserver:" >/tmp/yaml_config.namedns.yaml
         fi
         echo "    - \"$dns_type$dns_address\"" >>/tmp/yaml_config.namedns.yaml
      elif [ "$group" = "fallback" ]; then
         if [ -z "$(grep "^ \{0,\}fallback:$" /tmp/yaml_config.falldns.yaml 2>/dev/null)" ]; then
            echo "  fallback:" >/tmp/yaml_config.falldns.yaml
         fi
         echo "    - \"$dns_type$dns_address\"" >>/tmp/yaml_config.falldns.yaml
      elif [ "$group" = "default" ]; then
         if [ -z "$(grep "^ \{0,\}default-nameserver:$" /tmp/yaml_config.defaultdns.yaml 2>/dev/null)" ]; then
            echo "  default-nameserver:" >/tmp/yaml_config.defaultdns.yaml
         fi
         echo "    - \"$dns_type$dns_address\"" >>/tmp/yaml_config.defaultdns.yaml
      fi
   else
      return
   fi
}

config_load "openclash"
config_foreach yml_auth_get "authentication"
yml_dns_custom "$enable_custom_dns" "$5" "$append_wan_dns" "${16}"

#配置文件覆写部分
ruby -ryaml -rYAML -I "/usr/share/openclash" -E UTF-8 -e "
begin
   Value = YAML.load_file('$5');
rescue Exception => e
   YAML.LOG('Error: Load File Failed,【' + e.message + '】');
end;

threads = [];

#General
threads << Thread.new {
   begin
      Value['redir-port']=$4;
      Value['tproxy-port']=${15};
      Value['port']=$7;
      Value['socks-port']=$8;
      Value['mixed-port']=${14};
      Value['mode']='${10}';
      if '$9' != '0' then
         Value['log-level']='$9';
      end;
      Value['allow-lan']=true;
      Value['external-controller']='0.0.0.0:$3';
      Value['secret']='$2';
      Value['bind-address']='*';
      Value['external-ui']='/usr/share/openclash/ui';
      Value['keep-alive-interval']=15;
      Value['keep-alive-idle']=600;
      if $6 == 1 then
         Value['ipv6']=true;
      else
         Value['ipv6']=false;
      end;
      if '${22}' != '0' then
         Value['interface-name']='${22}';
      end;

      if ${19} == 1 then
         Value['geodata-mode']=true;
      end;
      if '${20}' != '0' then
         Value['geodata-loader']='${20}';
      end;
      if ${23} == 1 then
         Value['tcp-concurrent']=true;
      end;
      if ${32} == 1 then
         Value['unified-delay']=true;
      end;
      if '${27}' != '0' then
         Value['find-process-mode']='${27}';
      end;
      if '${29}' != '0' then
         Value['global-client-fingerprint']='${29}';
      end;
      if ${36} == 1 then
         if Value.key?('experimental') then
            Value['experimental']['quic-go-disable-gso']=true;
         else
            Value['experimental']={'quic-go-disable-gso'=>true};
         end;
      end;

      if ${16} == 1 then
         Value['dns']['ipv6']=true;
         Value['ipv6']=true;
      else
         Value['dns']['ipv6']=false;
      end;
      
      if '$1' == 'redir-host' then
         Value['dns']['enhanced-mode']='redir-host';
         Value['dns'].delete('fake-ip-range');
      else
         Value['dns']['enhanced-mode']='fake-ip';
         Value['dns']['fake-ip-range']='${28}';
      end;

      Value['dns']['listen']='0.0.0.0:${13}';
      
      #meta only
      if ${33} == 1 then
         Value['dns']['respect-rules']=true;
      end;
      
      if ${18} == 1 then
         Value_sniffer={'sniffer'=>{'enable'=>true, 'override-destination'=>true, 'sniff'=>{'QUIC'=>{'ports'=>[443]}, 'TLS'=>{'ports'=>[443, 8443]}, 'HTTP'=>{'ports'=>[80, '8080-8880'], 'override-destination'=>true}}, 'force-domain'=>['+.netflix.com', '+.nflxvideo.net', '+.amazonaws.com', '+.media.dssott.com'], 'skip-domain'=>['+.apple.com', 'Mijia Cloud', 'dlg.io.mi.com', '+.oray.com', '+.sunlogin.net', '+.push.apple.com']}};
         Value['sniffer']=Value_sniffer['sniffer'];
         if '$1' == 'redir-host' then
            Value['sniffer']['force-dns-mapping']=true;
         end;
         if ${26} == 1 then
            Value['sniffer']['parse-pure-ip']=true;
         end;
         if File::exist?('/etc/openclash/custom/openclash_custom_sniffer.yaml') then
            if ${21} == 1 then
               Value_7 = YAML.load_file('/etc/openclash/custom/openclash_custom_sniffer.yaml');
               if Value_7 != false and not Value_7['sniffer'].to_a.empty? then
                  Value['sniffer'].merge!(Value_7['sniffer']);
               end;
            end;
         end;
      end;
      Value_2={'tun'=>{'enable'=>true}};
      if $en_mode_tun != 0 or ${30} == 2 or ${30} == 3 then
         Value['tun']=Value_2['tun'];
         Value['tun']['stack']='$stack_type';
         Value['tun']['device']='utun';
         Value_2={'dns-hijack'=>['127.0.0.1:53']};
         Value['tun'].merge!(Value_2);
         Value['tun']['endpoint-independent-nat']=true;
         Value['tun']['auto-route']=false;
         Value['tun']['auto-detect-interface']=false;
         Value['tun']['auto-redirect']=false;
         Value['tun']['strict-route']=false;
         Value['tun'].delete_if{|x,y| x=='iproute2-table-index'};
      else
         if Value.key?('tun') then
            Value.delete('tun');
         end;
      end;
      if Value.key?('iptables') then
         Value.delete('iptables');
      end;
      if not Value.key?('profile') or Value['profile'].nil? then
         Value_3={'profile'=>{'store-selected'=>true}};
         Value['profile']=Value_3['profile'];
      else
         Value['profile']['store-selected']=true;
      end;
      if ${17} == 1 then
         Value['profile']['store-fake-ip']=true;
      end;

      if Value.key?('ebpf') then
         Value.delete('ebpf');
      end;

      if '${35}' == '0' then
         Value['routing-mark']=6666;
      else
         if Value.key?('routing-mark') then
            Value.delete('routing-mark');
         end;
      end;
      if Value.key?('auto-redir') then
         Value.delete('auto-redir');
      end;
   rescue Exception => e
      YAML.LOG('Error: Set General Failed,【' + e.message + '】');
   end;
};

#Custom dns
threads << Thread.new {
   begin
      if $enable_custom_dns == 1 or $append_wan_dns == 1 then
         if File::exist?('/tmp/yaml_config.namedns.yaml') then
            Value_1 = YAML.load_file('/tmp/yaml_config.namedns.yaml');
            if $enable_custom_dns == 1 then
               Value['dns']['nameserver'] = Value_1['nameserver'].uniq;
            elsif $append_wan_dns == 1 then
               if Value['dns'].has_key?('nameserver') then
                  Value['dns']['nameserver'] = Value['dns']['nameserver'] | Value_1['nameserver'];
               else
                  Value['dns']['nameserver'] = Value_1['nameserver'].uniq;
               end;
            end;
            if File::exist?('/tmp/yaml_config.falldns.yaml') and $enable_custom_dns == 1 then
               Value_2 = YAML.load_file('/tmp/yaml_config.falldns.yaml');
               Value['dns']['fallback'] = Value_2['fallback'].uniq;
            end;
         elsif $enable_custom_dns == 1 then
            YAML.LOG('Error: Nameserver Option Must Be Setted, Stop Customing DNS Servers');
         end;
      end;
   rescue Exception => e
      YAML.LOG('Error: Set Custom DNS Failed,【' + e.message + '】');
   end;

   #default-nameserver
   begin
      if $enable_custom_dns == 1 then
         if File::exist?('/tmp/yaml_config.defaultdns.yaml') then
            Value_1 = YAML.load_file('/tmp/yaml_config.defaultdns.yaml');
            if Value['dns'].has_key?('default-nameserver') then
               Value['dns']['default-nameserver'] = Value['dns']['default-nameserver'] | Value_1['default-nameserver'];
            else
               Value['dns']['default-nameserver'] = Value_1['default-nameserver'].uniq;
            end;
         end;
      end;
      if ${25} == 1 then
         reg = /^dhcp:\/\/|^system($|:\/\/)|([0-9a-zA-Z-]{1,}\.)+([a-zA-Z]{2,})/;
         if Value['dns'].has_key?('fallback') then
            Value_1=Value['dns']['nameserver'] | Value['dns']['fallback'];
         else
            Value_1=Value['dns']['nameserver'];
         end;
         Value_1.each{|x|
            if not x =~ reg then
               if Value['dns'].has_key?('default-nameserver') then
                  Value['dns']['default-nameserver']=Value['dns']['default-nameserver'].to_a.insert(-1,x).uniq;
               else
                  Value_2={'default-nameserver'=>[x]};
                  Value['dns'].merge!(Value_2);
               end;
            end;
         };
      end;
   rescue Exception => e
      YAML.LOG('Error: Set default-nameserver Failed,【' + e.message + '】');
   end;

   #fallback-filter
   begin
      if $custom_fallback_filter == 1 then
         if not Value['dns'].key?('fallback') then
            YAML.LOG('Error: Fallback-Filter Need fallback of DNS Been Setted, Ignore...');
         elsif not YAML.load_file('/etc/openclash/custom/openclash_custom_fallback_filter.yaml') then
            YAML.LOG('Error: Unable To Parse Custom Fallback-Filter File, Ignore...');
         else
            Value['dns']['fallback-filter'] = YAML.load_file('/etc/openclash/custom/openclash_custom_fallback_filter.yaml')['fallback-filter'];
         end;
      end;
   rescue Exception => e
      YAML.LOG('Error: Set fallback-filter Failed,【' + e.message + '】');
   end;
};

#proxy-server-nameserver
threads << Thread.new {
   begin
      if $enable_custom_dns == 1 then
         if File::exist?('/tmp/yaml_config.proxynamedns.yaml') then
            Value_1 = YAML.load_file('/tmp/yaml_config.proxynamedns.yaml');
            if Value['dns'].has_key?('proxy-server-nameserver') then
               Value['dns']['proxy-server-nameserver'] = Value['dns']['proxy-server-nameserver'] | Value_1['proxy-server-nameserver'];
            else
               Value['dns']['proxy-server-nameserver'] = Value_1['proxy-server-nameserver'].uniq;
            end;
         end;
      end;
   rescue Exception => e
      YAML.LOG('Error: Set proxy-server-nameserver Failed,【' + e.message + '】');
   end;
};

#direct-nameserver
threads << Thread.new {
   begin
      if $enable_custom_dns == 1 then
         if File::exist?('/tmp/yaml_config.directnamedns.yaml') then
            Value_1 = YAML.load_file('/tmp/yaml_config.directnamedns.yaml');
            if Value['dns'].has_key?('direct-nameserver') then
               Value['dns']['direct-nameserver'] = Value['dns']['direct-nameserver'] | Value_1['direct-nameserver'];
            else
               Value['dns']['direct-nameserver'] = Value_1['direct-nameserver'].uniq;
            end;
         end;
      end;
   rescue Exception => e
      YAML.LOG('Error: Set direct-nameserver Failed,【' + e.message + '】');
   end;
};

#nameserver-policy
threads << Thread.new {
   begin
      if $custom_name_policy == 1 then
         if File::exist?('/etc/openclash/custom/openclash_custom_domain_dns_policy.list') then
            Value_6 = YAML.load_file('/etc/openclash/custom/openclash_custom_domain_dns_policy.list');
            if Value_6 != false and not Value_6.nil? then
               if Value['dns'].has_key?('nameserver-policy') and not Value['dns']['nameserver-policy'].to_a.empty? then
                  Value['dns']['nameserver-policy'].merge!(Value_6);
               else
                  Value['dns']['nameserver-policy']=Value_6;
               end;
               Value['dns']['nameserver-policy'].uniq;
            end;
         end;
      end;
   rescue Exception => e
      YAML.LOG('Error: Set Nameserver-Policy Failed,【' + e.message + '】');
   end;
};

#fake-ip-filter
threads << Thread.new {
   begin
      if $custom_fakeip_filter == 1 then
         if '${34}' == 'whitelist' then
            Value['dns']['fake-ip-filter-mode']='whitelist';
         else
            Value['dns']['fake-ip-filter-mode']='blacklist';
         end;
         if '$1' == 'fake-ip' then
            if File::exist?('/etc/openclash/custom/openclash_custom_fake_filter.list') then
               Value_4 = IO.readlines('/etc/openclash/custom/openclash_custom_fake_filter.list');
               if not Value_4.empty? then
                  Value_4 = Value_4.map!{|x| x.gsub(/#.*$/,'').strip} - ['', nil];
                  if Value['dns'].has_key?('fake-ip-filter') and not Value['dns']['fake-ip-filter'].to_a.empty? then
                     Value['dns']['fake-ip-filter'] = Value['dns']['fake-ip-filter'] | Value_4;
                  else
                     Value['dns']['fake-ip-filter'] = Value_4;
                  end;
               end;
            end;
            if File::exist?('/tmp/yaml_openclash_fake_filter_include') then
               Value_4 = IO.readlines('/tmp/yaml_openclash_fake_filter_include');
               if not Value_4.empty? then
                  Value_4 = Value_4.map!{|x| x.gsub(/#.*$/,'').strip} - ['', nil];
                  if Value['dns'].has_key?('fake-ip-filter') and not Value['dns']['fake-ip-filter'].to_a.empty? then
                     Value['dns']['fake-ip-filter'] = Value['dns']['fake-ip-filter'] | Value_4;
                  else
                     Value['dns']['fake-ip-filter'] = Value_4;
                  end;
               end;
            end;
         end;
      end;
      if '$1' == 'fake-ip' then
         if '$china_ip_route' != '0' or '$china_ip6_route' != '0' then
            if Value['dns']['fake-ip-filter-mode'] == 'blacklist' or not Value['dns'].has_key?('fake-ip-filter-mode') then
               if Value['dns'].has_key?('fake-ip-filter') and not Value['dns']['fake-ip-filter'].to_a.empty? then
                  Value['dns']['fake-ip-filter'].insert(-1,'geosite:cn');
                  Value['dns']['fake-ip-filter']=Value['dns']['fake-ip-filter'].uniq;
               else
                  Value['dns'].merge!({'fake-ip-filter'=>['geosite:cn']});
               end;
               YAML.LOG('Tip: Because Need Ensure Bypassing IP Option Work, Added The Fake-IP-Filter Rule【geosite:cn】...');
            else
               if Value['dns'].has_key?('fake-ip-filter') and not Value['dns']['fake-ip-filter'].to_a.empty? then
                  Value['dns']['fake-ip-filter'].each{|x|
                     if x =~ /(geosite:?).*(@cn|:cn|,cn|:china)/ then
                        Value['dns']['fake-ip-filter'].delete(x);
                        YAML.LOG('Tip: Because Need Ensure Bypassing IP Option Work, Deleted The Fake-IP-Filter Rule【' + x + '】...');
                     end;
                  };
               end;
            end;
         end;
      end;
   rescue Exception => e
      YAML.LOG('Error: Set Fake-IP-Filter Failed,【' + e.message + '】');
   end;
};

#custom hosts
threads << Thread.new {
   begin
      if $custom_host == 1 then
         if File::exist?('/etc/openclash/custom/openclash_custom_hosts.list') then
            begin
               Value_3 = YAML.load_file('/etc/openclash/custom/openclash_custom_hosts.list');
               if Value_3 != false and not Value_3.nil? then
                  Value['dns']['use-hosts']=true;
                  if Value.has_key?('hosts') and not Value['hosts'].to_a.empty? then
                     Value['hosts'].merge!(Value_3);
                  else
                     Value['hosts']=Value_3;
                  end;
                  Value['hosts'].uniq;
                  YAML.LOG('Warning: You May Need to Turn off The Rebinding Protection Option of Dnsmasq When Hosts Has Set a Reserved Address...');
               end;
            rescue
               Value_3 = IO.readlines('/etc/openclash/custom/openclash_custom_hosts.list');
               if not Value_3.empty? then
                  Value_3 = Value_3.map!{|x| x.gsub(/#.*$/,'').strip} - ['', nil];
                  Value['dns']['use-hosts']=true;
                  if Value.has_key?('hosts') and not Value['hosts'].to_a.empty? then
                     Value_3.each{|x| Value['hosts'].merge!(x)};
                  else
                     Value_3.each{|x| Value['hosts'].merge!(x)};
                  end;
                  Value['hosts'].uniq;
                  YAML.LOG('Warning: You May Need to Turn off The Rebinding Protection Option of Dnsmasq When Hosts Has Set a Reserved Address...');
               end;
            end;
         end;
      end;
   rescue Exception => e
      YAML.LOG('Error: Set Hosts Rules Failed,【' + e.message + '】');
   end;
};

#auth
threads << Thread.new {
   begin
      if File::exist?('/tmp/yaml_openclash_auth') then
         Value_1 = YAML.load_file('/tmp/yaml_openclash_auth');
         if Value.has_key?('authentication') and not Value['authentication'].to_a.empty? then
            Value['authentication'] = Value['authentication'] | Value_1;
         else
            Value['authentication']=Value_1;
         end;
      end;
   rescue Exception => e
      YAML.LOG('Error: Set authentication Failed,【' + e.message + '】');
   end;
};

begin
   if not Value.key?('dns') or Value['dns'].nil? then
      Value_1={'dns'=>{'enable'=>true}};
      Value['dns']=Value_1['dns'];
   else
      Value['dns']['enable']=true;
   end;

   threads.each(&:join);
   
   #dns check
   if $enable_redirect_dns != 2 then
      threads.clear;
      dns_option = ['nameserver','fallback','default-nameserver','proxy-server-nameserver','nameserver-policy','direct-nameserver'];
      dns_option.each do |x|
         threads << Thread.new {
            begin
               if not Value['dns'].key?(x) or Value['dns'][x].nil? then
                  next;
               end;
               if x != 'nameserver-policy' then
                  Value['dns'][x].flatten.each do |v|
                     if v =~ /^system($|:\/\/$)/ then
                        Value['dns'][x].delete(v);
                        YAML.LOG('Tip: Option【' + x + '】is Setted【'+ v +'】as DNS Server Which May Cause DNS Loop, Already Remove It...');
                     end;
                  end;
                  if Value['dns'][x].to_a.grep(/^system($|:\/\/)/).empty? then
                     next;
                  end;
               else
                  Value['dns'][x].each do |k,v|
                     if v.class == Array then
                        v.flatten.each do |z|
                           if z =~ /^system($|:\/\/$)/ then
                              if v.length > 1 then
                                 v.delete(z);
                              else
                                 Value['dns'][x].delete(k);
                              end;
                              YAML.LOG('Tip: Option【' + x + ' - ' + k + '】is Setted【'+ z +'】as DNS Server Which May Cause DNS Loop, Already Remove It...');
                           end;
                        end;
                     else
                        if v =~ /^system($|:\/\/$)/ then
                           Value['dns'][x].delete(k);
                           YAML.LOG('Tip: Option【' + x + ' - ' + k + '】is Setted【'+ v +'】as DNS Server Which May Cause DNS Loop, Already Remove It...');
                        end;
                     end;
                  end;
                  if Value['dns'][x].values.flatten.grep(/^system($|:\/\/)/).empty? then
                     next;
                  end;
               end;
               YAML.LOG('Warning: Option【' + x + '】is Setted【system】as DNS Server Which May Cause DNS Loop, Please Consider Removing It When DNS Works Abnormally...');
            end;
         };
      end;
      threads.each(&:join);
   end;
   if not Value['dns'].key?('nameserver') or Value['dns']['nameserver'].to_a.empty? then
      YAML.LOG('Tip: Detected That The nameserver DNS Option Has No Server Set, Starting To Complete...');
      Value_1={'nameserver'=>['114.114.114.114','119.29.29.29','8.8.8.8','1.1.1.1']};
      Value_2={'fallback'=>['https://dns.cloudflare.com/dns-query','https://dns.google/dns-query']};
      Value['dns'].merge!(Value_1);
      Value['dns'].merge!(Value_2);
   end;
   if ${33} == 1 or Value['dns']['respect-rules'].to_s == 'true' then
      if not Value['dns'].has_key?('proxy-server-nameserver') or Value['dns']['proxy-server-nameserver'].to_a.empty? then
         Value['dns'].merge!({'proxy-server-nameserver'=>['114.114.114.114','119.29.29.29','8.8.8.8','1.1.1.1']});
         YAML.LOG('Tip: Respect-rules Option Need Proxy-server-nameserver Option Must Be Setted, Auto Set to【114.114.114.114, 119.29.29.29, 8.8.8.8, 1.1.1.1】');
      end;
   end;
rescue Exception => e
      YAML.LOG('Error: Config File Overwrite Failed,【' + e.message + '】');
ensure
   File.open('$5','w') {|f| YAML.dump(Value, f)};
end" 2>/dev/null >> $LOG_FILE