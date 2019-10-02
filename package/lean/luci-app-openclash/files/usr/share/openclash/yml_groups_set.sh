#!/bin/sh /etc/rc.common
status=$(ps|grep -c /usr/share/openclash/yml_groups_set.sh)
[ "$status" -gt "3" ] && exit 0

START_LOG="/tmp/openclash_start.log"
GROUP_FILE="/tmp/yaml_groups.yaml"
CONFIG_FILE="/etc/openclash/config.yaml"
CFG_FILE="/etc/config/openclash"

#加入节点
yml_servers_add()
{
	
	local section="$1"
	config_get "name" "$section" "name" ""
	config_list_foreach "$section" "groups" set_groups "$name" "$2"
	
}

set_groups()
{

	if [ "$1" = "$3" ]; then
	   echo "  - \"${2}\"" >>$GROUP_FILE
	fi

}

set_other_groups()
{

   echo "  - ${1}" >>$GROUP_FILE

}


#创建策略组
yml_groups_set()
{

   local section="$1"
   config_get "type" "$section" "type" ""
   config_get "name" "$section" "name" ""
   config_get "old_name" "$section" "old_name" ""
   config_get "test_url" "$section" "test_url" ""
   config_get "test_interval" "$section" "test_interval" ""

   if [ -z "$type" ]; then
      return
   fi
   
   if [ -z "$name" ]; then
      return
   fi
   
   if [ -z "$test_url" ] || [ -z "$test_interval" ] && [ "$type" != "select" ]; then
      return
   fi
   
   echo "正在写入【$type】-【$name】策略组到配置文件..." >$START_LOG
   
   echo "- name: $name" >>$GROUP_FILE
   echo "  type: $type" >>$GROUP_FILE
   echo "  proxies:" >>$GROUP_FILE
   
   #名字变化时处理规则部分
   if [ "$name" != "$old_name" ]; then
      sed -i "s/,${old_name}$/,${name}#d/g" $CONFIG_FILE 2>/dev/null
      sed -i "s/:${old_name}$/:${name}#d/g" $CONFIG_FILE 2>/dev/null
      sed -i "s/\'${old_name}\'/\'${name}\'/g" $CFG_FILE 2>/dev/null
      config_load "openclash"
   fi
   
   config_list_foreach "$section" "other_group" set_other_groups #加入其他策略组
   config_foreach yml_servers_add "servers" "$name" #加入服务器节点
   
   [ ! -z "$test_url" ] && {
   	echo "  url: $test_url" >>$GROUP_FILE
   }
   [ ! -z "$test_interval" ] && {
   echo "  interval: \"$test_interval\"" >>$GROUP_FILE
   }
}

create_config=$(uci get openclash.config.create_config 2>/dev/null)
if [ "$create_config" = "0" ]; then

   if [ -z "$(grep "^ \{0,\}Proxy:" /etc/openclash/config.yaml)" ] || [ -z "$(grep "^ \{0,\}Proxy Group:" /etc/openclash/config.yaml)" ]; then
      echo "配置文件不完整，开始使用ConnersHua规则创建..." >$START_LOG
      uci set openclash.config.create_config=1
      uci set openclash.config.rule_sources="ConnersHua"
      uci set openclash.config.rule_source="ConnersHua"
      uci commit openclash
   else
      echo "开始更新配置文件策略组信息..." >$START_LOG
      config_load "openclash"
      config_foreach yml_groups_set "groups"
      sed -i "s/#d$//g" $CONFIG_FILE 2>/dev/null
      echo "Rule:" >>$GROUP_FILE
   fi

fi
echo "配置文件策略组写入完成！" >$START_LOG
