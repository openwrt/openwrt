#!/bin/sh

CURRMODEM=$1

uVid=$(uci get modem.modem$CURRMODEM.uVid)
uPid=$(uci get modem.modem$CURRMODEM.uPid)

if [ $uVid == "2c7c" ]; then
	qc=$(uci get custom.atcmd.quectel)
	echo "$qc" > /tmp/modemlock
	echo " " >> /tmp/modemlock
else
	if [ $uVid == "1199" ]; then
		qc=$(uci get custom.atcmd.sierra)
	echo "$qc" > /tmp/modemlock
		echo " " >> /tmp/modemlock
	else
		qc=$(uci get custom.atcmd.generic)
	echo "$qc" > /tmp/modemlock
		echo " " >> /tmp/modemlock
	fi
fi