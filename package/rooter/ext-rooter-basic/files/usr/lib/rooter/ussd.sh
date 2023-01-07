#!/bin/sh

CURRMODEM=$(uci get modem.general.modemnum)
COMMPORT="/dev/ttyUSB"$(uci get modem.modem$CURRMODEM.commport)
ROOTER=/usr/lib/rooter

if [ -n "$1" ]; then
	echo "$1" > /tmp/ussd_arg$CURRMODEM
fi

while true; do
	if [ -e /tmp/ussd_arg$CURRMODEM ]; then
		read USSDSTR < /tmp/ussd_arg$CURRMODEM
		rm /tmp/ussd_arg$CURRMODEM
	fi
	if [ -n "$USSDSTR" ]; then
		ATCMDD="AT+CUSD=1,\"$USSDSTR\",15"
		OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "ussd.gcom" "$CURRMODEM" "$ATCMDD" | tr "\n" "\v")
		USSD=$(echo "$OX" | grep -o "+CUSD: .\+\",[0-9]\+" | tr "\v" "\n")
		USSDL=${#USSD}
		USSDLx=$((USSDL - 2))
		DCS=$(printf "${USSD:$USSDLx:2}")
		if [ $USSDL -ge 14 ]; then
			USSDL=$((USSDL - 14))
			USSD=$(printf "${USSD:10:$USSDL}")
			if [ $DCS -eq "72" ]; then
				USSDx=""
				USSDL=${#USSD}
				nV=0
				until [ $nV -ge $USSDL ]; do
					UU=$(printf "%d" "0x"${USSD:$nV:4})
					if [[ $UU -lt 128 ]]; then
						USSDx="$USSDx"$(printf "%b" "\\$(printf "0%o" "$UU")")
					elif [[ $UU -lt 2048 ]]; then
						UUU=$(((($UU & 1984) >> 6) | 192))
						USSDx="$USSDx"$(printf "%b" "\\$(printf "0%o" "$UUU")")
						UUU=$((($UU & 63) | 128))
						USSDx="$USSDx"$(printf "%b" "\\$(printf "0%o" "$UUU")")
					else
						UUU=$(((($UU & 61440) >> 12) | 224))
						USSDx="$USSDx"$(printf "%b" "\\$(printf "0%o" "$UUU")")
						UUU=$(((($UU & 4032) >> 6) | 128))
						USSDx="$USSDx"$(printf "%b" "\\$(printf "0%o" "$UUU")")
						UUU=$((($UU & 63) | 128))
						USSDx="$USSDx"$(printf "%b" "\\$(printf "0%o" "$UUU")")
					fi
					nV=$(( $nV + 4 ))
				done
				USSD="$USSDx"
			fi
		else
			USSD=$(echo "$OX" | tr "\v" "\n")
		fi
		echo
		echo "-----------------------------------------------------------"
		echo "$USSD"
		echo "-----------------------------------------------------------"
		echo
	fi
	printf "Enter blank to quit, or a USSD string to send: "; read USSDSTR
	if [ -z "$USSDSTR" ]; then
		break
	fi
done
ATCMDD="AT+CUSD=2"
OX=$($ROOTER/gcom/gcom-locked "$COMMPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
exit 0
