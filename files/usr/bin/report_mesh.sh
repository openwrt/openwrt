#!/bin/sh
while true; do
    HOSTNAME=$(cat /proc/sys/kernel/hostname)
    MAC=$(cat /sys/class/net/eth0/address 2>/dev/null)
    NEIGHBORS=$(batctl o 2>/dev/null | awk 'NR>1{print "\"" $2 "\""}' | paste -sd,)
    TS=$(date +%s)
    echo "{\"ts\":$TS,\"host\":\"$HOSTNAME\",\"mac\":\"$MAC\",\"neighbors\":[$NEIGHBORS]}" | alfred -s 100 2>/dev/null
    sleep 10
done
