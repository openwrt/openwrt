#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "NetID" "$@"
}

ID=$2

log "$ID"

if [ $ID = "xxxxxxxxxxxxxxxx" ]; then
	uci set zerotier.zerotier.enabled='0'
else
	uci set zerotier.zerotier.enabled='1'
fi

uci delete zerotier.zerotier.join
uci add_list zerotier.zerotier.join=$ID
uci commit zerotier
/etc/init.d/zerotier restart
