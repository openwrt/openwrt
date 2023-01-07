#!/bin/sh

ROOTER=/usr/lib/rooter

CURRMODEM=$1
COMMPORT=$2

if [ -e /etc/nocops ]; then
	echo "0" > /tmp/block
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "cellinfo0.gcom" "$CURRMODEM")
	rm -f /tmp/block
else
	OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "cellinfo0.gcom" "$CURRMODEM")
fi
OY=$($ROOTER/gcom/gcom-locked "$COMMPORT" "cellinfo.gcom" "$CURRMODEM")
OXx=$OX

OX=$(echo $OX | tr 'a-z' 'A-Z')
OY=$(echo $OY | tr 'a-z' 'A-Z')
OX=$OX" "$OY

COPS="-"
COPS_MCC="-"
COPS_MNC="-"
COPSX=$(echo $OXx | grep -o "+COPS: [01],0,.\+," | cut -d, -f3 | grep -o "[^\"]\+")

if [ "x$COPSX" != "x" ]; then
	COPS=$COPSX
fi

COPSX=$(echo $OX | grep -o "+COPS: [01],2,.\+," | cut -d, -f3 | grep -o "[^\"]\+")

if [ "x$COPSX" != "x" ]; then
	COPS_MCC=${COPSX:0:3}
	COPS_MNC=${COPSX:3:3}
	if [ "$COPS" = "-" ]; then
		COPS=$(awk -F[\;] '/'$COPS'/ {print $2}' $ROOTER/signal/mccmnc.data)
		[ "x$COPS" = "x" ] && COPS="-"
	fi
fi

if [ "$COPS" = "-" ]; then
	COPS=$(echo "$O" | awk -F[\"] '/^\+COPS: 0,0/ {print $2}')
	if [ "x$COPS" = "x" ]; then
		COPS="-"
		COPS_MCC="-"
		COPS_MNC="-"
	fi
fi
COPS_MNC=" "$COPS_MNC

OX=$(echo "${OX//[ \"]/}")

CID=""
CID5=""
RAT=""
REGV=$(echo "$OX" | grep -o "+C5GREG:2,[0-9],[A-F0-9]\{2,6\},[A-F0-9]\{5,10\},[0-9]\{1,2\}")
if [ -n "$REGV" ]; then
	LAC5=$(echo "$REGV" | cut -d, -f3)
	LAC5=$LAC5" ($(printf "%d" 0x$LAC5))"
	CID5=$(echo "$REGV" | cut -d, -f4)
	CID5L=$(printf "%010X" 0x$CID5)
	RNC5=${CID5L:1:6}
	RNC5=$RNC5" ($(printf "%d" 0x$RNC5))"
	CID5=${CID5L:7:3}
	CID5="Short $(printf "%X" 0x$CID5) ($(printf "%d" 0x$CID5)), Long $(printf "%X" 0x$CID5L) ($(printf "%d" 0x$CID5L))"
	RAT=$(echo "$REGV" | cut -d, -f5)
fi
REGV=$(echo "$OX" | grep -o "+CEREG:2,[0-9],[A-F0-9]\{2,4\},[A-F0-9]\{5,8\}")
REGFMT="3GPP"
if [ -z "$REGV" ]; then
	REGV=$(echo "$OX" | grep -o "+CEREG:2,[0-9],[A-F0-9]\{2,4\},[A-F0-9]\{1,3\},[A-F0-9]\{5,8\}")
	REGFMT="SW"
fi
if [ -n "$REGV" ]; then
	LAC=$(echo "$REGV" | cut -d, -f3)
	LAC=$(printf "%04X" 0x$LAC)" ($(printf "%d" 0x$LAC))"
	if [ $REGFMT = "3GPP" ]; then
		CID=$(echo "$REGV" | cut -d, -f4)
	else
		CID=$(echo "$REGV" | cut -d, -f5)
	fi
	CIDL=$(printf "%08X" 0x$CID)
	RNC=${CIDL:1:5}
	RNC=$RNC" ($(printf "%d" 0x$RNC))"
	CID=${CIDL:6:2}
	CID="Short $(printf "%X" 0x$CID) ($(printf "%d" 0x$CID)), Long $(printf "%X" 0x$CIDL) ($(printf "%d" 0x$CIDL))"

else
	REGV=$(echo "$OX" | grep -o "+CREG:2,[0-9],[A-F0-9]\{2,4\},[A-F0-9]\{2,8\}")
	if [ -n "$REGV" ]; then
		LAC=$(echo "$REGV" | cut -d, -f3)
		CID=$(echo "$REGV" | cut -d, -f4)
		if [ ${#CID} -gt 4 ]; then
			LAC=$(printf "%04X" 0x$LAC)" ($(printf "%d" 0x$LAC))"
			CIDL=$(printf "%08X" 0x$CID)
			RNC=${CIDL:1:3}
			CID=${CIDL:4:4}
			CID="Short $(printf "%X" 0x$CID) ($(printf "%d" 0x$CID)), Long $(printf "%X" 0x$CIDL) ($(printf "%d" 0x$CIDL))"
		else
			LAC=""
		fi
	else
		LAC=""
	fi
fi
REGSTAT=$(echo "$REGV" | cut -d, -f2)
if [ "$REGSTAT" == "5" -a "$COPS" != "-" ]; then
	COPS_MNC=$COPS_MNC" (Roaming)"
fi
if [ -n "$CID" -a -n "$CID5" ] && [ "$RAT" == "13" -o "$RAT" == "10" ]; then
	LAC="4G $LAC, 5G $LAC5"
	CID="4G $CID<br />5G $CID5"
	RNC="4G $RNC, 5G $RNC5"
elif [ -n "$CID5" ]; then
	LAC=$LAC5
	CID=$CID5
	RNC=$RNC5
fi
if [ -z "$LAC" ]; then
	LAC="-"
	CID="-"
	RNC="-"
fi

{
	echo 'COPS="'"$COPS"'"'
	echo 'COPS_MCC="'"$COPS_MCC"'"'
	echo 'COPS_MNC="'"$COPS_MNC"'"'
	echo 'LAC="'"$LAC"'"'
	echo 'LAC_NUM="'""'"'
	echo 'CID="'"$CID"'"'
	echo 'CID_NUM="'""'"'
	echo 'RNC="'"$RNC"'"'
	echo 'RNC_NUM="'""'"'
} > /tmp/cell$CURRMODEM.file
