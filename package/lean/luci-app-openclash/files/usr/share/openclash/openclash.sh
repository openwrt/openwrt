#!/bin/bash
status=$(ps|grep -c /usr/share/openclash/openclash.sh)
[ "$status" -gt "3" ] && exit 0

START_LOG="/tmp/openclash_start.log"
LOGTIME=$(date "+%Y-%m-%d %H:%M:%S")
CONFIG_FILE="/etc/openclash/config.yaml"
LOG_FILE="/tmp/openclash.log"
BACKPACK_FILE="/etc/openclash/config.bak"
URL_TYPE=$(uci get openclash.config.config_update_url_type 2>/dev/null)
subscribe_url=$(uci get openclash.config.subscribe_url 2>/dev/null)
en_mode=$(uci get openclash.config.en_mode 2>/dev/null)

config_dawnload()
{
if [ "$URL_TYPE" == "V2ray" ]; then
   echo "开始下载v2ray配置文件..." >$START_LOG
   subscribe_url=`echo $subscribe_url |sed 's/{/%7B/g;s/}/%7D/g;s/:/%3A/g;s/\"/%22/g;s/,/%2C/g;s/?/%3F/g;s/=/%3D/g;s/&/%26/g;s/\//%2F/g'`
   wget-ssl --no-check-certificate --quiet --timeout=10 --tries=2 https://tgbot.lbyczf.com/v2rayn2clash?url="$subscribe_url" -O /tmp/config.yaml
elif [ "$URL_TYPE" == "surge" ]; then
   echo "开始下载Surge配置文件..." >$START_LOG
   subscribe_url=`echo $subscribe_url |sed 's/{/%7B/g;s/}/%7D/g;s/:/%3A/g;s/\"/%22/g;s/,/%2C/g;s/?/%3F/g;s/=/%3D/g;s/&/%26/g;s/\//%2F/g'`
   wget-ssl --no-check-certificate --quiet --timeout=10 --tries=2 https://tgbot.lbyczf.com/surge2clash?url="$subscribe_url" -O /tmp/config.yaml
else
   echo "开始下载Clash配置文件..." >$START_LOG
   wget-ssl --no-check-certificate --quiet --timeout=10 --tries=2 "$subscribe_url" -O /tmp/config.yaml
fi
}

config_su_check()
{
   echo "配置文件下载成功，检查是否有更新..." >$START_LOG
   if [ -f "$CONFIG_FILE" ]; then
      cmp -s "$BACKPACK_FILE" /tmp/config.yaml
         if [ "$?" -ne "0" ]; then
            echo "配置文件有更新，开始替换..." >$START_LOG
            mv /tmp/config.yaml "$CONFIG_FILE" 2>/dev/null\
            && cp "$CONFIG_FILE" "$BACKPACK_FILE"\
            && echo "配置文件替换成功，开始启动 OpenClash ..." >$START_LOG\
            && echo "${LOGTIME} Config Update Successful" >>$LOG_FILE\
            && /etc/init.d/openclash restart 2>/dev/null
         else
            echo "配置文件没有任何更新，停止继续操作..." >$START_LOG
            rm -rf /tmp/config.yaml
            echo "${LOGTIME} Updated Config No Change, Do Nothing" >>$LOG_FILE
            sleep 5
            echo "" >$START_LOG
         fi
   else
      echo "配置文件下载成功，本地没有配置文件，开始创建 ..." >$START_LOG
      mv /tmp/config.yaml "$CONFIG_FILE" 2>/dev/null\
      && cp "$CONFIG_FILE" "$BACKPACK_FILE"\
      && echo "配置文件创建成功，开始启动 OpenClash ..." >$START_LOG\
      && echo "${LOGTIME} Config Update Successful" >>$LOG_FILE\
      && /etc/init.d/openclash restart 2>/dev/null
   fi
}

config_error()
{
   echo "配置文件下载失败，请检查网络或稍后再试！" >$START_LOG
   echo "${LOGTIME} Config Update Error" >>$LOG_FILE
   rm -rf /tmp/config.yaml 2>/dev/null
   sleep 5
   echo "" >$START_LOG
}

config_dawnload

if [ "$?" -eq "0" ] && [ "$(ls -l /tmp/config.yaml |awk '{print int($5/1024)}')" -ne 0 ]; then
   config_su_check
else
   if pidof clash >/dev/null; then
      echo "配置文件下载失败，尝试不使用代理下载配置文件..." >$START_LOG
      if [ "$en_mode" = "fake-ip"]; then
         resolve_sub_ips=$(nslookup "$subscribe_url" 114.114.114.114 |grep Address |awk -F ': ' '{print $2}')
      else
         resolve_sub_ips=$(nslookup "$subscribe_url" 127.0.0.1 |grep Address |awk -F ': ' '{print $2}')
      fi
      
      for resolve_sub_ip in $resolve_sub_ips; do
        iptables -t nat -I openclash -d "$resolve_sub_ip" -j RETURN >/dev/null 2>&1
      done
      
      config_dawnload
      
      if [ "$?" -eq "0" ] && [ "$(ls -l /tmp/config.yaml |awk '{print int($5/1024)}')" -ne 0 ]; then
         config_su_check
      else
         config_error
      fi
      
      for resolve_sub_ip in $resolve_sub_ips; do
        iptables -t nat -D openclash -d "$resolve_sub_ip" -j RETURN >/dev/null 2>&1
      done
   else
      config_error
   fi
fi
