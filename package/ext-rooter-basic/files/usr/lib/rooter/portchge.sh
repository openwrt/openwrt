#!/bin/sh

DIR=$1

log() {
	logger -t "port change" "$@"
}


CURRMODEM=$(uci get modem.general.modemnum)
BASEP=$(uci get modem.modem$CURRMODEM.baseport)
MAXP=$(uci get modem.modem$CURRMODEM.maxport)
PORT=$(uci get modem.modem$CURRMODEM.commport)

log "$DIR"

if [ $DIR = "up" ]; then
	if [ $PORT -lt $MAXP ]; then
		PORT=`expr $PORT + 1`
		echo 'PORT="'"$PORT"'"' > /tmp/port$CURRMODEM.file
	fi
else
	if [ $PORT -gt $BASEP ]; then
		PORT=`expr $PORT - 1`
		echo 'PORT="'"$PORT"'"' > /tmp/port$CURRMODEM.file
	fi
fi

