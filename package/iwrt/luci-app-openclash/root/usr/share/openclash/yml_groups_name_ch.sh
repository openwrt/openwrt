#!/bin/bash
. /lib/functions.sh

set_lock() {
   exec 882>"/tmp/lock/openclash_name_ch.lock" 2>/dev/null
   flock -x 882 2>/dev/null
}

del_lock() {
   flock -u 882 2>/dev/null
   rm -rf "/tmp/lock/openclash_name_ch.lock"
}

cfg_groups_set()
{

   CFG_FILE="/etc/config/openclash"
   local section="$1"
   local name old_name_cfg old_name
   config_get "name" "$section" "name" ""
   config_get "old_name_cfg" "$section" "old_name_cfg" ""
   config_get "old_name" "$section" "old_name" ""

   if [ -z "$name" ]; then
      return
   fi
   
   if [ -z "$old_name_cfg" ]; then
      uci -q set openclash."$section".old_name_cfg="$name"
      uci -q commit openclash
   fi
   
   if [ -z "$old_name" ]; then
      uci -q set openclash."$section".old_name="$name"
      uci -q commit openclash
   fi
   
   #名字变化时处理配置文件
   if [ "$name" != "$old_name_cfg" ] && [ ! -z "$old_name_cfg" ]; then
      convert_name=$(echo "$name" | sed 's/\//\\\//g' 2>/dev/null)
      convert_old_name_cfg=$(echo "$old_name_cfg" | sed 's/\//\\\//g' 2>/dev/null)
      sed -i "s/old_name_cfg \'${convert_old_name_cfg}\'/old_name_cfg \'${convert_name}\'/g" $CFG_FILE 2>/dev/null
      sed -i "s/groups \'${convert_old_name_cfg}\'/groups \'${convert_name}\'/g" $CFG_FILE 2>/dev/null
      sed -i "s/option group \'${convert_old_name_cfg}\'/option group \'${convert_name}\'/g" $CFG_FILE 2>/dev/null
      sed -i "s/option specific_group \'${convert_old_name_cfg}\'/option specific_group \'${convert_name}\'/g" $CFG_FILE 2>/dev/null
      sed -i "s/other_group \'${convert_old_name_cfg}\'/other_group \'${convert_name}\'/g" $CFG_FILE 2>/dev/null
      sed -i "s/new_servers_group \'${convert_old_name_cfg}\'/new_servers_group \'${convert_name}\'/g" $CFG_FILE 2>/dev/null
      sed -i "s/relay_groups \'${convert_old_name_cfg}\'/relay_groups \'${convert_name}\'/g" $CFG_FILE 2>/dev/null
      #第三方规则处理
      OTHER_RULE_NAMES=("GlobalTV" "AsianTV" "MainlandTV" "Proxy" "Youtube" "Bilibili" "Bahamut" "HBOMax" "Pornhub" "Apple" "GoogleFCM" "Scholar" "Microsoft" "Netflix" "Disney" "Spotify" "Steam" "Speedtest" "Telegram" "PayPal" "Netease_Music" "AdBlock" "Domestic" "Others" "miHoYo" "AI_Suite" "AppleTV" "Crypto" "Discord" "HTTPDNS")
      for i in ${OTHER_RULE_NAMES[@]}; do
      	sed -i "s/option ${i} \'${convert_old_name_cfg}\'/option ${i} \'${convert_name}\'/g" $CFG_FILE 2>/dev/null
      done 2>/dev/null
      config_load "openclash"
   fi

}

set_lock
config_load "openclash"
config_foreach cfg_groups_set "groups"
del_lock