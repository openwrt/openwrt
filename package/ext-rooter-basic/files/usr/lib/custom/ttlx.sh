#!/bin/sh 

log() {
	logger -t "TTL Hack" "$@"
}

cp -f /etc/firewall.user /etc/firewall.user.bk
sed /"#startTTL"/,/"#endTTL"/d /etc/firewall.user.bk > /etc/firewall.user
rm -f /etc/firewall.user.bk
/etc/init.d/firewall restart 2> /dev/null

