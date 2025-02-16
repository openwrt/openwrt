#!/bin/bash
. /lib/functions.sh
. /usr/share/openclash/openclash_ps.sh
. /usr/share/openclash/log.sh

set_lock() {
   exec 872>"/tmp/lock/openclash_core.lock" 2>/dev/null
   flock -x 872 2>/dev/null
}

del_lock() {
   flock -u 872 2>/dev/null
   rm -rf "/tmp/lock/openclash_core.lock" 2>/dev/null
}

set_lock

github_address_mod=$(uci -q get openclash.config.github_address_mod || echo 0)
if [ "$github_address_mod" = "0" ] && [ -z "$(echo $2 2>/dev/null |grep -E 'http|one_key_update')" ] && [ -z "$(echo $3 2>/dev/null |grep 'http')" ]; then
   LOG_OUT "Tip: If the download fails, try setting the CDN in Overwrite Settings - General Settings - Github Address Modify Options"
fi
if [ -n "$3" ] && [ "$2" = "one_key_update" ]; then
   github_address_mod="$3"
fi
if [ -n "$2" ] && [ "$2" != "one_key_update" ]; then
   github_address_mod="$2"
fi
CORE_TYPE="$1"
C_CORE_TYPE=$(uci -q get openclash.config.core_type)
[ -z "$CORE_TYPE" ] && CORE_TYPE="Meta"
small_flash_memory=$(uci -q get openclash.config.small_flash_memory)
CPU_MODEL=$(uci -q get openclash.config.core_version)
RELEASE_BRANCH=$(uci -q get openclash.config.release_branch || echo "master")
LOG_FILE="/tmp/openclash.log"

if [ "$github_address_mod" != "0" ]; then
   [ ! -f "/tmp/clash_last_version" ] && /usr/share/openclash/clash_version.sh "$github_address_mod" 2>/dev/null
else
   [ ! -f "/tmp/clash_last_version" ] && /usr/share/openclash/clash_version.sh 2>/dev/null
fi
if [ ! -f "/tmp/clash_last_version" ]; then
   LOG_OUT "Error: 【"$CORE_TYPE"】Core Version Check Error, Please Try Again Later..."
   SLOG_CLEAN
   del_lock
   exit 0
fi

if [ "$small_flash_memory" != "1" ]; then
   meta_core_path="/etc/openclash/core/clash_meta"
   mkdir -p /etc/openclash/core
else
   meta_core_path="/tmp/etc/openclash/core/clash_meta"
   mkdir -p /tmp/etc/openclash/core
fi

CORE_CV=$($meta_core_path -v 2>/dev/null |awk -F ' ' '{print $3}' |head -1)
CORE_LV=$(sed -n 3p /tmp/clash_last_version 2>/dev/null)

   
[ "$C_CORE_TYPE" = "$CORE_TYPE" ] || [ -z "$C_CORE_TYPE" ] && if_restart=1

if [ "$CORE_CV" != "$CORE_LV" ] || [ -z "$CORE_CV" ]; then
   if [ "$CPU_MODEL" != 0 ]; then
      LOG_OUT "【Meta】Core Downloading, Please Try to Download and Upload Manually If Fails"
      if [ "$github_address_mod" != "0" ]; then
         if [ "$github_address_mod" == "https://cdn.jsdelivr.net/" ] || [ "$github_address_mod" == "https://fastly.jsdelivr.net/" ] || [ "$github_address_mod" == "https://testingcf.jsdelivr.net/" ]; then
            curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 "$github_address_mod"gh/vernesong/OpenClash@core/"$RELEASE_BRANCH"/meta/clash-"$CPU_MODEL".tar.gz -o /tmp/clash_meta.tar.gz 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="/tmp/clash_meta.tar.gz" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
         else
            curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 "$github_address_mod"https://raw.githubusercontent.com/vernesong/OpenClash/core/"$RELEASE_BRANCH"/meta/clash-"$CPU_MODEL".tar.gz -o /tmp/clash_meta.tar.gz 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="/tmp/clash_meta.tar.gz" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
         fi
      else
         curl -SsL --connect-timeout 30 -m 60 --speed-time 30 --speed-limit 1 --retry 2 https://raw.githubusercontent.com/vernesong/OpenClash/core/"$RELEASE_BRANCH"/meta/clash-"$CPU_MODEL".tar.gz -o /tmp/clash_meta.tar.gz 2>&1 |sed ':a;N;$!ba; s/\n/ /g' | awk -v time="$(date "+%Y-%m-%d %H:%M:%S")" -v file="/tmp/clash_meta.tar.gz" '{print time "【" file "】Download Failed:【"$0"】"}' >> "$LOG_FILE"
      fi

      if [ "${PIPESTATUS[0]}" -eq 0 ]; then
         gzip -t /tmp/clash_meta.tar.gz >/dev/null 2>&1
      fi

      if [ "$?" == "0" ]; then
         LOG_OUT "【"$CORE_TYPE"】Core Download Successful, Start Update..."
         [ -s "/tmp/clash_meta.tar.gz" ] && {
            tar zxvfo /tmp/clash_meta.tar.gz -C /tmp >/dev/null 2>&1
            mv /tmp/clash /tmp/clash_meta >/dev/null 2>&1
            rm -rf /tmp/clash_meta.tar.gz >/dev/null 2>&1
            chmod 4755 /tmp/clash_meta >/dev/null 2>&1
            /tmp/clash_meta -v >/dev/null 2>&1
         }
			   
         if [ "$?" != "0" ]; then
            LOG_OUT "【"$CORE_TYPE"】Core Update Failed. Please Make Sure Enough Flash Memory Space or Selected Correct Core Platform And Try Again!"
            rm -rf /tmp/clash_meta >/dev/null 2>&1
            SLOG_CLEAN
            del_lock
            exit 0
         fi

         mv /tmp/clash_meta "$meta_core_path" >/dev/null 2>&1

         if [ "$?" == "0" ]; then
            LOG_OUT "【"$CORE_TYPE"】Core Update Successful!"
            if [ "$if_restart" -eq 1 ]; then
               uci -q set openclash.config.restart=1
               uci -q commit openclash
               if ([ -z "$2" ] || ([ -n "$2" ] && [ "$2" != "one_key_update" ])) && [ "$(unify_ps_prevent)" -eq 0 ]; then
                  uci -q set openclash.config.restart=0
                  uci -q commit openclash
                  /etc/init.d/openclash restart >/dev/null 2>&1 &
               fi
            else
               SLOG_CLEAN
            fi
         else
            LOG_OUT "【"$CORE_TYPE"】Core Update Failed. Please Make Sure Enough Flash Memory Space And Try Again!"
            SLOG_CLEAN
         fi
      else
         LOG_OUT "【"$CORE_TYPE"】Core Update Failed, Please Check The Network or Try Again Later!"
         SLOG_CLEAN
      fi
   else
      LOG_OUT "No Compiled Version Selected, Please Select In Update Page And Try Again!"
      SLOG_CLEAN
   fi
else
   LOG_OUT "【"$CORE_TYPE"】Core Has Not Been Updated, Stop Continuing Operation!"
   SLOG_CLEAN
fi

rm -rf /tmp/clash_meta >/dev/null 2>&1
del_lock
