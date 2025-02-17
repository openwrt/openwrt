#!/bin/bash
. /usr/share/openclash/log.sh

set_lock() {
   exec 878>"/tmp/lock/openclash_update.lock" 2>/dev/null
   flock -x 878 2>/dev/null
}

del_lock() {
   flock -u 878 2>/dev/null
   rm -rf "/tmp/lock/openclash_update.lock" 2>/dev/null
}

set_lock

if [ -n "$1" ] && [ "$1" != "one_key_update" ]; then
   [ ! -f "/tmp/openclash_last_version" ] && /usr/share/openclash/openclash_version.sh "$1" 2>/dev/null
elif [ -n "$2" ]; then
   [ ! -f "/tmp/openclash_last_version" ] && /usr/share/openclash/openclash_version.sh "$2" 2>/dev/null
else
   [ ! -f "/tmp/openclash_last_version" ] && /usr/share/openclash/openclash_version.sh 2>/dev/null
fi

if [ ! -f "/tmp/openclash_last_version" ]; then
   LOG_OUT "Error: Failed to Get Version Information, Please Try Again Later..."
   SLOG_CLEAN
   del_lock
   exit 0
fi

LAST_OPVER="/tmp/openclash_last_version"
LAST_VER=$(sed -n 1p "$LAST_OPVER" 2>/dev/null |sed "s/^v//g" |tr -d "\n")
if [ -x "/bin/opkg" ]; then
   OP_CV=$(rm -f /var/lock/opkg.lock && opkg status luci-app-openclash 2>/dev/null |grep 'Version' |awk -F 'Version: ' '{print $2}' |awk -F '.' '{print $2$3}' 2>/dev/null)
elif [ -x "/usr/bin/apk" ]; then
   OP_CV=$(apk list luci-app-openclash 2>/dev/null|grep 'installed' | grep -oE '[0-9]+(\.[0-9]+)*' | head -1 |awk -F '.' '{print $2$3}' 2>/dev/null)
fi
OP_LV=$(sed -n 1p "$LAST_OPVER" 2>/dev/null |awk -F 'v' '{print $2}' |awk -F '.' '{print $2$3}' 2>/dev/null)
RELEASE_BRANCH=$(uci -q get openclash.config.release_branch || echo "master")
LOG_FILE="/tmp/openclash.log"
github_address_mod=$(uci -q get openclash.config.github_address_mod || echo 0)

#一键更新
if [ "$1" = "one_key_update" ]; then
   uci -q set openclash.config.enable=1
   uci -q commit openclash
   if [ "$github_address_mod" = "0" ] && [ -z "$2" ]; then
      LOG_OUT "Tip: If the download fails, try setting the CDN in Overwrite Settings - General Settings - Github Address Modify Options"
   fi
   if [ -n "$2" ]; then
      /usr/share/openclash/openclash_core.sh "Meta" "$1" "$2" >/dev/null 2>&1 &
      github_address_mod="$2"
   else
      /usr/share/openclash/openclash_core.sh "Meta" "$1" >/dev/null 2>&1 &
   fi
   
   wait
else
   if [ "$github_address_mod" = "0" ]; then
      LOG_OUT "Tip: If the download fails, try setting the CDN in Overwrite Settings - General Settings - Github Address Modify Options"
   fi
fi

if [ -n "$OP_CV" ] && [ -n "$OP_LV" ] && [ "$(expr "$OP_LV" \> "$OP_CV")" -eq 1 ] && [ -f "$LAST_OPVER" ]; then
   LOG_OUT "Start Downloading【OpenClash - v$LAST_VER】..."
   if [ "$github_address_mod" != "0" ]; then
      if [ "$github_address_mod" == "https://cdn.jsdelivr.net/" ] || [ "$github_address_mod" == "https://fastly.jsdelivr.net/" ] || [ "$github_address_mod" == "https://testingcf.jsdelivr.net/" ]; then
         if [ -x "/bin/opkg" ]; then
            curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 "$github_address_mod"gh/vernesong/OpenClash@package/"$RELEASE_BRANCH"/luci-app-openclash_"$LAST_VER"_all.ipk -o /tmp/openclash.ipk 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="/tmp/openclash.ipk" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
         elif [ -x "/usr/bin/apk" ]; then
            curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 "$github_address_mod"gh/vernesong/OpenClash@package/"$RELEASE_BRANCH"/luci-app-openclash-"$LAST_VER".apk -o /tmp/openclash.apk 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="/tmp/openclash.apk" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
         fi
      else
         if [ -x "/bin/opkg" ]; then
            curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 "$github_address_mod"https://raw.githubusercontent.com/vernesong/OpenClash/package/"$RELEASE_BRANCH"/luci-app-openclash_"$LAST_VER"_all.ipk -o /tmp/openclash.ipk 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="/tmp/openclash.ipk" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
         elif [ -x "/usr/bin/apk" ]; then
            curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 "$github_address_mod"https://raw.githubusercontent.com/vernesong/OpenClash/package/"$RELEASE_BRANCH"/luci-app-openclash-"$LAST_VER".apk -o /tmp/openclash.apk 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="/tmp/openclash.apk" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
         fi
      fi
   else
      if [ -x "/bin/opkg" ]; then
         curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 https://raw.githubusercontent.com/vernesong/OpenClash/package/"$RELEASE_BRANCH"/luci-app-openclash_"$LAST_VER"_all.ipk -o /tmp/openclash.ipk 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="/tmp/openclash.ipk" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
      elif [ -x "/usr/bin/apk" ]; then
         curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 https://raw.githubusercontent.com/vernesong/OpenClash/package/"$RELEASE_BRANCH"/luci-app-openclash-"$LAST_VER".apk -o /tmp/openclash.apk 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="/tmp/openclash.apk" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
      fi
   fi

   if [ "${PIPESTATUS[0]}" -eq 0 ]; then
      LOG_OUT "【OpenClash - v$LAST_VER】Download Successful, Start Pre Update Test..."
      if [ -x "/bin/opkg" ]; then
         if [ -s "/tmp/openclash.ipk" ]; then
            if [ -z "$(opkg install /tmp/openclash.ipk --noaction 2>/dev/null |grep 'Upgrading luci-app-openclash on root' 2>/dev/null)" ]; then
               LOG_OUT "【OpenClash - v$LAST_VER】Pre Update Test Failed, The File is Saved in /tmp/openclash.ipk, Please Try to Update Manually With【opkg install /tmp/openclash.ipk】"
               if [ "$(uci -q get openclash.config.restart)" -eq 1 ]; then
                  uci -q set openclash.config.restart=0
                  uci -q commit openclash
                  /etc/init.d/openclash restart >/dev/null 2>&1 &
               else
                  SLOG_CLEAN
               fi
               del_lock
               exit 0
            fi
         fi
      elif [ -x "/usr/bin/apk" ]; then
         if [ -s "/tmp/openclash.apk" ]; then
            apk update >/dev/null 2>&1
            apk add -s -q --force-overwrite --clean-protected --allow-untrusted /tmp/openclash.apk >/dev/null 2>&1
            if [ "$?" != "0" ]; then
               LOG_OUT "【OpenClash - v$LAST_VER】Pre Update Test Failed, The File is Saved in /tmp/openclash.apk, Please Try to Update Manually With【apk add -q --force-overwrite --clean-protected --allow-untrusted /tmp/openclash.apk】"
               if [ "$(uci -q get openclash.config.restart)" -eq 1 ]; then
                  uci -q set openclash.config.restart=0
                  uci -q commit openclash
                  /etc/init.d/openclash restart >/dev/null 2>&1 &
               else
                  SLOG_CLEAN
               fi
               del_lock
               exit 0
            fi
         fi
      fi
      LOG_OUT "【OpenClash - v$LAST_VER】Pre Update Test Passed, Ready to Update and Please Do not Refresh The Page and Other Operations..."
      cat > /tmp/openclash_update.sh <<"EOF"
#!/bin/sh
START_LOG="/tmp/openclash_start.log"
LOG_FILE="/tmp/openclash.log"
LOGTIME=$(date "+%Y-%m-%d %H:%M:%S")
		
LOG_OUT()
{
	if [ -n "${1}" ]; then
		echo -e "${1}" > $START_LOG
		echo -e "${LOGTIME} ${1}" >> $LOG_FILE
	fi
}

SLOG_CLEAN()
{
	echo "" > $START_LOG
}

uci -q set openclash.config.enable=0
uci -q commit openclash
if [ -x "/bin/opkg" ]; then
   LOG_OUT "Uninstalling The Old Version, Please Do not Refresh The Page or Do Other Operations..."
   opkg remove --force-depends --force-remove luci-app-openclash
fi
LOG_OUT "Installing The New Version, Please Do Not Refresh The Page or Do Other Operations..."
if [ -x "/bin/opkg" ]; then
   opkg install /tmp/openclash.ipk
elif [ -x "/usr/bin/apk" ]; then
   apk add -q --force-overwrite --clean-protected --allow-untrusted /tmp/openclash.apk
fi
if [ -x "/bin/opkg" ]; then
   if [ "$?" != "0" ] || [ -z "$(opkg info *openclash |grep Installed-Time)" ]; then
      opkg install /tmp/openclash.ipk
   fi
   if [ "$?" == "0" ] && [ -n "$(opkg info *openclash |grep Installed-Time)" ]; then
      rm -rf /tmp/openclash.ipk >/dev/null 2>&1
      LOG_OUT "OpenClash Update Successful, About To Restart!"
      uci -q set openclash.config.enable=1
      uci -q commit openclash
      /etc/init.d/openclash restart 2>/dev/null
   else
      LOG_OUT "OpenClash Update Failed, The File is Saved in /tmp/openclash.ipk, Please Try to Update Manually With【opkg install /tmp/openclash.ipk】"
      SLOG_CLEAN
   fi
elif [ -x "/usr/bin/apk" ]; then
   if [ "$?" != "0" ] || [ -z "$(apk list luci-app-openclash 2>/dev/null |grep 'installed')" ]; then
      apk add -q --force-overwrite --clean-protected --allow-untrusted /tmp/openclash.apk
   fi
   if [ "$?" == "0" ] || [ -n "$(apk list luci-app-openclash 2>/dev/null |grep 'installed')" ]; then
      rm -rf /tmp/openclash.apk >/dev/null 2>&1
      LOG_OUT "OpenClash Update Successful, About To Restart!"
      uci -q set openclash.config.enable=1
      uci -q commit openclash
      /etc/init.d/openclash restart 2>/dev/null
   else
      LOG_OUT "OpenClash Update Failed, The File is Saved in /tmp/openclash.apk, Please Try to Update Manually With【apk add -q --force-overwrite --clean-protected --allow-untrusted /tmp/openclash.apk】"
      SLOG_CLEAN
   fi
fi
EOF
   chmod 4755 /tmp/openclash_update.sh
   /tmp/openclash_update.sh &
   wait
   rm -rf /tmp/openclash_update.sh
   else
      LOG_OUT "【OpenClash - v$LAST_VER】Download Failed, Please Check The Network or Try Again Later!"
      rm -rf /tmp/openclash.ipk >/dev/null 2>&1
      rm -rf /tmp/openclash.apk >/dev/null 2>&1
      if [ "$(uci -q get openclash.config.restart)" -eq 1 ]; then
         uci -q set openclash.config.restart=0
         uci -q commit openclash
         /etc/init.d/openclash restart >/dev/null 2>&1 &
      else
         SLOG_CLEAN
      fi
   fi
else
   if [ ! -f "$LAST_OPVER" ] || [ -z "$OP_CV" ] || [ -z "$OP_LV" ]; then
      LOG_OUT "Error: Failed to Get Version Information, Please Try Again Later..."
   else
      LOG_OUT "OpenClash Has not Been Updated, Stop Continuing!"
   fi
   if [ "$(uci -q get openclash.config.restart)" -eq 1 ]; then
      uci -q set openclash.config.restart=0
      uci -q commit openclash
      /etc/init.d/openclash restart >/dev/null 2>&1 &
   else
      SLOG_CLEAN
   fi
fi
del_lock
