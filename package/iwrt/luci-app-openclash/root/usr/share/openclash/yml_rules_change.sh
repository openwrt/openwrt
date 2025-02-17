#!/bin/sh
. /lib/functions.sh
. /usr/share/openclash/ruby.sh
. /usr/share/openclash/log.sh

LOG_FILE="/tmp/openclash.log"
RULE_PROVIDER_FILE="/tmp/yaml_rule_provider.yaml"
GAME_RULE_FILE="/tmp/yaml_game_rule.yaml"
github_address_mod=$(uci -q get openclash.config.github_address_mod || echo 0)
urltest_address_mod=$(uci -q get openclash.config.urltest_address_mod || echo 0)
tolerance=$(uci -q get openclash.config.tolerance || echo 0)
urltest_interval_mod=$(uci -q get openclash.config.urltest_interval_mod || echo 0)
CONFIG_NAME="$5"
rule_name=""
SKIP_CUSTOM_OTHER_RULES=0

#处理自定义规则集
yml_set_custom_rule_provider()
{
   local section="$1"
   local enabled name config type behavior path url interval group position other_parameters
   config_get_bool "enabled" "$section" "enabled" "1"
   config_get "name" "$section" "name" ""
   config_get "config" "$section" "config" ""
   config_get "type" "$section" "type" ""
   config_get "behavior" "$section" "behavior" ""
   config_get "path" "$section" "path" ""
   config_get "url" "$section" "url" ""
   config_get "interval" "$section" "interval" ""
   config_get "group" "$section" "group" ""
   config_get "position" "$section" "position" ""
   config_get "format" "$section" "format" ""
   config_get "other_parameters" "$section" "other_parameters" ""

   if [ "$enabled" = "0" ]; then
      return
   fi

   if [ -n "$(grep "$url" "$RULE_PROVIDER_FILE" 2>/dev/null)" ] && [ -n "$url" ]; then
      return
   fi
   
   if [ -n "$config" ] && [ "$config" != "$CONFIG_NAME" ] && [ "$config" != "all" ]; then
      return
   fi
   
   if [ -z "$name" ] || [ -z "$type" ] || [ -z "$behavior" ]; then
      return
   fi

   if [ "$type" = "http" ] && [ -z "$url" ]; then
      return
   fi

   if [ -z "$format" ]; then
      format="yaml"
   fi

   if [ -z "$(echo "$path" |grep "./rule_provider/" 2>/dev/null)" ] && [ "$type" = "http" ]; then
      if [ "$format" == "text" ]; then
         path="./rule_provider/$name"
      elif [ "$format" == "mrs" ]; then
         path="./rule_provider/$name.mrs"
      else
         path="./rule_provider/$name.yaml"
      fi
   elif [ -z "$path" ] && [ "$type" != "inline" ]; then
      return
   fi

   if [ -n "$(grep "$path" "$RULE_PROVIDER_FILE" 2>/dev/null)" ]; then
      return
   fi

   if [ -z "$interval" ] && [ "$type" = "http" ]; then
      interval=86400
   fi

cat >> "$RULE_PROVIDER_FILE" <<-EOF
  $name:
    type: $type
    behavior: $behavior
EOF
    if [ -n "$path" ]; then
cat >> "$RULE_PROVIDER_FILE" <<-EOF
    path: $path
EOF
    fi
    if [ -n "$format" ]; then
cat >> "$RULE_PROVIDER_FILE" <<-EOF
    format: $format
EOF
    fi
    if [ "$type" = "http" ]; then
cat >> "$RULE_PROVIDER_FILE" <<-EOF
    url: $url
    interval: $interval
EOF
    fi

   #other_parameters
   if [ -n "$other_parameters" ]; then
      echo -e "$other_parameters" >> "$RULE_PROVIDER_FILE"
   fi

   yml_rule_set_add "$name" "$group" "$position"
}

yml_rule_set_add()
{
   if [ -z "$3" ]; then
      return
   fi

   if [ "$3" == "1" ]; then
      if [ -z "$(grep "^ \{0,\}rules:$" /tmp/yaml_rule_set_bottom_custom.yaml 2>/dev/null)" ]; then
         echo "rules:" > "/tmp/yaml_rule_set_bottom_custom.yaml"
      fi
      echo "- RULE-SET,${1},${2}" >> "/tmp/yaml_rule_set_bottom_custom.yaml"
   else
      if [ -z "$(grep "^ \{0,\}rules:$" /tmp/yaml_rule_set_top_custom.yaml 2>/dev/null)" ]; then
         echo "rules:" > "/tmp/yaml_rule_set_top_custom.yaml"
      fi
      echo "- RULE-SET,${1},${2}" >> "/tmp/yaml_rule_set_top_custom.yaml"
   fi
}

yml_gen_rule_provider_file()
{
   if [ -z "$1" ]; then
      return
   fi
   
   RULE_PROVIDER_FILE_NAME=$(grep "^$1," /usr/share/openclash/res/rule_providers.list |awk -F ',' '{print $6}' 2>/dev/null)
   if [ -z "$RULE_PROVIDER_FILE_NAME" ]; then
      RULE_PROVIDER_FILE_NAME=$(grep "^$1," /usr/share/openclash/res/rule_providers.list |awk -F ',' '{print $5}' 2>/dev/null)
   fi
   RULE_PROVIDER_FILE_BEHAVIOR=$(grep ",$RULE_PROVIDER_FILE_NAME$" /usr/share/openclash/res/rule_providers.list |awk -F ',' '{print $3}' 2>/dev/null)
   RULE_PROVIDER_FILE_PATH="/etc/openclash/rule_provider/$RULE_PROVIDER_FILE_NAME"
   RULE_PROVIDER_FILE_URL_PATH="$(grep ",$RULE_PROVIDER_FILE_NAME$" /usr/share/openclash/res/rule_providers.list |awk -F ',' '{print $4$5}' 2>/dev/null)"
   if [ "$github_address_mod" -eq 0 ]; then
      RULE_PROVIDER_FILE_URL="https://raw.githubusercontent.com/${RULE_PROVIDER_FILE_URL_PATH}"
   else
      if [ "$github_address_mod" == "https://cdn.jsdelivr.net/" ] || [ "$github_address_mod" == "https://fastly.jsdelivr.net/" ] || [ "$github_address_mod" == "https://testingcf.jsdelivr.net/" ]; then
         RULE_PROVIDER_FILE_URL="${github_address_mod}gh/"$(echo "$RULE_PROVIDER_FILE_URL_PATH" |awk -F '/master' '{print $1}' 2>/dev/null)"@master"$(echo "$RULE_PROVIDER_FILE_URL_PATH" |awk -F 'master' '{print $2}')""
      else
         RULE_PROVIDER_FILE_URL="${github_address_mod}https://raw.githubusercontent.com/${RULE_PROVIDER_FILE_URL_PATH}"
      fi
   fi
   if [ -n "$(grep "$RULE_PROVIDER_FILE_URL" $RULE_PROVIDER_FILE 2>/dev/null)" ]; then
      return
   fi

   if [ -z "$RULE_PROVIDER_FILE_NAME" ] || [ -z "$RULE_PROVIDER_FILE_BEHAVIOR" ] || [ -z "$RULE_PROVIDER_FILE_URL" ]; then
      return
   fi

cat >> "$RULE_PROVIDER_FILE" <<-EOF
  $1:
    type: http
    behavior: $RULE_PROVIDER_FILE_BEHAVIOR
    path: $RULE_PROVIDER_FILE_PATH
    url: $RULE_PROVIDER_FILE_URL
EOF
   if [ -z "$3" ]; then
cat >> "$RULE_PROVIDER_FILE" <<-EOF
    interval: 86400
EOF
   else
cat >> "$RULE_PROVIDER_FILE" <<-EOF
    interval: $3
EOF
   fi
   yml_rule_set_add "$1" "$2" "$4"
}

yml_get_rule_provider()
{
   local section="$1"
   local enabled group config interval position
   config_get_bool "enabled" "$section" "enabled" "1"
   config_get "group" "$section" "group" ""
   config_get "config" "$section" "config" ""
   config_get "interval" "$section" "interval" ""
   config_get "position" "$section" "position" ""

   if [ "$enabled" = "0" ]; then
      return
   fi
   
   if [ -n "$config" ] && [ "$config" != "$CONFIG_NAME" ] && [ "$config" != "all" ]; then
      return
   fi

   if [ -z "$group" ]; then
      return
   fi
   
   config_list_foreach "$section" "rule_name" yml_gen_rule_provider_file "$group" "$interval" "$position"
}

get_rule_file()
{
   if [ -z "$1" ]; then
      return
   fi
   
   GAME_RULE_FILE_NAME=$(grep "^$1," /usr/share/openclash/res/game_rules.list |awk -F ',' '{print $3}' 2>/dev/null)

   if [ -z "$GAME_RULE_FILE_NAME" ]; then
      GAME_RULE_FILE_NAME=$(grep "^$1," /usr/share/openclash/res/game_rules.list |awk -F ',' '{print $2}' 2>/dev/null)
   fi
   
   GAME_RULE_PATH="./game_rules/$GAME_RULE_FILE_NAME"

   yml_rule_set_add "$1" "$2" "1"

cat >> "$RULE_PROVIDER_FILE" <<-EOF
  $1:
    type: file
    behavior: ipcidr
    path: '${GAME_RULE_PATH}'
EOF
}

yml_game_rule_get()
{
   local section="$1"
   local enabled group config
   config_get_bool "enabled" "$section" "enabled" "1"
   config_get "group" "$section" "group" ""
   config_get "config" "$section" "config" ""

   if [ "$enabled" = "0" ]; then
      return
   fi
   
   if [ -n "$config" ] && [ "$config" != "$CONFIG_NAME" ] && [ "$config" != "all" ]; then
      return
   fi
   
   if [ -z "$group" ]; then
      return
   fi
   
   config_list_foreach "$section" "rule_name" get_rule_file "$group"
}

yml_rule_group_get()
{
   local section="$1"
   local enabled group config
   config_get_bool "enabled" "$section" "enabled" "1"
   config_get "group" "$section" "group" ""
   config_get "config" "$section" "config" ""

   if [ "$enabled" = "0" ]; then
      return
   fi
   
   if [ -n "$config" ] && [ "$config" != "$CONFIG_NAME" ] && [ "$config" != "all" ]; then
      return
   fi
   
   if [ -z "$group" ] || [ "$group" = "DIRECT" ] || [ "$group" = "REJECT" ]; then
      return
   fi

   group_check=$(ruby -ryaml -rYAML -I "/usr/share/openclash" -E UTF-8 -e "
   begin
      Value = YAML.load_file('$2');
      Value['proxy-groups'].each{
         |x|
         if x['name'] == '$group' then
            if (x.key?('use') and not x['use'].to_a.empty?) or (x.key?('proxies') and not x['proxies'].to_a.empty?) then
               puts 'return';
               break;
            end;
         end;
      };
   end;" 2>/dev/null)

   if [ "$group_check" != "return" ]; then
      /usr/share/openclash/yml_groups_set.sh >/dev/null 2>&1 "$group"
   fi
}

yml_other_set()
{
   config_load "openclash"
   config_foreach yml_get_rule_provider "rule_provider_config"
   config_foreach yml_set_custom_rule_provider "rule_providers"
   config_foreach yml_game_rule_get "game_config"
   #添加缺失的节点与策略组
   config_foreach yml_rule_group_get "rule_provider_config" "$3"
   config_foreach yml_rule_group_get "rule_providers" "$3"
   config_foreach yml_rule_group_get "game_config" "$3"
   ruby -ryaml -rYAML -I "/usr/share/openclash" -E UTF-8 -e "
   begin
      Value = YAML.load_file('$3');
   rescue Exception => e
      YAML.LOG('Error: Load File Failed,【' + e.message + '】');
   end;

   begin
      if '$rule_name' == 'lhie1' and $SKIP_CUSTOM_OTHER_RULES == 0 then
         Value_1 = YAML.load_file('/usr/share/openclash/res/lhie1.yaml');
         if Value.has_key?('script') then
            Value.delete('script')
         end;
         if Value.has_key?('rules') then
            Value.delete('rules')
         end;
         if Value_1.has_key?('rule-providers') and not Value_1['rule-providers'].to_a.empty? then
            if Value.has_key?('rule-providers') and not Value['rule-providers'].to_a.empty? then
               Value['rule-providers'].merge!(Value_1['rule-providers'])
            else
               Value['rule-providers']=Value_1['rule-providers']
            end
         end;
         Value['rules']=Value_1['rules'];
         Value['rules'].to_a.collect!{|x|
         x.to_s.gsub(/,[\s]?Bilibili,[\s]?CN Mainland TV$/, ',Bilibili,$Bilibili#delete_')
         .gsub(/,[\s]?Bahamut,[\s]?Asian TV$/, ',Bahamut,$Bahamut#delete_')
         .gsub(/,[\s]?Max,[\s]?Max$/, ',Max,$HBOMax#delete_')
         .gsub(/,[\s]?Discovery Plus,[\s]?Global TV$/, ',Discovery Plus,$Discovery#delete_')
         .gsub(/,[\s]?DAZN,[\s]?Global TV$/, ',DAZN,$DAZN#delete_')
         .gsub(/,[\s]?Pornhub,[\s]?Global TV$/, ',Pornhub,$Pornhub#delete_')
         .gsub(/,[\s]?Global TV$/, ',$GlobalTV#delete_')
         .gsub(/,[\s]?Asian TV$/, ',$AsianTV#delete_')
         .gsub(/,[\s]?CN Mainland TV$/, ',$MainlandTV#delete_')
         .gsub(/,[\s]?Proxy$/, ',$Proxy#delete_')
         .gsub(/,[\s]?YouTube$/, ',$Youtube#delete_')
         .gsub(/,[\s]?Apple$/, ',$Apple#delete_')
         .gsub(/,[\s]?Apple TV$/, ',$AppleTV#delete_')
         .gsub(/,[\s]?Scholar$/, ',$Scholar#delete_')
         .gsub(/,[\s]?Netflix$/, ',$Netflix#delete_')
         .gsub(/,[\s]?Disney$/, ',$Disney#delete_')
         .gsub(/,[\s]?Spotify$/, ',$Spotify#delete_')
         .gsub(/,[\s]?AI Suite$/, ',$AI_Suite#delete_')
         .gsub(/,[\s]?Steam$/, ',$Steam#delete_')
         .gsub(/,[\s]?miHoYo$/, ',$miHoYo#delete_')
         .gsub(/,[\s]?AdBlock$/, ',$AdBlock#delete_')
         .gsub(/,[\s]?HTTPDNS$/, ',$HTTPDNS#delete_')
         .gsub(/,[\s]?Speedtest$/, ',$Speedtest#delete_')
         .gsub(/,[\s]?Telegram$/, ',$Telegram#delete_')
         .gsub(/,[\s]?Crypto$/, ',$Crypto#delete_')
         .gsub(/,[\s]?Discord$/, ',$Discord#delete_')
         .gsub(/,[\s]?Microsoft$/, ',$Microsoft#delete_')
         .to_s.gsub(/,[\s]?PayPal$/, ',$PayPal#delete_')
         .gsub(/,[\s]?Domestic$/, ',$Domestic#delete_')
         .gsub(/,[\s]?Others$/, ',$Others#delete_')
         .gsub(/,[\s]?Google FCM$/, ',$GoogleFCM#delete_')
         .gsub(/#delete_/, '')
         };
      end;
   rescue Exception => e
      YAML.LOG('Error: Set lhie1 Rules Failed,【' + e.message + '】');
   end;

   t1=Thread.new{
      #BT/P2P DIRECT Rules
      begin
         if $4 == 1 then
            Value['rules']=Value['rules'].to_a.insert(0,
            'GEOSITE,category-public-tracker,DIRECT'
            );
            match_group=Value['rules'].grep(/(MATCH|FINAL)/)[0];
            if not match_group.nil? then
               common_port_group = (match_group.split(',')[-1] =~ /^no-resolve$|^src$/) ? match_group.split(',')[-2] : match_group.split(',')[-1];
               if not common_port_group.nil? then
                  ruby_add_index = Value['rules'].index(Value['rules'].grep(/(MATCH|FINAL)/).first);
                  ruby_add_index ||= -1;
                  Value['rules']=Value['rules'].to_a.insert(ruby_add_index,
                  'PROCESS-NAME,aria2c,DIRECT',
                  'PROCESS-NAME,BitComet,DIRECT',
                  'PROCESS-NAME,fdm,DIRECT',
                  'PROCESS-NAME,NetTransport,DIRECT',
                  'PROCESS-NAME,qbittorrent,DIRECT',
                  'PROCESS-NAME,Thunder,DIRECT',
                  'PROCESS-NAME,transmission-daemon,DIRECT',
                  'PROCESS-NAME,transmission-qt,DIRECT',
                  'PROCESS-NAME,uTorrent,DIRECT',
                  'PROCESS-NAME,WebTorrent,DIRECT',
                  'PROCESS-NAME,Folx,DIRECT',
                  'PROCESS-NAME,Transmission,DIRECT',
                  'PROCESS-NAME,WebTorrent Helper,DIRECT',
                  'PROCESS-NAME,v2ray,DIRECT',
                  'PROCESS-NAME,ss-local,DIRECT',
                  'PROCESS-NAME,ssr-local,DIRECT',
                  'PROCESS-NAME,ss-redir,DIRECT',
                  'PROCESS-NAME,ssr-redir,DIRECT',
                  'PROCESS-NAME,ss-server,DIRECT',
                  'PROCESS-NAME,trojan-go,DIRECT',
                  'PROCESS-NAME,xray,DIRECT',
                  'PROCESS-NAME,hysteria,DIRECT',
                  'PROCESS-NAME,singbox,DIRECT',
                  'PROCESS-NAME,UUBooster,DIRECT',
                  'PROCESS-NAME,uugamebooster,DIRECT',
                  'DST-PORT,80,' + common_port_group,
                  'DST-PORT,443,' + common_port_group
                  );
               end;
            end
            Value['rules'].to_a.collect!{|x|x.to_s.gsub(/(^MATCH.*|^FINAL.*)/, 'MATCH,DIRECT')};
         end;
      rescue Exception => e
         YAML.LOG('Error: Set BT/P2P DIRECT Rules Failed,【' + e.message + '】');
      end;

      #Custom Rule Provider
      begin
         if File::exist?('$RULE_PROVIDER_FILE') then
            Value_1 = YAML.load_file('$RULE_PROVIDER_FILE');
            if Value.has_key?('rule-providers') and not Value['rule-providers'].to_a.empty? then
               Value['rule-providers'].merge!(Value_1);
            else
               Value['rule-providers']=Value_1;
            end;
         end;
      rescue Exception => e
         YAML.LOG('Error: Custom Rule Provider Merge Failed,【' + e.message + '】');
      end;

      #Game Proxy
      begin
         if File::exist?('/tmp/yaml_groups.yaml') or File::exist?('/tmp/yaml_servers.yaml') or File::exist?('/tmp/yaml_provider.yaml') then
            if File::exist?('/tmp/yaml_groups.yaml') then
               Value_1 = YAML.load_file('/tmp/yaml_groups.yaml');
               if Value.has_key?('proxy-groups') and not Value['proxy-groups'].to_a.empty? then
                  Value['proxy-groups'] = Value['proxy-groups'] + Value_1;
                  Value['proxy-groups'].uniq;
               else
                  Value['proxy-groups'] = Value_1;
               end;
            end;
            if File::exist?('/tmp/yaml_servers.yaml') then
               Value_2 = YAML.load_file('/tmp/yaml_servers.yaml');
               if Value.has_key?('proxies') and not Value['proxies'].to_a.empty? then
                  Value['proxies'] = Value['proxies'] + Value_2['proxies'];
                  Value['proxies'].uniq;
               else
                  Value['proxies']=Value_2['proxies'];
               end
            end;
            if File::exist?('/tmp/yaml_provider.yaml') then
               Value_3 = YAML.load_file('/tmp/yaml_provider.yaml');
               if Value.has_key?('proxy-providers') and not Value['proxy-providers'].to_a.empty? then
                  Value['proxy-providers'].merge!(Value_3['proxy-providers']);
                  Value['proxy-providers'].uniq;
               else
                  Value['proxy-providers']=Value_3['proxy-providers'];
               end;
            end;
         end;
      rescue Exception => e
         YAML.LOG('Error: Game Proxy Merge Failed,【' + e.message + '】');
      end;

      #CONFIG_GROUP
      CUSTOM_RULE = YAML.load_file('/etc/openclash/custom/openclash_custom_rules.list')
      CUSTOM_RULE_2 = YAML.load_file('/etc/openclash/custom/openclash_custom_rules_2.list')
      CONFIG_GROUP = (Value['proxy-groups'].map { |x| x['name'] }\
      + ['DIRECT', 'REJECT']\
      + (if Value['proxies'] != nil and not Value['proxies'].empty? then Value['proxies'].map { |x| x['name'] } else [] end)\
      + (if Value['sub-rules'] != nil and not Value['sub-rules'].empty? then Value['sub-rules'].keys else [] end)\
      + (if CUSTOM_RULE['sub-rules'] != nil and not CUSTOM_RULE['sub-rules'].empty? then CUSTOM_RULE['sub-rules'].keys else [] end)\
      + (if CUSTOM_RULE_2['sub-rules'] != nil and not CUSTOM_RULE_2['sub-rules'].empty? then CUSTOM_RULE_2['sub-rules'].keys else [] end)\
      ).uniq;

      #Custom Rule Set
      begin
         if Value.has_key?('rules') and not Value['rules'].to_a.empty? then
            if File::exist?('/tmp/yaml_rule_set_bottom_custom.yaml') then
               if $4 != 1 then
                  ruby_add_index = Value['rules'].index(Value['rules'].grep(/(GEOIP|MATCH|FINAL)/).first);
               else
                  if Value['rules'].grep(/GEOIP/)[0].nil? or Value['rules'].grep(/GEOIP/)[0].empty? then
                     ruby_add_index = Value['rules'].index(Value['rules'].grep(/DST-PORT,80/).last);
                     ruby_add_index ||= Value['rules'].index(Value['rules'].grep(/(MATCH|FINAL)/).first);
                  else
                     ruby_add_index = Value['rules'].index(Value['rules'].grep(/GEOIP/).first);
                  end;
               end;
               ruby_add_index ||= -1;
               Value_1 = YAML.load_file('/tmp/yaml_rule_set_bottom_custom.yaml');
               if ruby_add_index != -1 then
                  Value_1['rules'].uniq.reverse.each{|x|
                     RULE_GROUP = (x.split(',')[-1] =~ /^no-resolve$|^src$/) ? x.split(',')[-2] : x.split(',')[-1];
                     if CONFIG_GROUP.include?(RULE_GROUP) then
                        Value['rules'].insert(ruby_add_index,x);
                     else
                        YAML.LOG('Warning: Skiped The Custom Rule Because Group & Proxy Not Found:【' + x + '】');
                     end;
                  };
               else
                  Value_1['rules'].uniq.each{|x|
                     RULE_GROUP = (x.split(',')[-1] =~ /^no-resolve$|^src$/) ? x.split(',')[-2] : x.split(',')[-1];
                     if CONFIG_GROUP.include?(RULE_GROUP) then
                        Value['rules'].insert(ruby_add_index,x);
                     else
                        YAML.LOG('Warning: Skiped The Custom Rule Because Group & Proxy Not Found:【' + x + '】');
                     end;
                  };
               end;
            end;
            if File::exist?('/tmp/yaml_rule_set_top_custom.yaml') then
               Value_1 = YAML.load_file('/tmp/yaml_rule_set_top_custom.yaml');
               Value_1['rules'].uniq.reverse.each{|x|
                  RULE_GROUP = (x.split(',')[-1] =~ /^no-resolve$|^src$/) ? x.split(',')[-2] : x.split(',')[-1];
                  if CONFIG_GROUP.include?(RULE_GROUP) then
                     Value['rules'].insert(0,x);
                  else
                     YAML.LOG('Warning: Skiped The Custom Rule Because Group & Proxy Not Found:【' + x + '】');
                  end;
               };
            end;
         else
            if File::exist?('/tmp/yaml_rule_set_top_custom.yaml') then
               Value_1 = YAML.load_file('/tmp/yaml_rule_set_top_custom.yaml')['rules'].uniq;
               Value_1.each{|x|
                  RULE_GROUP = (x.split(',')[-1] =~ /^no-resolve$|^src$/) ? x.split(',')[-2] : x.split(',')[-1];
                  if not CONFIG_GROUP.include?(RULE_GROUP) then
                     Value_1.delete(x);
                     YAML.LOG('Warning: Skiped The Custom Rule Because Group & Proxy Not Found:【' + x + '】');
                  end;
               };
               Value['rules'] = Value_1;
            end;
            if File::exist?('/tmp/yaml_rule_set_bottom_custom.yaml') then
               Value_1 = YAML.load_file('/tmp/yaml_rule_set_bottom_custom.yaml')['rules'].uniq;
               Value_1.each{|x|
                  RULE_GROUP = (x.split(',')[-1] =~ /^no-resolve$|^src$/) ? x.split(',')[-2] : x.split(',')[-1];
                  if not CONFIG_GROUP.include?(RULE_GROUP) then
                     Value_1.delete(x);
                     YAML.LOG('Warning: Skiped The Custom Rule Because Group & Proxy Not Found:【' + x + '】');
                  end;
               };
               if File::exist?('/tmp/yaml_rule_set_top_custom.yaml') then
                  Value['rules'] = Value['rules'] | Value_1;
               else
                  Value['rules'] = Value_1;
               end;
            end;
         end;
      rescue Exception => e
         YAML.LOG('Error: Rule Set Add Failed,【' + e.message + '】');
      end;

      #Custom Rules
      begin
         if $2 == 1 then
         #rules
            if Value.has_key?('rules') and not Value['rules'].to_a.empty? then
               if File::exist?('/etc/openclash/custom/openclash_custom_rules.list') then
                  Value_1 = YAML.load_file('/etc/openclash/custom/openclash_custom_rules.list');
                  if Value_1 != false then
                     if Value_1.class.to_s == 'Hash' then
                        if not Value_1['rules'].to_a.empty? and Value_1['rules'].class.to_s == 'Array' then
                           Value_2 = Value_1['rules'].to_a.reverse!;
                        end;
                     elsif Value_1.class.to_s == 'Array' then
                        Value_2 = Value_1.reverse!;
                     end;
                     if defined? Value_2 then
                        Value_2.each{|x|
                           RULE_GROUP = (x.split(',')[-1] =~ /^no-resolve$|^src$/) ? x.split(',')[-2] : x.split(',')[-1];
                           if CONFIG_GROUP.include?(RULE_GROUP) then
                              Value['rules'].insert(0,x);
                           else
                              YAML.LOG('Warning: Skiped The Custom Rule Because Group & Proxy Not Found:【' + x + '】');
                           end;
                        };
                        Value['rules'] = Value['rules'].uniq;
                     end;
                  end;
               end;
               if File::exist?('/etc/openclash/custom/openclash_custom_rules_2.list') then
                  Value_3 = YAML.load_file('/etc/openclash/custom/openclash_custom_rules_2.list');
                  if Value_3 != false then
                     if Value['rules'].grep(/GEOIP/)[0].nil? or Value['rules'].grep(/GEOIP/)[0].empty? then
                        ruby_add_index = Value['rules'].index(Value['rules'].grep(/DST-PORT,80/).last);
                        ruby_add_index ||= Value['rules'].index(Value['rules'].grep(/(MATCH|FINAL)/).first);
                     else
                        ruby_add_index = Value['rules'].index(Value['rules'].grep(/GEOIP/).first);
                     end;
                     ruby_add_index ||= -1;
                     if Value_3.class.to_s == 'Hash' then
                        if not Value_3['rules'].to_a.empty? and Value_3['rules'].class.to_s == 'Array' then
                           Value_4 = Value_3['rules'].to_a.reverse!;
                        end;
                     elsif Value_3.class.to_s == 'Array' then
                        Value_4 = Value_3.reverse!;
                     end;
                     if defined? Value_4 then
                        if ruby_add_index == -1 then
                           Value_4 = Value_4.reverse!;
                        end;
                        Value_4.each{|x|
                           RULE_GROUP = (x.split(',')[-1] =~ /^no-resolve$|^src$/) ? x.split(',')[-2] : x.split(',')[-1];
                           if CONFIG_GROUP.include?(RULE_GROUP) then
                              Value['rules'].insert(ruby_add_index,x);
                           else
                              YAML.LOG('Warning: Skiped The Custom Rule Because Group & Proxy Not Found:【' + x + '】');
                           end;
                        };
                        Value['rules'] = Value['rules'].uniq;
                     end;
                  end;
               end;
            else
               if File::exist?('/etc/openclash/custom/openclash_custom_rules.list') then
                  Value_1 = YAML.load_file('/etc/openclash/custom/openclash_custom_rules.list');
                  if Value_1 != false then
                     if Value_1.class.to_s == 'Hash' then
                        if not Value_1['rules'].to_a.empty? and Value_1['rules'].class.to_s == 'Array' then
                           Value_1['rules'].to_a.each{|x|
                              RULE_GROUP = (x.split(',')[-1] =~ /^no-resolve$|^src$/) ? x.split(',')[-2] : x.split(',')[-1];
                              if not CONFIG_GROUP.include?(RULE_GROUP) then
                                 Value_1['rules'].delete(x);
                                 YAML.LOG('Warning: Skiped The Custom Rule Because Group & Proxy Not Found:【' + x + '】');
                              end;
                           };
                           Value['rules'] = Value_1['rules'];
                           Value['rules'] = Value['rules'].uniq;
                        end;
                     elsif Value_1.class.to_s == 'Array' then
                        Value_1.each{|x|
                           RULE_GROUP = (x.split(',')[-1] =~ /^no-resolve$|^src$/) ? x.split(',')[-2] : x.split(',')[-1];
                           if not CONFIG_GROUP.include?(RULE_GROUP) then
                              Value_1.delete(x);
                              YAML.LOG('Warning: Skiped The Custom Rule Because Group & Proxy Not Found:【' + x + '】');
                           end;
                        };
                        Value['rules'] = Value_1;
                        Value['rules'] = Value['rules'].uniq;
                     end;
                  end;
               end;
               if File::exist?('/etc/openclash/custom/openclash_custom_rules_2.list') then
                  Value_2 = YAML.load_file('/etc/openclash/custom/openclash_custom_rules_2.list');
                  if Value_2 != false then
                     if Value['rules'].to_a.empty? then
                        if Value_2.class.to_s == 'Hash' then
                           if not Value_2['rules'].to_a.empty? and Value_2['rules'].class.to_s == 'Array' then
                              Value_2['rules'].to_a.each{|x|
                                 RULE_GROUP = (x.split(',')[-1] =~ /^no-resolve$|^src$/) ? x.split(',')[-2] : x.split(',')[-1];
                                 if not CONFIG_GROUP.include?(RULE_GROUP) then
                                    Value_2['rules'].delete(x);
                                    YAML.LOG('Warning: Skiped The Custom Rule Because Group & Proxy Not Found:【' + x + '】');
                                 end;
                              };
                              Value['rules'] = Value_2['rules'];
                              Value['rules'] = Value['rules'].uniq;
                           end;
                        elsif Value_2.class.to_s == 'Array' then
                           Value_2.each{|x|
                              RULE_GROUP = (x.split(',')[-1] =~ /^no-resolve$|^src$/) ? x.split(',')[-2] : x.split(',')[-1];
                              if not CONFIG_GROUP.include?(RULE_GROUP) then
                                 Value_2.delete(x);
                                 YAML.LOG('Warning: Skiped The Custom Rule Because Group & Proxy Not Found:【' + x + '】');
                              end;
                           };
                           Value['rules'] = Value_2;
                           Value['rules'] = Value['rules'].uniq;
                        end;
                     else
                        if Value['rules'].grep(/GEOIP/)[0].nil? or Value['rules'].grep(/GEOIP/)[0].empty? then
                           ruby_add_index = Value['rules'].index(Value['rules'].grep(/DST-PORT,80/).last);
                           ruby_add_index ||= Value['rules'].index(Value['rules'].grep(/(MATCH|FINAL)/).first);
                        else
                           ruby_add_index = Value['rules'].index(Value['rules'].grep(/GEOIP/).first);
                        end;
                        ruby_add_index ||= -1;
                        if Value_2.class.to_s == 'Hash' then
                        if not Value_2['rules'].to_a.empty? and Value_2['rules'].class.to_s == 'Array' then
                           Value_3 = Value_2['rules'].to_a.reverse!;
                        end;
                        elsif Value_2.class.to_s == 'Array' then
                           Value_3 = Value_2.reverse!;
                        end;
                        if defined? Value_3 then
                           if ruby_add_index == -1 then
                              Value_3 = Value_3.reverse!;
                           end
                           Value_3.each{|x|
                              RULE_GROUP = (x.split(',')[-1] =~ /^no-resolve$|^src$/) ? x.split(',')[-2] : x.split(',')[-1];
                              if CONFIG_GROUP.include?(RULE_GROUP) then
                                 Value['rules'].insert(ruby_add_index,x);
                              else
                                 YAML.LOG('Warning: Skiped The Custom Rule Because Group & Proxy Not Found:【' + x + '】');
                              end;
                           };
                           Value['rules'] = Value['rules'].uniq;
                        end;
                     end;
                  end;
               end;
            end;
         #SUB-RULE
            if Value.has_key?('sub-rules') and not Value['sub-rules'].to_a.empty? then
               if File::exist?('/etc/openclash/custom/openclash_custom_rules.list') then
                  Value_1 = YAML.load_file('/etc/openclash/custom/openclash_custom_rules.list');
                  if Value_1 != false then
                     if Value_1.class.to_s == 'Hash' then
                        if not Value_1['sub-rules'].to_a.empty? and Value_1['sub-rules'].class.to_s == 'Hash' then
                           Value['sub-rules'] = Value['sub-rules'].merge!(Value_1['sub-rules']);
                        end;
                     end;
                  end;
               end;
               if File::exist?('/etc/openclash/custom/openclash_custom_rules_2.list') then
                  Value_2 = YAML.load_file('/etc/openclash/custom/openclash_custom_rules_2.list');
                  if Value_2 != false then
                     if Value_2.class.to_s == 'Hash' then
                        if not Value_2['sub-rules'].to_a.empty? and Value_2['sub-rules'].class.to_s == 'Hash' then
                           Value['sub-rules'] = Value['sub-rules'].merge!(Value_2['sub-rules']);
                        end;
                     end;
                  end;
               end;
            else
               if File::exist?('/etc/openclash/custom/openclash_custom_rules.list') then
                  Value_1 = YAML.load_file('/etc/openclash/custom/openclash_custom_rules.list');
                  if Value_1 != false then
                     if Value_1.class.to_s == 'Hash' then
                        if not Value_1['sub-rules'].to_a.empty? and Value_1['sub-rules'].class.to_s == 'Hash' then
                           Value['sub-rules'] = Value_1['sub-rules'];
                        end;
                     end;
                  end;
               end;
               if File::exist?('/etc/openclash/custom/openclash_custom_rules_2.list') then
                  Value_2 = YAML.load_file('/etc/openclash/custom/openclash_custom_rules_2.list');
                  if Value_2 != false then
                     if Value_2.class.to_s == 'Hash' then
                        if not Value_2['sub-rules'].to_a.empty? and Value_2['sub-rules'].class.to_s == 'Hash' then
                           Value['sub-rules'] = Value_2['sub-rules'];
                        end;
                     end;
                  end;
               end;
            end;
         end;
      rescue Exception => e
         YAML.LOG('Error: Set Custom Rules Failed,【' + e.message + '】');
      end;

      #Router Self Proxy Rule
      begin
         if $6 == 0 and $8 != 2 and '$9' == 'fake-ip' then
            if Value.has_key?('rules') and not Value['rules'].to_a.empty? then
               if Value['rules'].to_a.grep(/(?=.*SRC-IP-CIDR,'$7')/).empty? and not '$7'.empty? then
                  Value['rules']=Value['rules'].to_a.insert(0,'SRC-IP-CIDR,$7/32,DIRECT');
               end;
            else
               Value['rules']=['SRC-IP-CIDR,$7/32,DIRECT'];
            end;
         elsif Value.has_key?('rules') and not Value['rules'].to_a.empty? then
            Value['rules'].delete('SRC-IP-CIDR,$7/32,DIRECT');
         end;
      rescue Exception => e
         YAML.LOG('Error: Set Router Self Proxy Rule Failed,【' + e.message + '】');
      end;
   };

   t2=Thread.new{
      #Create threads
      threads = [];

      #provider path
      begin
         provider = {'proxy-providers' => 'proxy_provider', 'rule-providers' => 'rule_provider'}
         provider.each do |i, p|
            if Value.key?(i) then
               Value[i].values.each{
               |x,v|
                  threads << Thread.new {
                     if x['path'] and not x['path'] =~ /.\/#{p}\/*/ and not x['path'] =~ /.\/game_rules\/*/ then
                        v=File.basename(x['path']);
                        x['path']='./'+p+'/'+v;
                     end;
                     if not x['path'] and x['type'] == 'http' then
                        x['path']='./'+p+'/'+x['name'];
                     end;
                     #CDN Replace
                     if '$github_address_mod' != '0' then
                        if '$github_address_mod' == 'https://cdn.jsdelivr.net/' or '$github_address_mod' == 'https://fastly.jsdelivr.net/' or '$github_address_mod' == 'https://testingcf.jsdelivr.net/'then
                           if x['url'] and x['url'] =~ /^https:\/\/raw.githubusercontent.com/ then
                              x['url'] = '$github_address_mod' + 'gh/' + x['url'].split('/')[3] + '/' + x['url'].split('/')[4] + '@' + x['url'].split(x['url'].split('/')[2] + '/' + x['url'].split('/')[3] + '/' + x['url'].split('/')[4] + '/')[1];
                           end;
                        else
                           if x['url'] and x['url'] =~ /^https:\/\/(raw.|gist.)(githubusercontent.com|github.com)/ then
                              x['url'] = '$github_address_mod' + x['url'];
                           end;
                        end;
                     end;
                  };
               };
            end;
         end;
      rescue Exception => e
         YAML.LOG('Error: Edit Provider Path Failed,【' + e.message + '】');
      end;

      #tolerance
      begin
         if '$tolerance' != '0' then
            Value['proxy-groups'].each{
            |x|
               threads << Thread.new {
                  if x['type'] == 'url-test' then
                     x['tolerance']=${tolerance};
                  end
               };
            };
         end;
      rescue Exception => e
         YAML.LOG('Error: Edit URL-Test Group Tolerance Option Failed,【' + e.message + '】');
      end;

      #URL-Test interval
      begin
         if '$urltest_interval_mod' != '0' then
            if Value.key?('proxy-groups') then
               Value['proxy-groups'].each{
                  |x|
                  threads << Thread.new {
                     if x['type'] == 'url-test' or x['type'] == 'fallback' or x['type'] == 'load-balance' then
                        x['interval']=${urltest_interval_mod};
                     end
                  };
               };
            end;
            if Value.key?('proxy-providers') then
               Value['proxy-providers'].values.each{
                  |x|
                  threads << Thread.new {
                     if x['health-check'] and x['health-check']['enable'] and x['health-check']['enable'] == 'true' then
                        x['health-check']['interval']=${urltest_interval_mod};
                     end;
                  };
               };
            end;
         end;
      rescue Exception => e
         YAML.LOG('Error: Edit URL-Test Interval Failed,【' + e.message + '】');
      end;

      #health-check url
      begin
         if '$urltest_address_mod' != '0' then
            if Value.key?('proxy-providers') then
               Value['proxy-providers'].values.each{
               |x|
                  threads << Thread.new {
                     if x['health-check'] and x['health-check']['enable'] and x['health-check']['enable'] == 'true' then
                        x['health-check']['url']='$urltest_address_mod';
                     end;
                  };
               };
            end;
            if Value.key?('proxy-groups') then
               Value['proxy-groups'].each{
               |x|
                  threads << Thread.new {
                     if x['type'] == 'url-test' or x['type'] == 'fallback' or x['type'] == 'load-balance' then
                        x['url']='$urltest_address_mod';
                     end;
                  };
               };
            end;
         end;
      rescue Exception => e
         YAML.LOG('Error: Edit URL-Test URL Failed,【' + e.message + '】');
      end;

      #Run threads
      threads.each(&:join);
   };
   
   begin
      t1.join;
      t2.join;
   ensure
      File.open('$3','w') {|f| YAML.dump(Value, f)};
   end" 2>/dev/null >> $LOG_FILE
}

yml_other_rules_get()
{
   local section="$1"
   local enabled config
   config_get_bool "enabled" "$section" "enabled" "1"
   config_get "config" "$section" "config" ""
   
   if [ "$enabled" = "0" ] || [ "$config" != "$2" ]; then
      return
   fi
   
   if [ -n "$rule_name" ]; then
      LOG_OUT "Warning: Multiple Other-Rules-Configurations Enabled, Ignore..."
      return
   fi
   
   config_get "rule_name" "$section" "rule_name" ""
   config_get "GlobalTV" "$section" "GlobalTV" ""
   config_get "AsianTV" "$section" "AsianTV" ""
   config_get "MainlandTV" "$section" "MainlandTV" "DIRECT"
   config_get "Proxy" "$section" "Proxy" ""
   config_get "Youtube" "$section" "Youtube" ""
   config_get "Bilibili" "$section" "Bilibili" ""
   config_get "Bahamut" "$section" "Bahamut" ""
   config_get "HBOMax" "$section" "HBOMax" "$GlobalTV"
   config_get "Pornhub" "$section" "Pornhub" ""
   config_get "Apple" "$section" "Apple" ""
   config_get "Scholar" "$section" "Scholar" ""
   config_get "Netflix" "$section" "Netflix" ""
   config_get "Disney" "$section" "Disney" ""
   config_get "Spotify" "$section" "Spotify" ""
   config_get "Steam" "$section" "Steam" ""
   config_get "AdBlock" "$section" "AdBlock" ""
   config_get "HTTPDNS" "$section" "HTTPDNS" "REJECT"
   config_get "Netease_Music" "$section" "Netease_Music" ""
   config_get "Speedtest" "$section" "Speedtest" ""
   config_get "Telegram" "$section" "Telegram" ""
   config_get "Crypto" "$section" "Crypto" "$Proxy"
   config_get "Discord" "$section" "Discord" "$Proxy"
   config_get "Microsoft" "$section" "Microsoft" ""
   config_get "PayPal" "$section" "PayPal" ""
   config_get "Domestic" "$section" "Domestic" ""
   config_get "Others" "$section" "Others" ""
   config_get "GoogleFCM" "$section" "GoogleFCM" "DIRECT"
   config_get "Discovery" "$section" "Discovery" "$GlobalTV"
   config_get "DAZN" "$section" "DAZN" "$GlobalTV"
   config_get "AI_Suite" "$section" "AI_Suite" "$Proxy"
   config_get "AppleTV" "$section" "AppleTV" "$GlobalTV"
   config_get "miHoYo" "$section" "miHoYo" "$Domestic"
}

if [ "$1" != "0" ]; then
   /usr/share/openclash/yml_groups_name_get.sh
   if [ $? -ne 0 ]; then
      LOG_OUT "Error: Unable To Parse Config File, Please Check And Try Again!"
      exit 0
   fi
   config_load "openclash"
   config_foreach yml_other_rules_get "other_rules" "$5"
   if [ -z "$rule_name" ]; then
      SKIP_CUSTOM_OTHER_RULES=1
      yml_other_set "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9"
      exit 0
   #判断策略组是否存在
   elif [ "$rule_name" = "lhie1" ]; then
       if [ -z "$(grep -F "$GlobalTV" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$AsianTV" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$MainlandTV" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Proxy" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Youtube" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Bilibili" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Bahamut" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$HBOMax" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Pornhub" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Apple" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$AppleTV" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Scholar" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Netflix" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Disney" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Discovery" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$DAZN" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$AI_Suite" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Spotify" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Steam" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$miHoYo" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$AdBlock" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$HTTPDNS" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Speedtest" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Telegram" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Crypto" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Discord" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Microsoft" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$PayPal" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Others" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$GoogleFCM" /tmp/Proxy_Group)" ]\
    || [ -z "$(grep -F "$Domestic" /tmp/Proxy_Group)" ]; then
         LOG_OUT "Warning: Because of The Different Porxy-Group's Name, Stop Setting The Other Rules!"
         SKIP_CUSTOM_OTHER_RULES=1
         yml_other_set "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9"
         exit 0
       fi
   fi
   if [ -z "$Proxy" ]; then
      LOG_OUT "Error: Missing Porxy-Group's Name, Stop Setting The Other Rules!"
      SKIP_CUSTOM_OTHER_RULES=1
      yml_other_set "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9"
      exit 0
   fi
fi

yml_other_set "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9"
