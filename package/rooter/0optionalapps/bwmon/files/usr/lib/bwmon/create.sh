#!/bin/sh

log() {
	logger -t "createdata" "$@"
}

lua /usr/lib/bwmon/create_data.lua
sleep 60
while [ true ]
do
	result=`ps | grep -i "create_data.lua" | grep -v "grep" | wc -l`
	if [ $result -lt 1 ]; then
		lua /usr/lib/bwmon/create_data.lua
	fi
	sleep 60
done