#!/bin/bash
. /lib/functions.sh
. /usr/share/openclash/ruby.sh
. /usr/share/openclash/openclash_ps.sh
. /usr/share/openclash/log.sh
. /lib/functions/procd.sh

set_lock() {
   exec 889>"/tmp/lock/openclash_subs.lock" 2>/dev/null
   flock -x 889 2>/dev/null
}

del_lock() {
   flock -u 889 2>/dev/null
   rm -rf "/tmp/lock/openclash_subs.lock" 2>/dev/null
}

set_lock

LOGTIME=$(echo $(date "+%Y-%m-%d %H:%M:%S"))
LOG_FILE="/tmp/openclash.log"
CFG_FILE="/tmp/yaml_sub_tmp_config.yaml"
CRON_FILE="/etc/crontabs/root"
CONFIG_PATH=$(uci -q get openclash.config.config_path)
servers_update=$(uci -q get openclash.config.servers_update)
router_self_proxy=$(uci -q get openclash.config.router_self_proxy || echo 1)
FW4=$(command -v fw4)
CLASH="/etc/openclash/clash"
CLASH_CONFIG="/etc/openclash"
restart=0
only_download=0

urlencode() {
   if [ "$#" -eq 1 ]; then
      echo "$(/usr/share/openclash/openclash_urlencode.lua "$1")"
   fi
}

kill_streaming_unlock() {
   streaming_unlock_pids=$(unify_ps_pids "openclash_streaming_unlock.lua")
   for streaming_unlock_pid in $streaming_unlock_pids; do
      kill -9 "$streaming_unlock_pid" >/dev/null 2>&1
   done >/dev/null 2>&1
}

config_test()
{
   if [ -f "$CLASH" ]; then
      LOG_OUT "Config File Download Successful, Test If There is Any Errors..."
      test_info=$($CLASH -t -d $CLASH_CONFIG -f "$CFG_FILE")
      local IFS=$'\n'
      for i in $test_info; do
         if [ -n "$(echo "$i" |grep "configuration file")" ]; then
            local info=$(echo "$i" |sed "s# ${CFG_FILE} #【${CONFIG_FILE}】#g")
            LOG_OUT "$info"
         else
            echo "$i" >> "$LOG_FILE"
         fi
      done
      if [ -n "$(echo "$test_info" |grep "test failed")" ]; then
         return 1
      fi
   else
      return 0
   fi
}

config_download()
{
LOG_OUT "Tip: Config File【$name】Downloading User-Agent【$sub_ua】..."
if [ -n "$subscribe_url_param" ]; then
   if [ -n "$c_address" ]; then
      echo "$LOGTIME Tip: Config File【$name】Downloading URL【$c_address$subscribe_url_param】..." >> $LOG_FILE
      curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 -H "$sub_ua" "$c_address""$subscribe_url_param" -o "$CFG_FILE" 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="$CFG_FILE" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
   else
      echo "$LOGTIME Tip: Config File【$name】Downloading URL【https://api.dler.io/sub$subscribe_url_param】..." >> $LOG_FILE
      curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 -H "$sub_ua" https://api.dler.io/sub"$subscribe_url_param" -o "$CFG_FILE" 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="$CFG_FILE" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
   fi
else
   echo "$LOGTIME Tip: Config File【$name】Downloading URL【$subscribe_url】..." >> $LOG_FILE
   curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 -H "$sub_ua" "$subscribe_url" -o "$CFG_FILE" 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="$CFG_FILE" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
fi
}

config_cus_up()
{
	if [ -z "$CONFIG_PATH" ]; then
      for file_name in /etc/openclash/config/*
      do
         if [ -f "$file_name" ]; then
            CONFIG_PATH=$file_name
            break
         fi
      done
      uci -q set openclash.config.config_path="$CONFIG_PATH"
      uci commit openclash
	fi
	if [ -z "$subscribe_url_param" ]; then
	   if [ -n "$key_match_param" ] || [ -n "$key_ex_match_param" ]; then
	      LOG_OUT "Config File【$name】is Replaced Successfully, Start Picking Nodes..."	      
	      ruby -ryaml -rYAML -I "/usr/share/openclash" -E UTF-8 -e "
	      begin
            threads = [];
	         Value = YAML.load_file('$CONFIG_FILE');
	         if Value.has_key?('proxies') and not Value['proxies'].to_a.empty? then
	            Value['proxies'].reverse.each{
	            |x|
                  if not '$key_match_param'.empty? then
                     threads << Thread.new {
                        if not /$key_match_param/i =~ x['name'] then
                           Value['proxies'].delete(x)
                           Value['proxy-groups'].each{
                              |g|
                              g['proxies'].reverse.each{
                                 |p|
                                 if p == x['name'] then
                                    g['proxies'].delete(p)
                                 end;
                              };
                           };
                        end;
                     };
                  end;
                  if not '$key_ex_match_param'.empty? then
                     threads << Thread.new {
                        if /$key_ex_match_param/i =~ x['name'] then
                           if Value['proxies'].include?(x) then
                              Value['proxies'].delete(x)
                              Value['proxy-groups'].each{
                                 |g|
                                 g['proxies'].reverse.each{
                                    |p|
                                    if p == x['name'] then
                                       g['proxies'].delete(p)
                                    end;
                                 };
                              };
                           end;
                        end;
                     };
                  end;
	            };
	         end;
            if Value.key?('proxy-providers') and not Value['proxy-providers'].nil? then
               Value['proxy-providers'].values.each do
                  |i|
                  threads << Thread.new {
                     if not '$key_match_param'.empty? then
                        i['filter'] = '(?i)$key_match_param';
                     end;
                     if not '$key_ex_match_param'.empty? then
                        i['exclude-filter'] = '(?i)$key_ex_match_param';
                     end;
                  };
               end;
            end;
            threads.each(&:join);
	      rescue Exception => e
	         YAML.LOG('Error: Filter Proxies Failed,【' + e.message + '】');
	      ensure
	         File.open('$CONFIG_FILE','w') {|f| YAML.dump(Value, f)};
	      end" 2>/dev/null >> $LOG_FILE
	   fi
   fi
   if [ "$servers_update" -eq 1 ]; then
      LOG_OUT "Config File【$name】is Replaced Successfully, Start to Reserving..."
      uci -q set openclash.config.config_update_path="/etc/openclash/config/$name.yaml"
      uci -q set openclash.config.servers_if_update=1
      uci commit openclash
      /usr/share/openclash/yml_groups_get.sh
      uci -q set openclash.config.servers_if_update=1
      uci commit openclash
      /usr/share/openclash/yml_groups_set.sh
      if [ "$CONFIG_FILE" == "$CONFIG_PATH" ]; then
         restart=1
      fi
      LOG_OUT "Config File【$name】Update Successful!"
      SLOG_CLEAN
   elif [ "$CONFIG_FILE" == "$CONFIG_PATH" ]; then
      LOG_OUT "Config File【$name】Update Successful!"
      restart=1
   else
      LOG_OUT "Config File【$name】Update Successful!"
      SLOG_CLEAN
   fi
   
   rm -rf /tmp/Proxy_Group 2>/dev/null
}

config_su_check()
{
   LOG_OUT "Config File Test Successful, Check If There is Any Update..."
   sed -i 's/!<str> /!!str /g' "$CFG_FILE" >/dev/null 2>&1
   if [ -f "$CONFIG_FILE" ]; then
      cmp -s "$BACKPACK_FILE" "$CFG_FILE"
      if [ "$?" -ne 0 ]; then
         LOG_OUT "Config File【$name】Are Updates, Start Replacing..."
         cp "$CFG_FILE" "$BACKPACK_FILE"
         #保留规则部分
         if [ "$servers_update" -eq 1 ] && [ "$only_download" -eq 0 ]; then
   	        ruby -ryaml -rYAML -I "/usr/share/openclash" -E UTF-8 -e "
               Value = YAML.load_file('$CONFIG_FILE');
               Value_1 = YAML.load_file('$CFG_FILE');
               if Value.key?('rules') or Value.key?('script') or Value.key?('rule-providers') then
                  if Value.key?('rules') then
                     Value_1['rules'] = Value['rules']
                  end;
                  if Value.key?('script') then
                     Value_1['script'] = Value['script']
                  end;
                  if Value.key?('rule-providers') then
                     Value_1['rule-providers'] = Value['rule-providers']
                  end;
                  File.open('$CFG_FILE','w') {|f| YAML.dump(Value_1, f)};
               end;
            " 2>/dev/null
         fi
         mv "$CFG_FILE" "$CONFIG_FILE" 2>/dev/null
         if [ "$only_download" -eq 0 ]; then
            config_cus_up
         else
            LOG_OUT "Config File【$name】Update Successful!"
            SLOG_CLEAN
         fi
      else
         LOG_OUT "Config File【$name】No Change, Do Nothing!"
         rm -rf "$CFG_FILE"
         SLOG_CLEAN
      fi
   else
      LOG_OUT "Config File【$name】Download Successful, Start To Create..."
      mv "$CFG_FILE" "$CONFIG_FILE" 2>/dev/null
      cp "$CONFIG_FILE" "$BACKPACK_FILE"
      if [ "$only_download" -eq 0 ]; then
         config_cus_up
      else
         LOG_OUT "Config File【$name】Update Successful!"
         SLOG_CLEAN
      fi
   fi
}

config_error()
{
   LOG_OUT "Error:【$name】Update Error, Please Try Again Later..."
   rm -rf "$CFG_FILE" 2>/dev/null
   SLOG_CLEAN
}

change_dns()
{
   if pidof clash >/dev/null; then
      /etc/init.d/openclash reload "restore" >/dev/null 2>&1
      procd_send_signal "openclash" "openclash-watchdog" CONT
   fi
}

config_download_direct()
{
   if pidof clash >/dev/null && [ "$router_self_proxy" = 1 ]; then
      kill_streaming_unlock
      procd_send_signal "openclash" "openclash-watchdog" STOP
      /etc/init.d/openclash reload "revert" >/dev/null 2>&1
      sleep 3

      config_download
      
      if [ "${PIPESTATUS[0]}" -eq 0 ] && [ -s "$CFG_FILE" ]; then
         #prevent ruby unexpected error
         sed -i -E 's/protocol-param: ([^,'"'"'"''}( *#)\n\r]+)/protocol-param: "\1"/g' "$CFG_FILE" 2>/dev/null
         sed -i '/^ \{0,\}enhanced-mode:/d' "$CFG_FILE" >/dev/null 2>&1
         config_test
         if [ $? -ne 0 ]; then
            LOG_OUT "Error: Config File Tested Faild, Please Check The Log Infos!"
            change_dns
            config_error
            return
         fi
         ruby -ryaml -rYAML -I "/usr/share/openclash" -E UTF-8 -e "
         begin
         YAML.load_file('$CFG_FILE');
         rescue Exception => e
         YAML.LOG('Error: Unable To Parse Config File,【' + e.message + '】');
         system 'rm -rf ${CFG_FILE} 2>/dev/null'
         end
         " 2>/dev/null >> $LOG_FILE
         if [ $? -ne 0 ]; then
            LOG_OUT "Error: Ruby Works Abnormally, Please Check The Ruby Library Depends!"
            only_download=1
            change_dns
            config_su_check
         elif [ ! -f "$CFG_FILE" ]; then
            LOG_OUT "Config File Format Validation Failed..."
            change_dns
            config_error
         elif ! "$(ruby_read "$CFG_FILE" ".key?('proxies')")" && ! "$(ruby_read "$CFG_FILE" ".key?('proxy-providers')")" ; then
            LOG_OUT "Error: Updated Config【$name】Has No Proxy Field, Update Exit..."
            change_dns
            config_error
         else
            change_dns
            config_su_check
         fi
      else
         change_dns
         config_error
      fi
   else
      config_error
   fi
}

server_key_match()
{
	local key_match key_word
	 
   if [ -n "$(echo "$1" |grep "^ \{0,\}$")" ] || [ -n "$(echo "$1" |grep "^\t\{0,\}$")" ]; then
	    return
   fi
	 
   if [ -n "$(echo "$1" |grep "&")" ]; then
      key_word=$(echo "$1" |sed 's/&/ /g')
	    for k in $key_word
	    do
	       if [ -z "$k" ]; then
	          continue
	       fi
	       k="(?=.*$k)"
	       key_match="$key_match$k"
	    done
	    key_match="^($key_match).*"
   else
	    if [ -n "$1" ]; then
	       key_match="($1)"
	    fi
   fi
   
   if [ "$2" = "keyword" ]; then
      if [ -z "$key_match_param" ]; then
         key_match_param="$key_match"
      else
         key_match_param="$key_match_param|$key_match"
      fi
   elif [ "$2" = "ex_keyword" ]; then
   	  if [ -z "$key_ex_match_param" ]; then
         key_ex_match_param="$key_match"
      else
         key_ex_match_param="$key_ex_match_param|$key_match"
      fi
   fi
}

convert_custom_param()
{
   if ! (echo "$1" | grep -qE "^\w+=.+$") then
      return
   fi
   local p_name="${1%%=*}" p_value="${1#*=}"
   if [ -z "$append_custom_params" ]; then
      append_custom_params="&${p_name}=$(urlencode "$p_value")"
   else
      append_custom_params="${append_custom_params}\`$(urlencode "$p_value")"
   fi
}

sub_info_get()
{
   local section="$1" subscribe_url template_path subscribe_url_param template_path_encode key_match_param key_ex_match_param c_address de_ex_keyword sub_ua append_custom_params
   config_get_bool "enabled" "$section" "enabled" "1"
   config_get "name" "$section" "name" ""
   config_get "sub_convert" "$section" "sub_convert" ""
   config_get "address" "$section" "address" ""
   config_get "keyword" "$section" "keyword" ""
   config_get "ex_keyword" "$section" "ex_keyword" ""
   config_get "emoji" "$section" "emoji" ""
   config_get "udp" "$section" "udp" ""
   config_get "skip_cert_verify" "$section" "skip_cert_verify" ""
   config_get "sort" "$section" "sort" ""
   config_get "convert_address" "$section" "convert_address" ""
   config_get "template" "$section" "template" ""
   config_get "node_type" "$section" "node_type" ""
   config_get "rule_provider" "$section" "rule_provider" ""
   config_get "custom_template_url" "$section" "custom_template_url" ""
   config_get "de_ex_keyword" "$section" "de_ex_keyword" ""
   config_get "sub_ua" "$section" "sub_ua" "Clash"
   
   if [ "$enabled" -eq 0 ]; then
      if [ -n "$2" ]; then
         if [ "$2" != "$CONFIG_FILE" ] && [ "$2" != "$name" ]; then
            return
         fi
      else
         return
      fi
   fi
   
   if [ -z "$address" ]; then
      return
   fi
   
   if [ "$udp" == "true" ]; then
      udp="&udp=true"
   else
      udp=""
   fi

   if [ -n "$sub_ua" ]; then
      sub_ua="User-Agent: $sub_ua"
   fi
   
   if [ "$rule_provider" == "true" ]; then
      rule_provider="&expand=false&classic=true"
   else
      rule_provider=""
   fi
   
   if [ -z "$name" ]; then
      name="config"
      CONFIG_FILE="/etc/openclash/config/config.yaml"
      BACKPACK_FILE="/etc/openclash/backup/config.yaml"
   else
      CONFIG_FILE="/etc/openclash/config/$name.yaml"
      BACKPACK_FILE="/etc/openclash/backup/$name.yaml"
   fi

   if [ -n "$2" ] && [ "$2" != "$CONFIG_FILE" ] && [ "$2" != "$name" ]; then
      return
   fi
   
   if [ ! -z "$keyword" ] || [ ! -z "$ex_keyword" ]; then
      config_list_foreach "$section" "keyword" server_key_match "keyword"
      config_list_foreach "$section" "ex_keyword" server_key_match "ex_keyword"
   fi
   
   if [ -n "$de_ex_keyword" ]; then
      for i in $de_ex_keyword;
      do
      	if [ -z "$key_ex_match_param" ]; then
      	   key_ex_match_param="($i)"
      	else
      	   key_ex_match_param="$key_ex_match_param|($i)"
        fi
      done
   fi
         
   if [ "$sub_convert" -eq 0 ]; then
      subscribe_url=$address
   elif [ "$sub_convert" -eq 1 ] && [ -n "$template" ]; then
      while read line
      do
      	subscribe_url=$([ -n "$subscribe_url" ] && echo "$subscribe_url|")$(urlencode "$line")
      done < <(echo "$address")
      if [ "$template" != "0" ]; then
         template_path=$(grep "^$template," /usr/share/openclash/res/sub_ini.list |awk -F ',' '{print $3}' 2>/dev/null)
      else
         template_path=$custom_template_url
      fi
      if [ -n "$template_path" ]; then
         config_list_foreach "$section" "custom_params" convert_custom_param
         template_path_encode=$(urlencode "$template_path")
         [ -n "$key_match_param" ] && key_match_param="$(urlencode "(?i)$key_match_param")"
         [ -n "$key_ex_match_param" ] && key_ex_match_param="$(urlencode "(?i)$key_ex_match_param")"
         subscribe_url_param="?target=clash&new_name=true&url=$subscribe_url&config=$template_path_encode&include=$key_match_param&exclude=$key_ex_match_param&emoji=$emoji&list=false&sort=$sort$udp&scv=$skip_cert_verify&append_type=$node_type&fdn=true$rule_provider$append_custom_params"
         c_address="$convert_address"
      else
         subscribe_url=$address
      fi
   else
      subscribe_url=$address
   fi

   LOG_OUT "Start Updating Config File【$name】..."

   config_download
   if [ "${PIPESTATUS[0]}" -eq 0 ] && [ -s "$CFG_FILE" ]; then
      #prevent ruby unexpected error
      sed -i -E 's/protocol-param: ([^,'"'"'"''}( *#)\n\r]+)/protocol-param: "\1"/g' "$CFG_FILE" 2>/dev/null
      sed -i '/^ \{0,\}enhanced-mode:/d' "$CFG_FILE" >/dev/null 2>&1
      config_test
      if [ $? -ne 0 ]; then
         LOG_OUT "Error: Config File Tested Faild, Please Check The Log Infos!"
         LOG_OUT "Error: Config File【$name】Subscribed Failed, Trying to Download Without Agent..."
         config_download_direct
         return
      fi
      ruby -ryaml -rYAML -I "/usr/share/openclash" -E UTF-8 -e "
      begin
      YAML.load_file('$CFG_FILE');
      rescue Exception => e
      YAML.LOG('Error: Unable To Parse Config File,【' + e.message + '】');
      system 'rm -rf ${CFG_FILE} 2>/dev/null'
      end
      " 2>/dev/null >> $LOG_FILE
      if [ $? -ne 0 ]; then
         LOG_OUT "Error: Ruby Works Abnormally, Please Check The Ruby Library Depends!"
         only_download=1
         config_su_check
      elif [ ! -f "$CFG_FILE" ]; then
         LOG_OUT "Config File Format Validation Failed, Trying To Download Without Agent..."
         config_download_direct
      elif ! "$(ruby_read "$CFG_FILE" ".key?('proxies')")" && ! "$(ruby_read "$CFG_FILE" ".key?('proxy-providers')")" ; then
            LOG_OUT "Error: Updated Config【$name】Has No Proxy Field, Trying To Download Without Agent..."
            config_download_direct
      else
         config_su_check
      fi
   else
      LOG_OUT "Error: Config File【$name】Subscribed Failed, Trying to Download Without Agent..."
      config_download_direct
   fi
}

#分别获取订阅信息进行处理
config_load "openclash"
config_foreach sub_info_get "config_subscribe" "$1"
uci -q delete openclash.config.config_update_path
uci commit openclash

if [ "$restart" -eq 1 ] && [ "$(unify_ps_prevent)" -eq 0 ]; then
   /etc/init.d/openclash restart >/dev/null 2>&1 &
elif [ "$restart" -eq 0 ] && [ "$(unify_ps_prevent)" -eq 0 ] && [ "$(uci -q get openclash.config.restart)" -eq 1 ]; then
   /etc/init.d/openclash restart >/dev/null 2>&1 &
   uci -q set openclash.config.restart=0
   uci -q commit openclash
elif [ "$restart" -eq 1 ] && [ "$(unify_ps_prevent)" -eq 0 ]; then
   uci -q set openclash.config.restart=1
   uci -q commit openclash
else
   sed -i '/openclash.sh/d' $CRON_FILE 2>/dev/null
   [ "$(uci -q get openclash.config.auto_update)" -eq 1 ] && [ "$(uci -q get openclash.config.config_auto_update_mode)" -ne 1 ] && echo "0 $(uci -q get openclash.config.auto_update_time) * * $(uci -q get openclash.config.config_update_week_time) /usr/share/openclash/openclash.sh" >> $CRON_FILE
   /etc/init.d/cron restart
fi
del_lock
