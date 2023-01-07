#!/bin/sh 
. /lib/functions.sh

bwdata() {
	result=`ps | grep -i "create_data.lua" | grep -v "grep" | wc -l`
	if [ $result -lt 1 ]; then
		lua /usr/lib/bwmon/create_data.lua
	fi
	while [ true ]
	do
		if [ -e /tmp/bwdata ]; then
			break
		fi
		sleep 1
	done
}

bwdata
/usr/lib/iframe/update.sh
logtype=$(uci -q get iframe.login.logtype)
if [ $logtype = "1" ]; then
	sleep 300
	while [ true ]
	do
		result=`ps | grep -i "update.sh" | grep -v "grep" | wc -l`
		if [ $result -lt 1 ]; then
			/usr/lib/iframe/update.sh
		fi
		sleep 300
	done
fi