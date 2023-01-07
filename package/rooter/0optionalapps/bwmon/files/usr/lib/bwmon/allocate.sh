#!/bin/sh

log() {
	logger -t "allocate" "$@"
}

amount=$1

if [ $amount != "0" ]; then
	uci set custom.bwallocate.allocate=$amount
	uci commit custom
else
	sleep 3
fi
result=`ps | grep -i "create_data.lua" | grep -v "grep" | wc -l`
while [ $result -ge 1 ]; do
	sleep 2
	result=`ps | grep -i "create_data.lua" | grep -v "grep" | wc -l`
done

lua /usr/lib/bwmon/create_data.lua
