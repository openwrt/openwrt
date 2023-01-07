#!/bin/sh

log() {
	logger -t "sendsms" "$@"
}

chksms() {
	CURRMODEM=1
	rm -f /tmp/texting
	CPORT=$(uci -q get modem.modem$CURRMODEM.commport)
	if [ -z $CPORT ]; then
		return
	fi
	SMS_OK=$(uci -q get modem.modem$CURRMODEM.sms)
	if [ "$SMS_OK" != "1" ]; then
		return
	fi 
	echo "0" > /tmp/texting
}

delay=$(uci -q get modem.modeminfo$CURRMODEM.delay)
if [ "$delay" -gt 0 ]; then
	cnt=1
	while true; do
		sleep 3600
		let cnt=cnt+1
		if [ $cnt -gt $delay ]; then
			break
		fi
	done
fi
total=$(uci -q get.custom.bwday.bwday)
chksms
phone=$(uci -q get modem.modeminfo$CURRMODEM.bwphone)
if [ (! -z "$phone") -o ("$phone" != "0") ]; then
	if [ -e /tmp/texting ]; then
		/usr/lib/sms/smsout.sh "$phone" "$total" 
	fi
fi
