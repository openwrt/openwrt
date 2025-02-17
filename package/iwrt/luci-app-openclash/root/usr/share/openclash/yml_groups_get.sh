#!/bin/bash
. /lib/functions.sh
. /usr/share/openclash/log.sh

set_lock() {
   exec 876>"/tmp/lock/openclash_groups_get.lock" 2>/dev/null
   flock -x 876 2>/dev/null
}

del_lock() {
   flock -u 876 2>/dev/null
   rm -rf "/tmp/lock/openclash_groups_get.lock"
}

CFG_FILE="/etc/config/openclash"
other_group_file="/tmp/yaml_other_group.yaml"
servers_update=$(uci -q get openclash.config.servers_update)
servers_if_update=$(uci -q get openclash.config.servers_if_update)
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

LOG_OUT "Start Getting【$CONFIG_NAME】Groups Setting..."

/usr/share/openclash/yml_groups_name_get.sh
if [ $? -ne 0 ]; then
	LOG_OUT "Read Error, Config File【$CONFIG_NAME】Abnormal!"
	uci -q commit openclash
	SLOG_CLEAN
	del_lock
	exit 0
fi

#判断当前配置文件是否有策略组信息
cfg_group_name()
{
   local section="$1"
   local config
   config_get "config" "$section" "config" ""

   if [ -z "$config" ]; then
      return
   fi

   [ "$config" = "$CONFIG_NAME" ] || [ "$config" = "all" ] && {
      config_group_exist=1
   }
}

#删除不必要的配置
cfg_delete()
{
   LOG_OUT "Deleting Old Configuration..."
#删除策略组
   group_num=$(grep "^config groups$" "$CFG_FILE" |wc -l)
   for ((i=$group_num;i>=0;i--))
	 do
	    if [ "$(uci -q get openclash.@groups["$i"].config)" = "$CONFIG_NAME" ]; then
	       uci -q delete openclash.@groups["$i"]
	       uci -q commit openclash
	    fi
	 done
#删除启用的节点
   server_num=$(grep "^config servers$" "$CFG_FILE" |wc -l)
   for ((i=$server_num;i>=0;i--))
	 do
	    if [ "$(uci -q get openclash.@servers["$i"].config)" = "$CONFIG_NAME" ]; then
	    	 if [ "$(uci -q get openclash.@servers["$i"].enabled)" = "1" ] && [ "$(uci -q get openclash.@servers["$i"].manual)" = "0" ]; then
	          uci -q delete openclash.@servers["$i"]
	          uci -q commit openclash
	       fi
	    fi
	 done
#删除启用的代理集
   provider_num=$(grep "^config proxy-provider$" "$CFG_FILE" 2>/dev/null |wc -l)
   for ((i=$provider_num;i>=0;i--))
	 do
	    if [ "$(uci -q get openclash.@proxy-provider["$i"].config)" = "$CONFIG_NAME" ]; then
	       if [ "$(uci -q get openclash.@proxy-provider["$i"].enabled)" = "1" ] && [ "$(uci -q get openclash.@proxy-provider["$i"].manual)" = "0" ]; then
	          uci -q delete openclash.@proxy-provider["$i"]
	          uci -q commit openclash
	       fi
	    fi
	 done
}

config_load "openclash"
config_foreach cfg_group_name "groups"

if [ "$servers_if_update" -eq 1 ] && [ "$servers_update" -eq 1 ] && [ "$config_group_exist" -eq 1 ]; then
   /usr/share/openclash/yml_proxys_get.sh
   del_lock
   exit 0
fi

cfg_delete

ruby -ryaml -rYAML -I "/usr/share/openclash" -E UTF-8 -e "
   begin
      Value = YAML.load_file('$CONFIG_FILE');
   rescue Exception => e
      YAML.LOG('Error: Load File Failed,【' + e.message + '】');
   end;

   threads_g = [];
   threadsp = [];
   threads_uci = [];
   uci_commands = [];

   if not Value.key?('proxy-groups') or Value['proxy-groups'].nil? then
      proxy-groups = [];
   end;

	Value_1 = File.readlines('/tmp/Proxy_Group').map!{|x| x.strip};
   Value['proxy-groups'].each do |x|
      threadsp << Thread.new {
      begin
         next unless x['name'] && x['type'];
         uci_name_tmp=%x{uci -q add openclash groups 2>&1}.chomp
         uci_set='uci -q set openclash.' + uci_name_tmp + '.'
         uci_add='uci -q add_list openclash.' + uci_name_tmp + '.'

         YAML.LOG('Start Getting【${CONFIG_NAME} - ' + x['type'].to_s + ' - ' + x['name'].to_s + '】Group Setting...');

         threads_g << Thread.new {
            #name
            if x.key?('name') then
               uci_commands << uci_set + 'name=\"' + x['name'].to_s + '\"'
            end;
         };

         threads_g << Thread.new {
            #type
            if x.key?('type') then
               uci_commands << uci_set + 'type=\"' + x['type'].to_s + '\"'
            end;
         };

         threads_g << Thread.new {
            #enabled
            uci_commands << uci_set + 'enabled=\"1\"'
            #config
            uci_commands << uci_set + 'config=\"' + '${CONFIG_NAME}' + '\"'
            #old_name
            uci_commands << uci_set + 'old_name=\"' + x['name'] + '\"'
            #old_name_cfg
            uci_commands << uci_set + 'old_name_cfg=\"' + x['name'] + '\"'
         };

         threads_g << Thread.new {
            #strategy
            if x.key?('strategy') then
               uci_commands << uci_set + 'strategy=\"' + x['strategy'].to_s + '\"'
            end;
         };

         threads_g << Thread.new {
            #disable-udp
            if x.key?('disable-udp') then
               uci_commands << uci_set + 'disable_udp=\"' + x['disable-udp'].to_s + '\"'
            end;
         };

         threads_g << Thread.new {
            if x['type'] == 'url-test' or x['type'] == 'fallback' or x['type'] == 'load-balance' then
               #test_url
               if x.key?('url') then
                  uci_commands << uci_set + 'test_url=\"' + x['url'].to_s + '\"'
               end;

               #test_interval
               if x.key?('interval') then
                  uci_commands << uci_set + 'test_interval=\"' + x['interval'].to_s + '\"'
               end;

               #test_tolerance
               if x['type'] == 'url-test' then
                  if x.key?('tolerance') then
                     uci_commands << uci_set + 'tolerance=\"' + x['tolerance'].to_s + '\"'
                  end;
               end;
            end;
         };

         threads_g << Thread.new {
            #Policy Filter
            if x.key?('filter') then
               uci_commands << uci_set + 'policy_filter=\"' + x['filter'].to_s + '\"'
            end
         };
         
         threads_g << Thread.new {
            #other_group
            if x.key?('proxies') then 
               x['proxies'].each{
               |y|
                  if Value_1.include?(y) then
                     commands = uci_add + 'other_group=\"^' + y.to_s + '$\"'
                     system(commands)
                  end
               }
            end
         };
         threads_g.each(&:join);
      rescue Exception => e
         YAML.LOG('Error: Resolve Groups Failed,【${CONFIG_NAME} - ' + x['type'] + ' - ' + x['name'] + ': ' + e.message + '】');
      end;
      };
   end;
   threadsp.each(&:join);
   batch_size = 30;
   (0...uci_commands.length).step(batch_size) do |i|
      threads_uci << Thread.new{
         system(uci_commands[i, batch_size].join('; '));
      };
   end;
   threads_uci.each(&:join);
   system('uci -q commit openclash');
   system('rm -rf /tmp/yaml_other_group.yaml 2>/dev/null');
" 2>/dev/null >> $LOG_FILE

/usr/share/openclash/yml_proxys_get.sh
del_lock