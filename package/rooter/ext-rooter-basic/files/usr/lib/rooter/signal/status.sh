#!/bin/sh

CURRMODEM=$1
MSG=$2
MSG1=$3
COMMPORT="/dev/ttyUSB"$(uci -q get modem.modem$CURRMODEM.commport)
if [ -z $MSG1 ]; then
	MSG1="-"
	COMMPORT="-"
fi

echo "$COMMPORT" > /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "$MSG" >> /tmp/status$CURRMODEM.file
echo "$MSG1" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo " " >> /tmp/status$CURRMODEM.file
echo " " >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "Modem $CURRMODEM" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
echo "-" >> /tmp/status$CURRMODEM.file
