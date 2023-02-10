#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	modlog "Get ICCID $CURRMODEM" "$@"
}

echo "0" > /tmp/block

decode_crsm() {
	i=0
	while [ $i -lt $length ]; do
		c1=${sstring:$i:1}
		let 'j=i+1'
		c2=${sstring:$j:1}
		xstring=$xstring$c2$c1
		let 'i=i+2'
	done
}

CURRMODEM=$1
CPORT=$(uci get modem.modem$CURRMODEM.commport)

OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "gettype.gcom" "$CURRMODEM")
OX=$($ROOTER/common/processat.sh "$OX")

MANUF=$(echo "$OX" | awk -F[:] '/Manufacturer:/ { print $2}')

if [ -z "$MANUF" ]; then
        ATCMDD="AT+CGMI"
        MANUF=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	MANUF=$(echo $MANUF)
	MANUF=$(echo "${MANUF//[\"]/}")
	MANUF=${MANUF::-3}
	MPREFIX=${MANUF::8}
	if [ "$MPREFIX" = "AT+CGMI " ]; then
		MANUF=$(echo $MANUF | cut -c 9-)
	fi
	MPREFIX=${MANUF::7}
	if [ "$MPREFIX" = "+CGMI: " ]; then
		MANUF=$(echo $MANUF | cut -c 8-)
	fi
fi
if [ -z "$MANUF" ]; then
	MANUF=$(uci get modem.modem$CURRMODEM.manuf)
fi

MODEL=$(echo "$OX" | awk -F[,\ ] '/^\+MODEL:/ {print $2}')

if [ -z "$MODEL" ]; then
        ATCMDD="AT+CGMM"
        MODEL=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	MODEL=$(echo $MODEL)
	MODEL=$(echo "${MODEL//[\"]/}")
	MODEL=${MODEL::-3}
	MPREFIX=${MODEL::8}
	if [ "$MPREFIX" = "AT+CGMM " ]; then
		MODEL=$(echo $MODEL | cut -c 9-)
	fi
	MPREFIX=${MODEL::7}
	if [ "$MPREFIX" = "+CGMM: " ]; then
		MODEL=$(echo $MODEL | cut -c 8-)
	fi
	MODEL=$(echo $MODEL | cut -d, -f1)
fi
if [ -z "$MODEL" ]; then
	MODEL=$(uci get modem.modem$CURRMODEM.model)
fi

uci set modem.modem$CURRMODEM.manuf="$MANUF"
uci set modem.modem$CURRMODEM.model="$MODEL"
uci commit modem

$ROOTER/signal/status.sh $CURRMODEM "$MANUF $MODEL" "Connecting"

IMEI=$(echo "$OX" | awk -F[,\ ] '/^\IMEI:/ {print $2}')

if [ -z "$IMEI" ]; then
	ATCMDD="AT+CGSN"
	IMEI=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	IMEI=$(echo $IMEI | grep -o "[0-9]\{15\}")
fi

if [ -z "$IMEI" ]; then
	ATCMDD="ATI5"
	IMEI=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	IMEI=$(echo $IMEI | grep -o "[0-9]\{15\}")
fi

if [ -n "$IMEI" ]; then
	IMEI=$(echo "$IMEI" | sed -e 's/"//g')
	IMEI=${IMEI:0:15}
else
	IMEI="Unknown"
fi
uci set modem.modem$CURRMODEM.imei=$IMEI

IDP=$(uci get modem.modem$CURRMODEM.idP)
IDV=$(uci get modem.modem$CURRMODEM.idV)

echo $IDV" : "$IDP > /tmp/msimdatax$CURRMODEM
echo "$IMEI" >> /tmp/msimdatax$CURRMODEM

lua $ROOTER/signal/celltype.lua "$MODEL" $CURRMODEM
source /tmp/celltype$CURRMODEM
rm -f /tmp/celltype$CURRMODEM

uci set modem.modem$CURRMODEM.celltype=$CELL
uci commit modem

$ROOTER/luci/celltype.sh $CURRMODEM

M2=$(echo "$OX" | grep -o "+CNUM:[^,]*,[^,]*,[0-9]\{3\}")","
M2=${M2:6}
CNUMx=$(echo "$M2" | cut -d, -f1 | cut -d\" -f2)
CNUMx=$(echo $CNUMx)
CNUM=$(echo "$M2" | cut -d, -f2 | cut -d\" -f2)
CNUMtype=$(echo "$M2" | cut -d, -f3)
if [ "${CNUM:0:1}" != "+" -a "$CNUMtype" == "145" ]; then
	CNUM="+"$CNUM
fi
if [ -z "$CNUM" ]; then
	CNUM="*"
fi
if [ -z "$CNUMx" ]; then
	CNUMx="*"
fi

NLEN=$(echo "$OX" | awk -F[,\ ] '/^\+CPBR:/ {print $4}')
if [ "x$NLEN" != "x" ]; then
	NLEN=$(echo "$NLEN" | sed -e 's/"//g')
else
	NLEN="14"
fi
echo 'NLEN="'"$NLEN"'"' > /tmp/namelen$CURRMODEM

ATCMDD="ATE1"
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
ATCMDD="AT+CTZU=1"
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
ATCMDD="AT\$QCPBMPREF?"
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
PBzero=$(echo "$OX" | grep "0")
if [ -n "$PBzero" ]; then
	ATCMDD="AT\$QCPBMPREF=1"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
fi
ATCMDD="AT+CGDCONT?"
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
ATCMDD="AT\$QCPDPIMSCFGE?"
OX=$OX$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
OX=$(echo $OX | tr 'a-z' 'A-Z')
OX=$(echo "${OX//[ ]/}")
imsAPN=$(echo $OX | grep -o "+CGDCONT:2,[^,]\+,\"IMS\"")
ims_on=$(echo $OX | grep -o "\$QCPDPIMSCFGE:2,1,")
if [ -n "$imsAPN" -a -z "$ims_on" ]; then
	ATCMDD="AT\$QCPDPIMSCFGE=2,1"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
fi
if [ "$IDV" == "2c7c" ]; then
	ATCMDD="AT+QLWCFG=\"startup\""
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	LWon=$(echo "$OX" | grep "1")
	if [ -n "$LWon" ]; then
		ATCMDD="AT+QLWCFG=\"startup\",0"
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	fi
	ATCMDD="AT+QCFG=\"ims\""
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	IMSon=$(echo "$OX" | grep "1")
	if [ -z "$IMSon" ]; then
		ATCMDD="AT+QCFG=\"ims\",1"
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	fi
fi
ATCMDD="AT+CIMI"
OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
OX=$($ROOTER/common/processat.sh "$OX")
ERROR="ERROR"
if `echo ${OX} | grep "${ERROR}" 1>/dev/null 2>&1`
then
	ATCMDD="AT+CRSM=176,28423,0,0,9"
	sstring=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD" | grep -o "[0-9F]\{18\}")
	if [ -n "$sstring" ]; then
		length=${#sstring}
		decode_crsm
		IMSI=${xstring:3}
	else
		IMSI="Unknown"
	fi
else
	OX=${OX//[!0-9]/}
	IMSIL=${#OX}
	IMSI=${OX:0:$IMSIL}
fi
echo "$IMSI" >> /tmp/msimdatax$CURRMODEM
uci set modem.modem$CURRMODEM.imsi=$IMSI

idV=$(uci -q get modem.modem$CURRMODEM.idV)
idP=$(uci -q get modem.modem$CURRMODEM.idP)
if [ $idV = 0e8d ]; then
	ATCMDD="AT+CCID"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	OX=$($ROOTER/common/processat.sh "$OX")
	ERROR="ERROR"
	if `echo ${OX} | grep "${ERROR}" 1>/dev/null 2>&1`
	then
		ICCID="Unknown"
	else
		ICCID=$(echo "$OX" | awk -F[,\ ] '/^\+CCID:/ {print $2}')
		if [ "x$ICCID" != "x" ]; then
			sstring=$(echo "$ICCID" | sed -e 's/"//g')
			ICCID=$sstring
		else
			ICCID="Unknown"
		fi
	fi
else
	ATCMDD="AT+CRSM=176,12258,0,0,10"
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	OX=$($ROOTER/common/processat.sh "$OX")
	ERROR="ERROR"
	if `echo ${OX} | grep "${ERROR}" 1>/dev/null 2>&1`
	then
		ICCID="Unknown"
	else
		ICCID=$(echo "$OX" | awk -F[,\ ] '/^\+CRSM:/ {print $4}')
		if [ "x$ICCID" != "x" ]; then
			sstring=$(echo "$ICCID" | sed -e 's/"//g')
			length=${#sstring}
			xstring=""
			decode_crsm
			ICCID=$xstring
		else
			ICCID="Unknown"
		fi
	fi
fi
uci set modem.modem$CURRMODEM.iccid=$ICCID
uci commit modem
if [ -e /etc/config/modeinfo ]; then
	uci set modeinfo.global.iccid$CURRMODEM=$ICCID
	uci commit modeinfo
fi
echo "$ICCID" >> /tmp/msimdatax$CURRMODEM
echo "0" >> /tmp/msimdatax$CURRMODEM
echo "$CNUM" > /tmp/msimnumx$CURRMODEM
echo "$CNUMx" >> /tmp/msimnumx$CURRMODEM

mv -f /tmp/msimdatax$CURRMODEM /tmp/msimdata$CURRMODEM
mv -f /tmp/msimnumx$CURRMODEM /tmp/msimnum$CURRMODEM

rm -f /tmp/block
