#!/bin/sh

ROOTER=/usr/lib/rooter
ROOTER_LINK=$ROOTER"/links"

log() {
	logger -t "MBIM Data" "$@"
}

STARTIMEX=$(date +%s)
MONSTAT=
rm -f /tmp/monstat$CURRMODEM

build_status() {
	CSQ=$signal
	if [ $CSQ -ge 0 -a $CSQ -le 31 ]; then
		CSQ_PER=$(($CSQ * 100/31))
		CSQ_RSSI=$((2 * CSQ - 113))
		CSQX=$CSQ_RSSI
		[ $CSQ -eq 0 ] && CSQ_RSSI="<= "$CSQ_RSSI
		[ $CSQ -eq 31 ] && CSQ_RSSI=">= "$CSQ_RSSI
		CSQ_PER=$CSQ_PER"%"
		CSQ_RSSI=$CSQ_RSSI" dBm"
	else
		CSQ="-"
		CSQ_PER="-"
		CSQ_RSSI="-"
	fi
	echo "-" > /tmp/status$CURRMODEM.file
	echo "$CSQ" >> /tmp/status$CURRMODEM.file
	echo "$CSQ_PER" >> /tmp/status$CURRMODEM.file
	echo "$CSQ_RSSI" >> /tmp/status$CURRMODEM.file
	echo "$manuf" >> /tmp/status$CURRMODEM.file
	echo "$provider" >> /tmp/status$CURRMODEM.file
	echo "$cellmode" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "$mcc" >> /tmp/status$CURRMODEM.file
	echo "$mnc" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "$down" >> /tmp/status$CURRMODEM.file
	echo "$up" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo " " >> /tmp/status$CURRMODEM.file
	echo " " >> /tmp/status$CURRMODEM.file
	echo "$MONSTAT" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "$conn" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "MBIM" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
}

CURRMODEM=$1

conn="Modem #"$CURRMODEM
custom=$(uci get modem.modem$CURRMODEM.custom)
port=$(uci get modem.modem$CURRMODEM.wdm)
netd=$(uci get modem.modem$CURRMODEM.wwan)
manuf=$(uci get modem.modem$CURRMODEM.manuf)
model=$(uci get modem.modem$CURRMODEM.model)
mcc=$(uci get modem.modem$CURRMODEM.mcc)
mnc=$(uci get modem.modem$CURRMODEM.mnc)
up=$(uci get modem.modem$CURRMODEM.up)
down=$(uci get modem.modem$CURRMODEM.down)
provider=$(uci get modem.modem$CURRMODEM.provider)
cellmode=$(uci get modem.modem$CURRMODEM.mode)
if [ $cellmode = "CUSTOM" ]; then
	cellmode=$custom
fi
signal=$(uci get modem.modem$CURRMODEM.sig)

device="/dev/cdc-wdm"$port
netdev="wwan"$netd
manuf=$manuf" "$model

tid=2
while [ 1 -eq 1 ]; do
	tid=2
	#SIGNAL=$(umbim -n -t $tid -d $device signal)
	tid=$((tid + 1))
	#SIGNAL=$(umbim -n -t $tid -d $device signal)
	signal=0
	if [ -e /tmp/monstat$CURRMODEM ]; then
		source /tmp/monstat$CURRMODEM
	fi
	if [ -z $MONSTAT ]; then
		MONSTAT="Unknown"
	fi
	build_status
	if [ -e /etc/netspeed ]; then
		NETSPEED=60
	else
		NETSPEED=10
	fi
	CURRTIME=$(date +%s)
	let ELAPSE=CURRTIME-STARTIMEX
	while [ $ELAPSE -lt $NETSPEED ]; do
		sleep 2
		CURRTIME=$(date +%s)
		let ELAPSE=CURRTIME-STARTIMEX
	done
	STARTIMEX=$CURRTIME
done
