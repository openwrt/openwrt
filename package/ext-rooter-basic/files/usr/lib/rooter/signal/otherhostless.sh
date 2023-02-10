#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "hostless " "$@"
}

get_basic() {
	# get basic data here
	#
	# how it is done with other modems
	#
	COMMPORT="/dev/ttyUSB"$(uci get modem.modem$CURRMODEM.commport)
	$ROOTER/signal/basedata.sh $CURRMODEM $COMMPORT
	source /tmp/base$CURRMODEM.file
	rm -f /tmp/base$CURRMODEM.file
	$ROOTER/signal/celldata.sh $CURRMODEM $COMMPORT
	source /tmp/cell$CURRMODEM.file
	rm -f /tmp/cell$CURRMODEM.file
	lua $ROOTER/signal/celltype.lua "$MODEM" $CURRMODEM
	source /tmp/celltype$CURRMODEM
	rm -f /tmp/celltype$CURRMODEM
}

make_signal() {
	# get signal data here
	#
	# how it is done with other modems (Sierra)
	#
	COMMPORT="/dev/ttyUSB"$(uci get modem.modem$CURRMODEM.commport)
	MANUF=$(uci get modem.modem$CURRMODEM.manuf)
	MODEL=$(uci get modem.modem$CURRMODEM.model)
	MODEM=$MANUF" "$MODEL
	$ROOTER/common/ubloxdata.sh $CURRMODEM $COMMPORT
	source /tmp/signal$CURRMODEM.file
	rm -f /tmp/signal$CURRMODEM.file
	echo "$COMMPORT" > /tmp/statusx$CURRMODEM.file
	echo "$CSQ" >> /tmp/statusx$CURRMODEM.file
	echo "$CSQ_PER" >> /tmp/statusx$CURRMODEM.file
	echo "$CSQ_RSSI" >> /tmp/statusx$CURRMODEM.file
	echo "$MODEM" >> /tmp/statusx$CURRMODEM.file
	echo "$COPS" >> /tmp/statusx$CURRMODEM.file
	echo "$MODE" >> /tmp/statusx$CURRMODEM.file
	echo "$LAC" >> /tmp/statusx$CURRMODEM.file
	echo "$LAC_NUM" >> /tmp/statusx$CURRMODEM.file
	echo "$CID" >> /tmp/statusx$CURRMODEM.file
	echo "$CID_NUM" >> /tmp/statusx$CURRMODEM.file
	echo "$COPS_MCC" >> /tmp/statusx$CURRMODEM.file
	echo "$COPS_MNC" >> /tmp/statusx$CURRMODEM.file
	echo "$RNC" >> /tmp/statusx$CURRMODEM.file
	echo "$RNC_NUM" >> /tmp/statusx$CURRMODEM.file
	echo "$DOWN" >> /tmp/statusx$CURRMODEM.file
	echo "$UP" >> /tmp/statusx$CURRMODEM.file
	echo "$ECIO" >> /tmp/statusx$CURRMODEM.file
	echo "$RSCP" >> /tmp/statusx$CURRMODEM.file
	echo "$ECIO1" >> /tmp/statusx$CURRMODEM.file
	echo "$RSCP1" >> /tmp/statusx$CURRMODEM.file
	echo "$MONSTAT" >> /tmp/statusx$CURRMODEM.file
	echo "$CELL" >> /tmp/statusx$CURRMODEM.file
	echo "$MODTYPE" >> /tmp/statusx$CURRMODEM.file
	echo "$CONN" >> /tmp/statusx$CURRMODEM.file
	echo "$CHANNEL" >> /tmp/statusx$CURRMODEM.file
	echo "$CNUM" >> /tmp/statusx$CURRMODEM.file
	echo "$CNAM" >> /tmp/statusx$CURRMODEM.file
	echo "$LBAND" >> /tmp/statusx$CURRMODEM.file
	echo "$TEMP" >> /tmp/statusx$CURRMODEM.file
	echo "$PROTO" >> /tmp/statusx$CURRMODEM.file
	echo "$PCI" >> /tmp/statusx$CURRMODEM.file
	echo "-" >> /tmp/statusx$CURRMODEM.file
	echo "-" >> /tmp/statusx$CURRMODEM.file
	mv -f /tmp/statusx$CURRMODEM.file /tmp/status$CURRMODEM.file
}

CURRMODEM=$1
PROTO=$2
CONN="Modem #"$CURRMODEM

MANUF=$(uci get modem.modem$CURRMODEM.manuf)
MODEL=$(uci get modem.modem$CURRMODEM.model)
MODEM=$MANUF" "$MODEL
IP=$(uci get modem.modem$CURRMODEM.ip)
MONSTAT="Unknown"
rm -f /tmp/monstat$CURRMODEM

idV=$(uci get modem.modem$CURRMODEM.idV)
idP=$(uci get modem.modem$CURRMODEM.idP)
if [ $idV = 1546 -a $idP = 1146 ]; then
	get_basic
fi


STARTIMEX=$(date +%s)
MONSTAT="Unknown"
rm -f /tmp/monstat$CURRMODEM

if [ -e $ROOTER/modem-led.sh ]; then
	$ROOTER/modem-led.sh $CURRMODEM 4
fi
		
while [ 1 = 1 ]; do
	if [ $idV = 1546 -a $idP = 1146 ]; then
# ublox
		make_signal
	else
		echo "$IP" > /tmp/status$CURRMODEM.file
		echo "-" >> /tmp/status$CURRMODEM.file
		echo "-" >> /tmp/status$CURRMODEM.file
		echo "-" >> /tmp/status$CURRMODEM.file
		echo "$MODEM" >> /tmp/status$CURRMODEM.file
		echo "Hostless/Phone" >> /tmp/status$CURRMODEM.file
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
		echo "$MONSTAT" >> /tmp/status$CURRMODEM.file
		echo "-" >> /tmp/status$CURRMODEM.file
		echo "-" >> /tmp/status$CURRMODEM.file
		echo "$CONN" >> /tmp/status$CURRMODEM.file
		echo "-" >> /tmp/status$CURRMODEM.file
		echo "-" >> /tmp/status$CURRMODEM.file
		echo "-" >> /tmp/status$CURRMODEM.file
		echo "$IP" >> /tmp/status$CURRMODEM.file
		echo "-" >> /tmp/status$CURRMODEM.file
		echo "Hostless/Phone" >> /tmp/status$CURRMODEM.file
		echo "-" >> /tmp/status$CURRMODEM.file
		echo "-" >> /tmp/status$CURRMODEM.file
		echo "-" >> /tmp/status$CURRMODEM.file
	fi
	uci set modem.modem$CURRMODEM.cmode="1"
	uci commit modem
	if [ -e /tmp/monstat$CURRMODEM ]; then
		source /tmp/monstat$CURRMODEM
	fi
	if [ -z $MONSTAT ]; then
		MONSTAT="Unknown"
	fi
		CURRTIME=$(date +%s)

				if [ -e /etc/netspeed ]; then
					NETSPEED=60
				else
					NETSPEED=10
				fi



	let ELAPSE=CURRTIME-STARTIMEX
	while [ $ELAPSE -lt $NETSPEED ]; do
		sleep 2
		CURRTIME=$(date +%s)
		let ELAPSE=CURRTIME-STARTIMEX
	done
	STARTIMEX=$CURRTIME
done
