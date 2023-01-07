#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Wireguard TextConf" "$@"
}

conf1=$1
conf=$(echo $conf1)
conf=$(echo "$conf" | tr "?" "~")

boot=$(echo "$conf" | cut -d~ -f1)
iname=$(echo "$conf" | cut -d~ -f2)
conf=$(echo "$conf1" | tr "?" "~")
confile=$(echo "$conf" | cut -d~ -f3)

echo "$confile" > /tmp/confile

/usr/lib/wireguard/conf.sh $iname /tmp/confile $boot