#!/bin/sh
. /lib/functions.sh

log() {
	logger -t "Load" "$@"
}

profile=$(cat /tmp/profilename)
valid=$(echo "$profile" | grep "**Profile**")
if [ ! -z "$valid" ]; then
	echo "$profile" > /tmp/profile
	sed -i '1d' /tmp/profile
	cp /tmp/profile /etc/config/profile
fi
