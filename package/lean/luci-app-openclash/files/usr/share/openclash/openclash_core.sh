#!/bin/sh
#禁止多个实例
status=$(ps|grep -c /usr/share/openclash/openclash_core.sh)
[ "$status" -gt "3" ] && exit 0

START_LOG="/tmp/openclash_start.log"
LOGTIME=$(date "+%Y-%m-%d %H:%M:%S")
LOG_FILE="/tmp/openclash.log"
CPU_MODEL=$(uci get openclash.config.core_version 2>/dev/null)
   
if [ "$(/etc/openclash/clash -v 2>/dev/null |awk -F ' ' '{print $2}')" != "$(sed -n 1p /tmp/clash_last_version 2>/dev/null)" ] || [ -z "$(/etc/openclash/clash -v 2>/dev/null |awk -F ' ' '{print $2}')" ] || [ ! -f /etc/openclash/clash ]; then
   if [ "$CPU_MODEL" != 0 ]; then
   echo "开始下载 OpenClash 内核..." >$START_LOG
   wget-ssl --no-check-certificate --quiet --timeout=10 --tries=5 https://github.com/vernesong/OpenClash/releases/download/Clash/clash-"$CPU_MODEL".tar.gz -O /tmp/clash.tar.gz
   if [ "$?" -eq "0" ] && [ "$(ls -l /tmp/clash.tar.gz |awk '{print int($5/1024)}')" -ne 0 ]; then
      tar zxvf /tmp/clash.tar.gz -C /tmp >/dev/null 2>&1\
      && rm -rf /tmp/clash.tar.gz >/dev/null 2>&1\
      && chmod 4755 /tmp/clash\
      && chown root:root /tmp/clash
      /etc/init.d/openclash stop
      echo "OpenClash 内核下载成功，开始更新..." >$START_LOG\
      && rm -rf /etc/openclash/clash\
      && mv /tmp/clash /etc/openclash/clash >/dev/null 2>&1
      if [ "$?" -eq "0" ]; then
         /etc/init.d/openclash start
         echo "核心程序更新成功！" >$START_LOG
         echo "${LOGTIME} OpenClash Core Update Successful" >>$LOG_FILE
         sleep 5
         echo "" >$START_LOG
      else
         echo "核心程序更新失败，请确认设备闪存空间足够后再试！" >$START_LOG
         echo "${LOGTIME} OpenClash Core Update Error" >>$LOG_FILE
         sleep 5
         echo "" >$START_LOG
      fi
   else
      echo "核心程序下载失败，请检查网络或稍后再试！" >$START_LOG
      rm -rf /tmp/clash.tar.gz >/dev/null 2>&1
      echo "${LOGTIME} OpenClash Core Update Error" >>$LOG_FILE
      sleep 10
      echo "" >$START_LOG
   fi
   else
      echo "未选择编译版本，请到全局设置中选择后再试！" >$START_LOG
      sleep 10
      echo "" >$START_LOG
   fi
else
      echo "核心程序没有更新，停止继续操作！" >$START_LOG
      echo "${LOGTIME} OpenClash Core No Change, Do Nothing" >>$LOG_FILE
      rm -rf /tmp/clash >/dev/null 2>&1
      sleep 5
      echo "" >$START_LOG
fi