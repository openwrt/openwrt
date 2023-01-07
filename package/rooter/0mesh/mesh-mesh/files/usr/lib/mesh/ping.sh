#!/bin/sh
. /lib/functions.sh

pinging() {
	PING=0
	RETURN_CODE_1=$(curl -m 10 -s -o /dev/null -w "%{http_code}" http://www.google.com/)
	RETURN_CODE_2=$(curl -m 10 -s -o /dev/null -w "%{http_code}" http://www.example.org/)
	RETURN_CODE_3=$(curl -m 10 -s -o /dev/null -w "%{http_code}" https://github.com)
	
	if [[ "$RETURN_CODE_1" != "200" &&  "$RETURN_CODE_2" != "200" &&  "$RETURN_CODE_3" != "200" ]]; then
		PING=1
	fi
}

gateway() {
	mode=$1
# batman-adv gateway handling (DHCP mangling)
	[ "$(uci -q get batman-adv.bat0.gw_mode)" == "client" ] || return
	if grep -q "^=>" /sys/kernel/debug/batman_adv/bat0/gateways ; then
		BATTYPE=gw BATACTION=$mode /etc/hotplug.d/net/99-batman-gw
	fi
}

while true ; do
	sleep 20
	pinging
	if [ $PING -eq 1 ]; then
		gateway add
		batctl gw_mode client
	else
		gateway del
		batctl gw_mode server 10000
	fi
done