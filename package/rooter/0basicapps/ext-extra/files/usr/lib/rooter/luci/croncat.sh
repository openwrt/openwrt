#!/bin/sh

log() {
	logger -t "Croncat" "$@"
}

> /tmp/crontmp
if [ -e /etc/crontabs/root ]; then
	while read -r line; do
		if [ -n "$line" ]; then
			if [ ${line: -1} == ";" ]; then
				echo "$line" >> /tmp/crontmp
			fi
		fi
	done < /etc/crontabs/root
fi

if [ -f /etc/cronuser ]; then
	if [ -f /etc/cronbase ]; then
		cat /etc/cronbase /etc/cronuser > /etc/crontabs/root
	else
		cp /etc/cronuser /etc/crontabs/root
	fi
else
	if [ -f /etc/cronbase ]; then
		cp /etc/cronbase /etc/crontabs/root
	else
		> /etc/crontabs/root
	fi
fi
cat /tmp/crontmp /etc/crontabs/root > /tmp/cronroot
cp /tmp/cronroot /etc/crontabs/root
rm /tmp/crontmp
rm /tmp/cronroot

/etc/init.d/cron restart
