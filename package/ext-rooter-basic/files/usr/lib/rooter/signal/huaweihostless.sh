#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "hostless " "$@"
}

TMPFILE="/tmp/XXXXXX"

handle_timeout(){
	local wget_pid="$1"
	local count=0
	ps | grep -v grep | grep $wget_pid
	res="$?"
	while [ "$res" = 0 -a $count -lt "$((TIMEOUT))" ]; do
		sleep 1
		count=$((count+1))
		ps | grep -v grep | grep $wget_pid
		res="$?"
	done

	if [ "$res" = 0 ]; then
		log "Killing process on timeout"
		kill "$wget_pid" 2> /dev/null
		ps | grep -v grep | grep $wget_pid
		res="$?"
		if [ "$res" = 0 ]; then
			log "Killing process on timeout"
			kill -9 $wget_pid 2> /dev/null
		fi
	fi
}

make_status() {
	echo "$IP" > /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "$CSQ" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "$MODEM" >> /tmp/status$CURRMODEM.file
	echo "$COPS" >> /tmp/status$CURRMODEM.file
	echo "$NETWORK" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "$COPS_MCC" >> /tmp/status$CURRMODEM.file
	echo "$COPS_MNC" >> /tmp/status$CURRMODEM.file
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
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "Hostless/Phone" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
	echo "-" >> /tmp/status$CURRMODEM.file
}

get_signal() {
	TIMEOUT=3
	wget http://$IP/api/monitoring/status --load-cookies cookie -O $TMPFILE > /dev/null 2>&1 &
	handle_timeout "$!"
	local in_CurrentNetworkType="`cat $TMPFILE | grep \"<CurrentNetworkType>\" | cut -d \">\" -f 2 | cut -d \"<\" -f 1`"
	if [ -z $in_CurrentNetworkType ]; then
		NETWORK="-"
	else
		[ "$in_CurrentNetworkType" = "19" ] && NETWORK="LTE" # LTE
		[ "$in_CurrentNetworkType" = "9" ] && NETWORK="HSPA+" # HSPA+
		[ "$in_CurrentNetworkType" = "7" ] && NETWORK="HSPA" # HSPA
		[ "$in_CurrentNetworkType" = "6" ] && NETWORK="HSUPA" # HSUPA
		[ "$in_CurrentNetworkType" = "5" ] && NETWORK="HSDPA" # HSDPA
		[ "$in_CurrentNetworkType" = "4" ] && NETWORK="WCDMA" # WCDMA
		[ "$in_CurrentNetworkType" = "3" ] && NETWORK="EDGE" # EDGE
		[ "$in_CurrentNetworkType" = "2" ] && NETWORK="GPRS" # GPRS
		[ "$in_CurrentNetworkType" = "1" ] && NETWORK="GSM" # GSM
	fi
	SIGNAL=""
	local in_SignalStrength="`cat $TMPFILE | grep \"<SignalStrength>\" | cut -d \">\" -f 2 | cut -d \"<\" -f 1`"
	[ "$in_SignalStrength" != "" ] && SIGNAL="$in_SignalStrength"

	if [ "$SIGNAL" = "" ]; then
		in_SignalStrength="`cat $TMPFILE | grep \"<SignalIcon>\" | cut -d \">\" -f 2 | cut -d \"<\" -f 1`"
		[ "$in_SignalStrength" != "" ] && SIGNAL="$((in_SignalStrength*20))"
	fi
	[ -z $SIGNAL ] && SIGNAL=0
	rm -f $TMPFILE
}

CURRMODEM=$1
PROTO=$2
CONN="Modem #"$CURRMODEM

MANUF=$(uci get modem.modem$CURRMODEM.manuf)
MODEL=$(uci get modem.modem$CURRMODEM.model)
MODEM=$MANUF" "$MODEL
IP=$(uci get modem.modem$CURRMODEM.ip)

STARTIMEX=$(date +%s)
MONSTAT="Unknown"
rm -f /tmp/monstat$CURRMODEM

sleep 5
TIMEOUT=5
wget -q http://$IP/html/home.html -O nul --save-cookies cookie --keep-session-cookies
sleep 5
wget http://$IP/api/net/current-plmn -O $TMPFILE --load-cookies cookie > /dev/null 2>&1 &
handle_timeout "$!"

wget http://$IP/api/device/information -O $TMPFILE --load-cookies cookie > /dev/null 2>&1 &
handle_timeout "$!"
in_mod="`cat $TMPFILE | grep \"<DeviceName>\" | cut -d \">\" -f 2 | cut -d \"<\" -f 1`"
if [ "$in_mod" != "" ]; then
	MODEM=$MANUF" "$in_mod
fi
IMEI="Unknown"
IMSI="Unknown"
ICCID="Unknown"
CNUM="*"
CNUMx="*"
in_im="`cat $TMPFILE | grep \"<Imei>\" | cut -d \">\" -f 2 | cut -d \"<\" -f 1`"
if [ "$in_im" != "" ]; then
	IMEI=$in_im
fi
in_im="`cat $TMPFILE | grep \"<Imsi>\" | cut -d \">\" -f 2 | cut -d \"<\" -f 1`"
if [ "$in_im" != "" ]; then
	IMSI=$in_im
fi
in_im="`cat $TMPFILE | grep \"<Iccid>\" | cut -d \">\" -f 2 | cut -d \"<\" -f 1`"
if [ "$in_im" != "" ]; then
	ICCID=$in_im
fi
in_im="`cat $TMPFILE | grep \"<Msisdn>\" | cut -d \">\" -f 2 | cut -d \"<\" -f 1`"
if [ "$in_im" != "" ]; then
	CNUM=$in_im
fi


IDP=$(uci get modem.modem$CURRMODEM.idP)
IDV=$(uci get modem.modem$CURRMODEM.idV)

echo $IDV" : "$IDP > /tmp/msimdatax$CURRMODEM
echo "$IMEI" >> /tmp/msimdatax$CURRMODEM
echo "$IMSI" >> /tmp/msimdatax$CURRMODEM
echo "$ICCID" >> /tmp/msimdatax$CURRMODEM
echo "1" >> /tmp/msimdatax$CURRMODEM
mv -f /tmp/msimdatax$CURRMODEM /tmp/msimdata$CURRMODEM
echo "$CNUM" > /tmp/msimnumx$CURRMODEM
echo "$CNUMx" >> /tmp/msimnumx$CURRMODEM
mv -f /tmp/msimnumx$CURRMODEM /tmp/msimnum$CURRMODEM

sleep 20
TIMEOUT=20
wget http://$IP/api/net/current-plmn -O $TMPFILE --load-cookies cookie > /dev/null 2>&1 &
handle_timeout "$!"
COPS=""
in_provider="`cat $TMPFILE | grep \"<FullName>\" | cut -d \">\" -f 2 | cut -d \"<\" -f 1`"
[ "$in_provider" != "" ] && COPS="$in_provider"

if [ "$COPS" = "" ]; then
	COPS="-"
	in_provider="`cat $TMPFILE | grep \"<ShortName>\" | cut -d \">\" -f 2 | cut -d \"<\" -f 1`"
	[ "$in_provider" != "" ] && COPS="$in_provider)"
fi
COPS_MCC="-"
COPS_MNC="-"
in_mcc="`cat $TMPFILE | grep \"<Numeric>\" | cut -d \">\" -f 2 | cut -d \"<\" -f 1`"
if [ "$in_mcc" != "" ]; then
	COPS_MCC=${in_mcc:0:3}
	COPS_MNC=${in_mcc:3:3}
fi
COPS_MNC=" "$COPS_MNC
TIMEOUT=5

while [ 1 = 1 ]; do
	get_signal
	CSQ="$SIGNAL"
	if [ -e /tmp/monstat$CURRMODEM ]; then
		source /tmp/monstat$CURRMODEM
	fi
	if [ -z $MONSTAT ]; then
		MONSTAT="Unknown"
	fi
	make_status
	CURRTIME=$(date +%s)
	let ELAPSE=CURRTIME-STARTIMEX
	while [ $ELAPSE -lt 10 ]; do
		sleep 2
		CURRTIME=$(date +%s)
		let ELAPSE=CURRTIME-STARTIMEX
	done
	STARTIMEX=$CURRTIME
done
