#!/bin/bash
set_lock() {
   exec 869>"/tmp/lock/openclash_version.lock" 2>/dev/null
   flock -x 869 2>/dev/null
}

del_lock() {
   flock -u 869 2>/dev/null
   rm -rf "/tmp/lock/openclash_version.lock" 2>/dev/null
}

set_lock

TIME=$(date "+%Y-%m-%d-%H")
CHTIME=$(date "+%Y-%m-%d-%H" -r "/tmp/openclash_last_version" 2>/dev/null)
LAST_OPVER="/tmp/openclash_last_version"
RELEASE_BRANCH=$(uci -q get openclash.config.release_branch || echo "master")
if [ -x "/bin/opkg" ]; then
   OP_CV=$(rm -f /var/lock/opkg.lock && opkg status luci-app-openclash 2>/dev/null |grep 'Version' |awk -F 'Version: ' '{print $2}' |awk -F '.' '{print $2$3}' 2>/dev/null)
elif [ -x "/usr/bin/apk" ]; then
   OP_CV=$(apk list luci-app-openclash 2>/dev/null|grep 'installed' | grep -oE '[0-9]+(\.[0-9]+)*' | head -1 |awk -F '.' '{print $2$3}' 2>/dev/null)
fi
OP_LV=$(sed -n 1p $LAST_OPVER 2>/dev/null |awk -F 'v' '{print $2}' |awk -F '.' '{print $2$3}' 2>/dev/null)
github_address_mod=$(uci -q get openclash.config.github_address_mod || echo 0)
if [ -n "$1" ]; then
   github_address_mod="$1"
fi
LOG_FILE="/tmp/openclash.log"

if [ "$TIME" != "$CHTIME" ]; then
	if [ "$github_address_mod" != "0" ]; then
      if [ "$github_address_mod" == "https://cdn.jsdelivr.net/" ] || [ "$github_address_mod" == "https://fastly.jsdelivr.net/" ] || [ "$github_address_mod" == "https://testingcf.jsdelivr.net/" ]; then
         curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 "$github_address_mod"gh/vernesong/OpenClash@package/"$RELEASE_BRANCH"/version -o $LAST_OPVER 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="$LAST_OPVER" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
      else
         curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 "$github_address_mod"https://raw.githubusercontent.com/vernesong/OpenClash/package/"$RELEASE_BRANCH"/version -o $LAST_OPVER 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="$LAST_OPVER" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
      fi
   else
      curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 https://raw.githubusercontent.com/vernesong/OpenClash/package/"$RELEASE_BRANCH"/version -o $LAST_OPVER 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="$LAST_OPVER" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
   fi

   if [ "${PIPESTATUS[0]}" -eq 0 ] && [ -z "$(cat $LAST_OPVER |grep '<html>')" ]; then
   	OP_LV=$(sed -n 1p $LAST_OPVER 2>/dev/null |awk -F 'v' '{print $2}' |awk -F '.' '{print $2$3}' 2>/dev/null)
      if [ "$(expr "$OP_CV" \>= "$OP_LV")" = "1" ]; then
         sed -i '/^https:/,$d' $LAST_OPVER
      elif [ "$(expr "$OP_LV" \> "$OP_CV")" = "1" ] && [ -n "$OP_LV" ]; then
         del_lock
         exit 2
      else
         del_lock
         exit 0
      fi
   else
      rm -rf "$LAST_OPVER"
   fi
elif [ "$(expr "$OP_LV" \> "$OP_CV")" = "1" ] && [ -n "$OP_LV" ]; then
   del_lock
   exit 2
else
   del_lock
   exit 0
fi 2>/dev/null
del_lock