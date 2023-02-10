#!/bin/sh

ROOTER=/usr/lib/rooter

log() {
	logger -t "modem signal" "$@"
}

CURRMODEM=$1
PROTO=$2
CONN="Modem #"$CURRMODEM
STARTIME=$(date +%s)
STARTIMEX=$(date +%s)
SMSTIME=0
COMMPORT="/dev/ttyUSB"$(uci get modem.modem$CURRMODEM.commport)
NUMB=0
MONSTAT="Unknown"
rm -f /tmp/monstat$CURRMODEM

make_connect() {
	{
		echo "Changing Port"
		echo "-"
		echo "-"
		echo "-"
		echo "$MODEM"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo " "
		echo " "
		echo "-"
		echo "-"
		echo "-"
		echo "$CONN"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
		echo "-"
	} > /tmp/statusx$CURRMODEM.file
	mv -f /tmp/statusx$CURRMODEM.file /tmp/status$CURRMODEM.file
}

make_signal() {
	{
		if [ -e $ROOTER/provchk.sh ]; then
			$ROOTER/provchk.sh $COPS $CURRMODEM
			source /tmp/cops$CURRMODEM.file
			rm -f /tmp/cops$CURRMODEM.file
		fi
		echo "$COMMPORT"
		echo "$CSQ"
		echo "$CSQ_PER"
		echo "$CSQ_RSSI"
		echo "$MODEM"
		echo "$COPS"
		echo "$MODE"
		echo "$LAC"
		echo "$LAC_NUM"
		echo "$CID"
		echo "$CID_NUM"
		echo "$COPS_MCC"
		echo "$COPS_MNC"
		echo "$RNC"
		echo "$RNC_NUM"
		echo "$DOWN"
		echo "$UP"
		echo "$ECIO"
		echo "$RSCP"
		echo "$ECIO1"
		echo "$RSCP1"
		echo "$MONSTAT"
		echo "$CELL"
		echo "$MODTYPE"
		echo "$CONN"
		echo "$CHANNEL"
		echo "$CNUM"
		echo "$CNAM"
		echo "$LBAND"
		echo "$TEMP"
		echo "$PROTO"
		echo "$PCI"
		echo "$SINR"
		echo "$LATITUDE"
		echo "$LONGITUDE"
	} > /tmp/statusx$CURRMODEM.file
	mv -f /tmp/statusx$CURRMODEM.file /tmp/status$CURRMODEM.file
	if [ -e $ROOTER/modem-led.sh ]; then
		$ROOTER/modem-led.sh $CURRMODEM 4 $CSQ
	fi
}

get_basic() {
	$ROOTER/signal/basedata.sh $CURRMODEM $COMMPORT
	if [ -e /tmp/base$CURRMODEM.file ]; then
		source /tmp/base$CURRMODEM.file
		rm -f /tmp/base$CURRMODEM.file
	fi
	$ROOTER/signal/celldata.sh $CURRMODEM $COMMPORT
	if [ -e /tmp/cell$CURRMODEM.file ]; then
		source /tmp/cell$CURRMODEM.file
		rm -f /tmp/cell$CURRMODEM.file
	fi
	lua $ROOTER/signal/celltype.lua "$MODEM" $CURRMODEM
	if [ -e /tmp/celltype$CURRMODEM ]; then
		source /tmp/celltype$CURRMODEM
		rm -f /tmp/celltype$CURRMODEM
	fi
}

get_basic
while [ 1 = 1 ]; do
	get_basic
	if [ -e /tmp/port$CURRMODEM.file ]; then
		source /tmp/port$CURRMODEM.file
		rm -f /tmp/port$CURRMODEM.file
		COMMPORT="/dev/ttyUSB"$PORT
		uci set modem.modem$CURRMODEM.commport=$PORT
		make_connect
		get_basic
		STARTIME=$(date +%s)
	else
		CURRTIME=$(date +%s)
		let ELAPSE=CURRTIME-STARTIME
		if [ $ELAPSE -ge 60 ]; then
			STARTIME=$CURRTIME
			$ROOTER/signal/celldata.sh $CURRMODEM $COMMPORT
			if [ -e /tmp/cell$CURRMODEM.file ]; then
				source /tmp/cell$CURRMODEM.file
				rm -f /tmp/cell$CURRMODEM.file
			fi
		fi
		if [ -e /tmp/port$CURRMODEM.file ]; then
			source /tmp/port$CURRMODEM.file
			rm -f /tmp/port$CURRMODEM.file
			COMMPORT="/dev/ttyUSB"$PORT
			uci set modem.modem$CURRMODEM.commport=$PORT
			make_connect
			get_basic
			STARTIME=$(date +%s)
		else
			VENDOR=$(uci get modem.modem$CURRMODEM.idV)
			PROD=$(uci get modem.modem$CURRMODEM.idP)
# This case statement should be kept in sync with: $ROOTER/luci/celltype.sh
			case $VENDOR in
			"1199"|"0f3d" )
				$ROOTER/common/sierradata.sh $CURRMODEM $COMMPORT
				;;
			"19d2" )
				if [ $PROD = 1432 ]; then
					$ROOTER/common/mdm9215data.sh $CURRMODEM $COMMPORT
				else
					$ROOTER/common/ztedata.sh $CURRMODEM $COMMPORT
				fi
				;;
			"12d1" )
				$ROOTER/common/huaweidata.sh $CURRMODEM $COMMPORT
				;;
			"2c7c" )
				$ROOTER/common/quecteldata.sh $CURRMODEM $COMMPORT
				;;
			"2cb7"|"1508" )
				$ROOTER/common/fibocomdata.sh $CURRMODEM $COMMPORT
				;;
			"2dee" )
				$ROOTER/common/meigdata.sh $CURRMODEM $COMMPORT
				;;
			"05c6" )
				case $PROD in
					"f601" )
						$ROOTER/common/meigdata.sh $CURRMODEM $COMMPORT
					;;
					"5042" )
						$ROOTER/common/mdm9215data.sh $CURRMODEM $COMMPORT
					;;
					"9090"|"9003"|"9215" )
						$ROOTER/common/quecteldata.sh $CURRMODEM $COMMPORT
					;;
					"90db" )
						$ROOTER/common/simcomdata.sh $CURRMODEM $COMMPORT
					;;
					* )
						$ROOTER/common/otherdata.sh $CURRMODEM $COMMPORT
					;;
				esac
				;;
			"1bc7" )
				case $PROD in
					"1900"|"1901"|"1910"|"1911" )
						$ROOTER/common/t77data.sh $CURRMODEM $COMMPORT
					;;
					* )
						$ROOTER/common/telitdata.sh $CURRMODEM $COMMPORT
					;;
				esac
				;;
			"1410" )
				$ROOTER/common/novateldata.sh $CURRMODEM $COMMPORT
				;;
			"413c" )
				case $PROD in
					"81d7"|"81d8" )
						$ROOTER/common/t77data.sh $CURRMODEM $COMMPORT
					;;
					* )
						$ROOTER/common/sierradata.sh $CURRMODEM $COMMPORT
					;;
				esac
				;;
			"0489" |"03f0" )
				$ROOTER/common/t77data.sh $CURRMODEM $COMMPORT
				;;
			"1e0e" )
				$ROOTER/common/simcomdata.sh $CURRMODEM $COMMPORT
				;;
			"8087" )
				if [ $PROD = "095a" ]; then
					$ROOTER/common/fibocomdata.sh $CURRMODEM $COMMPORT
				fi
				;;
			"0408" )
				$ROOTER/common/quantadata.sh $CURRMODEM $COMMPORT
				;;
			* )
				$ROOTER/common/otherdata.sh $CURRMODEM $COMMPORT
				;;
			esac
			CHANNEL="-"
			PCI="-"
			LBAND="-"
			TEMP="-"
			SINR="-"
			LATITUDE="-"
			LONGITUDE="-"
			if [ -e /tmp/signal$CURRMODEM.file ]; then
				source /tmp/signal$CURRMODEM.file
				rm -f /tmp/signal$CURRMODEM.file
			fi
			if [ -e /tmp/phonenumber$CURRMODEM ]; then
				source /tmp/phonenumber$CURRMODEM
				rm -f /tmp/phonenumber$CURRMODEM
			fi
			if [ -e /tmp/gpsdata ]; then
				source /tmp/gpsdata
			fi
			make_signal
			uci set modem.modem$CURRMODEM.cmode="1"
			uci commit modem
			if [ -e /tmp/monstat$CURRMODEM ]; then
				source /tmp/monstat$CURRMODEM
			fi
			if [ -z "$MONSTAT" ]; then
				MONSTAT="Unknown"
			fi
		fi
	fi
	if [ -e /etc/netspeed ]; then
		NETSPEED=60
	else
		NETSPEED=10
	fi
	CURRTIME=$(date +%s)
	SLEEPTIME=$(($(echo $NETSPEED) - ($(echo $CURRTIME) - $(echo $STARTIMEX))))
	if [ $SLEEPTIME -gt $NETSPEED ]; then
		sleep $NETSPEED
	elif [ $SLEEPTIME -lt 1 ]; then
		sleep 2
	else
		sleep $SLEEPTIME
	fi
	STARTIMEX=$(date +%s)
done
