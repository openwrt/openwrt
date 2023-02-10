#!/bin/sh

sleep 10
rm -f /tmp/timez
wget --no-check-certificate -O /tmp/timez http://ip-api.com/json > /dev/null 2>&1
OX=$(cat /tmp/timez)

TZ=$(echo $OX" " | tr -d '"' | grep -o 'timezone:[^;]*' | tr ":" ",")
ZONEN=$(echo "$TZ" | cut -d, -f2)

/usr/lib/rooter/tzone.lua "$ZONEN"
source /tmp/tzone

uci set system.@system[-1].timezone="$ZNAME" 
uci set system.@system[-1].zonename="$ZONEN" 
uci commit system
/etc/init.d/system restart


