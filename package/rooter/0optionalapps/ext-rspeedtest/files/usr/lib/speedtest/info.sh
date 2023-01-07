#!/bin/sh

killprocess() {
	proc=$1
	PID=$(ps |grep "$proc" | grep -v grep |head -n 1 | awk '{print $1}')
	if [ ! -z $PID ]; then
		kill -9 $PID
	fi
}

killprocess "speedtest --test-server"
killprocess "/speedtest/closest.lua"
killprocess "/speedtest/getspeed.sh"
killprocess "/speedtest/servers.lua"
	
rm -f /tmp/speed	
rm -f /tmp/sinfo
rm -f /tmp/close
rm -f /tmp/getspeed
rm -f /tmp/jpg
rm -f /tmp/pinfo
rm -f /tmp/sinfo
rm -f /tmp/slist
echo "0" > /tmp/getspeed
echo "0" >> /tmp/getspeed
echo "0" >> /tmp/getspeed
echo "0" >> /tmp/getspeed
echo "0" >> /tmp/getspeed
echo "0" > /tmp/spworking
wget -q -O /tmp/client http://speedtest.net/speedtest-config.php
if [ $? = "0" ]; then
	RAW=$(cat /tmp/client)
	clid=$(echo $RAW" " | grep -o "<client .\+ /> " | tr -d '"' | tr " " "," | tr "/" "," | tr "=" ",")
	ip=$(echo $clid | cut -d, -f3)
	ulat=$(echo $clid | cut -d, -f5)
	ulon=$(echo $clid | cut -d, -f7)
	isp=$(echo $clid | cut -d, -f9)
	rm -f /tmp/client
	echo "1" > /tmp/sinfo
	echo "$ip" >> /tmp/sinfo
	echo "$isp" >> /tmp/sinfo
	
	wget -q -O /tmp/servers http://www.speedtest.net/speedtest-servers-static.php
	if [ $? != "0" ]; then
		wget -q -O /tmp/servers http://c.speedtest.net/speedtest-servers.php
		if [ $? != "0" ]; then
			echo "0" > /tmp/sinfo
			exit 0
		fi
	fi
	rm -f /tmp/slist
	while IFS= read -r line
	do
		read -r line
		/usr/lib/speedtest/servers.lua "$line"
		if [ -e /tmp/server ]; then
			source /tmp/server
			echo "$URL $LAT $LON "\""$NAME"\"" $SPONSOR" >> /tmp/slist 
			rm -f /tmp/server
		fi
	done < /tmp/servers
	rm -f /tmp/servers
	
	/usr/lib/speedtest/closest.lua $ulat $ulon
	source /tmp/close
	echo "$CITY" >> /tmp/sinfo
	echo "$PROV" >> /tmp/sinfo
	echo "$CURL" >> /tmp/sinfo
	rm -f /tmp/slist
	rm -f /tmp/close
else
	echo "0" > /tmp/sinfo
	exit 0
fi

