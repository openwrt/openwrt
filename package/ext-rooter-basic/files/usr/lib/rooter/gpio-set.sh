#!/bin/sh

GPIO=$1	# name or number (without "gpio" prefix)
VAL=$2	# 0 or 1
WAIT=$3	# delay in seconds, optional

log() {
	modlog "GPIO Set" "$@"
}


[ -n "$WAIT" ] && sleep $WAIT

if `echo "$GPIO" | grep -q "^[0-9]*$"`; then
	GPIO=gpio$GPIO
fi

if [ -w /sys/class/gpio/$GPIO/value ]; then
	echo "$VAL" > /sys/class/gpio/$GPIO/value
	log "GPIO $GPIO has been set to $VAL"
else
	log "GPIO $GPIO is not writable"
fi
