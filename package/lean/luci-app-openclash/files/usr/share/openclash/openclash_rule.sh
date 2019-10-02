#!/bin/sh
   START_LOG="/tmp/openclash_start.log"
   LOGTIME=$(date "+%Y-%m-%d %H:%M:%S")
   LOG_FILE="/tmp/openclash.log"
   echo "开始获取使用中的第三方规则名称..." >$START_LOG
   rule_source=$(uci get openclash.config.rule_source 2>/dev/null)
   echo "开始下载使用中的第三方规则..." >$START_LOG
      if [ "$rule_source" = "lhie1" ]; then
         wget-ssl --no-check-certificate --quiet --timeout=10 --tries=2 https://raw.githubusercontent.com/lhie1/Rules/master/Clash/Rule.yml -O /tmp/rules.yaml
      elif [ "$rule_source" = "ConnersHua" ]; then
         wget-ssl --no-check-certificate --quiet --timeout=10 --tries=2 https://raw.githubusercontent.com/ConnersHua/Profiles/master/Clash/Pro.yaml -O /tmp/rules.yaml
         sed -i -n '/^Rule:/,$p' /tmp/rules.yaml
      elif [ "$rule_source" = "ConnersHua_return" ]; then
         wget-ssl --no-check-certificate --quiet --timeout=10 --tries=2 https://raw.githubusercontent.com/ConnersHua/Profiles/master/Clash/BacktoCN.yaml -O /tmp/rules.yaml
         sed -i -n '/^Rule:/,$p' /tmp/rules.yaml
      fi
   if [ "$?" -eq "0" ] && [ "$rule_source" != 0 ] && [ "$(ls -l /tmp/rules.yaml |awk '{print int($5/1024)}')" -ne 0 ]; then
      echo "下载成功，开始预处理规则文件..." >$START_LOG
      sed -i "/^Rule:/a\##source:${rule_source}" /tmp/rules.yaml >/dev/null 2>&1
      echo "检查下载的规则文件是否有更新..." >$START_LOG
      cmp -s /etc/openclash/"$rule_source".yaml /tmp/rules.yaml
      if [ "$?" -ne "0" ]; then
         echo "检测到下载的规则文件有更新，开始替换..." >$START_LOG
         mv /tmp/rules.yaml /etc/openclash/"$rule_source".yaml >/dev/null 2>&1
         sed -i '/^Rule:/a\##updated' /etc/openclash/"$rule_source".yaml >/dev/null 2>&1
         echo "替换成功，重新加载 OpenClash 应用新规则..." >$START_LOG
         /etc/init.d/openclash reload 2>/dev/null
         echo "${LOGTIME} Other Rules Update Successful" >>$LOG_FILE
      else
         echo "检测到下载的规则文件没有更新，停止继续操作..." >$START_LOG
         rm -rf /tmp/rules.yaml >/dev/null 2>&1
         echo "${LOGTIME} Updated Other Rules No Change, Do Nothing" >>$LOG_FILE
         sleep 10
         echo "" >$START_LOG
      fi
   elif [ "$rule_source" = 0 ]; then
      echo "未启用第三方规则，更新程序终止！" >$START_LOG
      rm -rf /tmp/rules.yaml >/dev/null 2>&1
      echo "${LOGTIME} Other Rules Not Enable, Update Stop" >>$LOG_FILE
      sleep 10
      echo "" >$START_LOG
   else
      echo "第三方规则下载失败，请检查网络或稍后再试！" >$START_LOG
      rm -rf /tmp/rules.yaml >/dev/null 2>&1
      echo "${LOGTIME} Other Rules Update Error" >>$LOG_FILE
      sleep 10
      echo "" >$START_LOG
   fi