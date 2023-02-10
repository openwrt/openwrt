#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "GPS" "$@"
}

CURRMODEM=$1
SMSdest=$2
MODTYPE=$(uci -q get modem.modem$CURRMODEM.modemtype)
if [ "$MODTYPE" == "2" -o "$MODTYPE" == "6" ]; then
	COMMPORT="/dev/ttyUSB"$(uci -q get modem.modem$CURRMODEM.commport)
else
	log "SMS position request from $SMSdest failed, modem $CURRMODEM is neither a Sierra nor Quectel"
	exit
fi
GPSon=$(uci -q get gps.configuration.enabled)
if [ "$GPSon" != "1" -o $CURRMODEM == "2" ]; then
	GPSon="0"
	if [ $MODTYPE == "2" ]; then
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "AT!CUSTOM?")
		GPSsel=$(echo $OX | grep "GPSSEL")
		GPSenable=$(echo $OX | grep "GPSENABLE")
		if [ -z "$GPSsel" -o -z "$GPSenable" ]; then
			ATCMDD="AT!ENTERCND=\"A710\""
			OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
			if [ -z "$GPSsel" ]; then
				ATCMDD="at!custom=\"GPSSEL\",1"
				OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
			fi
			if [ -z "$GPSenable" ]; then
				ATCMDD="at!custom=\"GPSENABLE\",1"
				OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
				ATCMDD="AT+CFUN=0;+CFUN=1,1"
				OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
				sleep 30
			fi
			ATCMDD="AT!ENTERCND=\"AWRONG\""
			OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		else
			GPSon="1"
		fi
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "AT!GPSTRACK=1,240,30,1000,5")
		GPSendcmd="AT!GPSEND=0"
	else
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "AT+QGPS?")
		err=$(echo "$OX" | grep "+QGPS: 1")
		if [ -z "$err" ]; then
			OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "AT+QGPS=1")
		else
			GPSon="1"
		fi
		ATCMDD="AT+QCFG=\"gpsdrx\""
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		err=$(echo "$OX" | grep "0")
		if [ -n "$err" ]; then
			ATCMDD="AT+QCFG=\"gpsdrx\",1"
			OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		fi
		GPSendcmd="AT+QGPSEND"
	fi
fi
SMStime=$(($(date +%s) + 120))
LAT=""
LON=""
while true; do
	if [ $MODTYPE == "2" ]; then
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "AT!GPSLOC?")
		LATx=$(echo $OX | grep -o "Lat:[^(]\+([^)]\+" | grep -o "0x.\+")
		LONx=$(echo $OX | grep -o "Lon:[^(]\+([^)]\+" | grep -o "0x.\+")
		if [ -n "$LATx" -a -n "$LONx" ]; then
			if [ $(printf "%d" $LATx) -gt $(printf "%d" 0x7FFFFFFF) ]; then
				LATx=$(( ($LATx ^ 0xFFFFFFFF) + 1 ))
				LAT="-"$(lua -e "print(string.format(\"%.5f\", $LATx * (180 / 2^25)))")
			else
				LAT=$(lua -e "print(string.format(\"%.5f\", $LATx * (180 / 2^25)))")
			fi
			if [ $(printf "%d" $LONx) -gt $(printf "%d" 0x7FFFFFFF) ]; then
				LONx=$(( ($LONx ^ 0xFFFFFFFF) + 1 ))
				LON="-"$(lua -e "print(string.format(\"%.5f\", $LONx * (180 / 2^25)))")
			else
				LON=$(lua -e "print(string.format(\"%.5f\", $LONx * (180 / 2^25)))")
			fi
		fi
	else
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "AT+QGPSLOC=2")
		LAT=$(echo $OX | grep -o "+QGPSLOC:[ .0-9]\+,.\+" | cut -d, -f2)
		LON=$(echo $OX | grep -o "+QGPSLOC:[ .0-9]\+,.\+" | cut -d, -f3)
	fi
	if [ -n "$LAT" -a -n "$LON" ]; then
		SMSMODEM=$(uci -q get modem.general.smsnum)
		if [ "$CURRMODEM" != "$SMSMODEM" ]; then
			uci set modem.general.smsnum=$CURRMODEM
			uci commit modem.general.smsnum
		fi
		OY=$(/usr/lib/sms/smsout.sh "$SMSdest" "$LAT $LON")
		OYsent=$(echo $OY | grep -o "SMS sent")
		if [ -n "$OYsent" ]; then
			/usr/lib/sms/sys2sms.sh "ROOter" "GPS coordinates sent to $SMSdest"
			log "GPS coordinates sent to $SMSdest"
		else
			log "Failed to send GPS coordinates sent to $SMSdest"
		fi
		if [ "$CURRMODEM" != "$SMSMODEM" ]; then
			uci set modem.general.smsnum=$SMSMODEM
			uci commit modem.general.smsnum
		fi
		break
	fi
	if [ $(date +%s) -gt $SMStime ]; then
		log "Failed request from $SMSdest by SMS for LAT/LON, position not available"
		break
	fi
	sleep 8
done
if [ "$GPSon" != "1" ]; then
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$GPSendcmd")
fi
