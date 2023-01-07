#!/bin/sh

killprocess() {
	proc=$1
	PID=$(pgrep -f "$proc")
	if [ ! -z $PID ]; then
		kill -9 $PID
	fi
}

killprocess "speedtest --test-server"
killprocess "/speedtest/closest.lua"
killprocess "/speedtest/getspeed.sh"
killprocess "/speedtest/servers.lua"

flist="/tmp/speed /tmp/sinfo /tmp/close /tmp/getspeed /tmp/jpg /tmp/pinfo /tmp/sinfo /tmp/slist"

for val in $flist; do
	rm -f $val
done

echo "0" > /tmp/getspeed
echo "0" >> /tmp/getspeed
echo "0" >> /tmp/getspeed
echo "0" >> /tmp/getspeed
echo "0" >> /tmp/getspeed
echo "0" > /tmp/spworking

