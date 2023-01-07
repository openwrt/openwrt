#!/bin/sh 

log() {
	logger -t "Blacklist" "$@"
}


result=`ps | grep -i "blacklist.sh" | grep -v "grep" | wc -l`
if [ $result -eq 0 ]; then
	/usr/lib/blacklist/blacklist.sh &
fi