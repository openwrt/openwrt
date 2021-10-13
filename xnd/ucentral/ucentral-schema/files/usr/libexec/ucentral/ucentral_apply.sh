#!/bin/sh

config=$1

echo config:$config

[ -f "$config" ] || {
	logger "ucentral_apply: invalid paramters"
	exit 1
}

/usr/share/ucentral/ucentral.uc

[ $? -eq 0 ] || {
	logger "ucentral_apply: applying $1 failed"
	exit 1
}

active=$(readlink /etc/ucentral/ucentral.active)
[ -n "$active" -a -f "$active" ] && {
	rm -f /etc/ucentral/ucentral.old
	ln -s $active /etc/ucentral/ucentral.old
}

rm -f /etc/ucentral/ucentral.active
ln -s $config /etc/ucentral/ucentral.active

return 0
