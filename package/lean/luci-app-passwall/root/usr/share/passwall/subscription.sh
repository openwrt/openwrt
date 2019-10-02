#!/bin/sh

CONFIG=passwall
LOCK_FILE=/var/lock/${CONFIG}_subscription.lock
Date=$(date "+%Y-%m-%d %H:%M:%S")
LOG_FILE=/var/log/$CONFIG.log

config_t_get() {
	local index=0
	[ -n "$3" ] && index=$3
	local ret=$(uci get $CONFIG.@$1[$index].$2 2>/dev/null)
	#echo ${ret:=$3}
	echo $ret
}

start() {
	#防止并发开启服务
	[ -f "$LOCK_FILE" ] && return 3
	touch "$LOCK_FILE"
	echo "$Date: 开始执行在线订阅脚本..." >> $LOG_FILE
	/usr/share/$CONFIG/subscription_ssr.sh start 2>/dev/null
	/usr/share/$CONFIG/subscription_v2ray.sh start 2>/dev/null
	echo "$Date: 在线订阅脚本执行完毕..." >> $LOG_FILE
	rm -f "$LOCK_FILE"
	exit 0
}

stop() {
	echo "$Date: 开始执行删除所有订阅脚本..." >> $LOG_FILE
	/usr/share/$CONFIG/subscription_ssr.sh stop 2>/dev/null
	/usr/share/$CONFIG/subscription_v2ray.sh stop 2>/dev/null
	echo "$Date: 删除所有订阅脚本执行完毕..." >> $LOG_FILE
	rm -f "$LOCK_FILE"
	exit 0
}

case $1 in
stop)
	stop
	;;
*)
	start
	;;
esac
