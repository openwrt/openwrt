#!/bin/sh
CKTIME=$(date "+%Y-%m-%d-%H")
LAST_OPVER="/tmp/clash_last_version"
version_url="https://raw.githubusercontent.com/vernesong/OpenClash/master/core_version"
if [ "$CKTIME" != "$(grep "CheckTime" $LAST_OPVER 2>/dev/null |awk -F ':' '{print $2}')" ]; then
   wget-ssl --no-check-certificate --quiet --timeout=10 --tries=2 "$version_url" -O $LAST_OPVER
   if [ "$?" -eq "0" ] && [ "$(ls -l $LAST_OPVER 2>/dev/null |awk '{print int($5)}')" -gt 0 ]; then
      echo "CheckTime:$CKTIME" >>$LAST_OPVER
   else
      rm -rf $LAST_OPVER
   fi
fi