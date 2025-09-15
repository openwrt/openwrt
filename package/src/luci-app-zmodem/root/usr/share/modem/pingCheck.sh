#!/bin/sh
#Todo Avoiding conflicts
# Enable_PING=`uci -q get modem.@ndis[0].pingen` || Enable_PING=0
# if [ ${Enable_PING} == 0 ];then
#     rm -rf "$LOCK_FILE"
#     exit 1
# fi
# LOCK_FILE="/tmp/pingCheck.lock"
# if [ -e "$LOCK_FILE" ]; then
#     echo "pingCheck互斥" >> /tmp/pingCheck.log
#     exit 1
# fi
# SLEEP_FILE="/tmp/pingCheck.file"
# if [ -e "$SLEEP_FILE" ]; then
#     sleep 1
# else
#     echo "系统第一次启动 等待40秒模块启动" >> /tmp/rm520n.log
#     sleep 40
#     touch "$SLEEP_FILE"
# fi
# touch "$LOCK_FILE"
# REST_FILE="/tmp/pingCheck.rest"
# pingaddr=$(uci -q get modem.@ndis[0].pingaddr) || pingaddr="119.29.29.29"
# timesleep=$(uci -q get modem.@ndis[0].count) || timesleep=5
# while :; do
#     if ping -c 1 -w 1 "$pingaddr"; then
#         echo "网络连接正常" > /tmp/pingCheck.log
#         sleep $timesleep
#         continue
#     fi
#     result=$(sendat 3 "AT+CPIN?")
#     if echo "$result" | grep -q "READY"; then
#         echo "SIM卡准备就绪 但网络连接失败 重启WAN口" >> /tmp/pingCheck.log
#         /sbin/ifup wan
#         /sbin/ifup wan6
#         sleep 10
#     else 
#         if [ -e "$REST_FILE" ]; then
#             rm -rf "$LOCK_FILE"
#             echo "重启无效 退出脚本" >> /tmp/pingCheck.log
#             exit 0
#             else
#             touch "$REST_FILE"
#             killall sendat
#             sendat 3 'AT+CFUN=1,1'
#             echo "尝试重启一次5G模块" >> /tmp/pingCheck.log
#             sleep 35
#             /sbin/ifup wan
#             /sbin/ifup wan6
#         fi
#     fi
# done
