#!/bin/sh

if [ -f "/etc/openclash/config.yaml" ]; then
   awk '/Proxy Group:/,/Rule:/{print}' /etc/openclash/config.yaml 2>/dev/null |sed "s/\'//g" 2>/dev/null |sed 's/\"//g' 2>/dev/null |sed 's/\t/ /g' 2>/dev/null |grep name: |awk -F 'name:' '{print $2}' |sed 's/,.*//' |sed 's/^ \{0,\}//' 2>/dev/null |sed 's/ \{0,\}$//' 2>/dev/null |sed 's/ \{0,\}\}\{0,\}$//g' 2>/dev/null >/tmp/Proxy_Group 2>&1
   echo 'DIRECT' >>/tmp/Proxy_Group
   echo 'REJECT' >>/tmp/Proxy_Group
else
   echo '读取错误，配置文件异常！' >/tmp/Proxy_Group
   exit 0
fi