#!/bin/sh

ROOTER=/usr/lib/rooter

NEWMOD=$1

 log() {
	modlog "ProtoChange $CURRMODEM" "$@"
 }
 
ifname1="ifname"
if [ -e /etc/newstyle ]; then
	ifname1="device"
fi
 
 change_bconf() {
	local devname=$1
	local conf=$2
	local mode=$3
	local unconf=0
	log "Switching Modem at $devname to $mode by selecting Cfg# $conf"
	echo $unconf >/sys/bus/usb/devices/$devname/bConfigurationValue
	sleep 1
	echo $conf >/sys/bus/usb/devices/$devname/bConfigurationValue
}
 
 chkT77() {
	T77=0
	if [ $idV = 413c -a $idP = 81d7 ]; then
		T77=1
	elif [ $idV = 413c -a $idP = 81d8 ]; then
		T77=1
	elif [ $idV = 0489 -a $idP = e0b4 ]; then
		T77=1
	elif [ $idV = 0489 -a $idP = e0b5 ]; then
		T77=1
	elif [ $idV = 1bc7 -a $idP = 1910 ]; then
		T77=1
	fi
}
 
 chksierra() {
	SIERRAID=0
	if [ $idV = 1199 ]; then
		case $idP in
			"68aa"|"68a2"|"68a3"|"68a9"|"68b0"|"68b1" )
				SIERRAID=1
			;;
			"68c0"|"9040"|"9041"|"9051"|"9054"|"9056"|"90d3" )
				SIERRAID=1
			;;
			"9070"|"907b"|"9071"|"9079"|"901c"|"9091"|"901f"|"90b1" )
				SIERRAID=1
			;;
		esac
	elif [ $idV = 114f -a $idP = 68a2 ]; then
		SIERRAID=1
	elif [ $idV = 413c -a $idP = 81a8 ]; then
		SIERRAID=1
	elif [ $idV = 413c -a $idP = 81b6 ]; then
		SIERRAID=1
	fi
}

chkquectel() {
	QUECTEL=false
	if [ "$idV" = "2c7c" ]; then
		QUECTEL=true
	elif [ "$idV" = "05c6" ]; then
		QUELST="9090,9003,9215"
		if [[ $(echo "$QUELST" | grep -o "$idP") ]]; then
			QUECTEL=true
		fi
	fi
}

log "Protocol Change to $NEWMOD"

CURRMODEM=$(uci get modem.general.modemnum)
CPORT=$(uci get modem.modem$CURRMODEM.commport)
idV=$(uci get modem.modem$CURRMODEM.uVid)
idP=$(uci get modem.modem$CURRMODEM.uPid)

chkquectel
if $QUECTEL; then
	case $NEWMOD in
		"1" )
			ATCMDD='AT+QCFG="usbnet",0'
		;;
		"2" )
			ATCMDD='AT+QCFG="usbnet",2'
		;;
		"3" )
			ATCMDD='AT+QCFG="usbnet",1'
		;;
	esac
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
fi

 chksierra
 if [ $SIERRAID -eq 1 ]; then
	ATCMDD='AT!ENTERCND="A710"'
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	
	case $idP in
		"68c0"|"9041"|"901f" ) # MC7354 EM/MC7355
			case $NEWMOD in
				"1" )
					ATCMDD='at!UDUSBCOMP=6'
				;;
				"2" )
					ATCMDD='at!UDUSBCOMP=8'
				;;
			esac
		;;
		"9070"|"9071"|"9078"|"9079"|"907a"|"907b" ) # EM/MC7455
			case $NEWMOD in
				"1" )
					ATCMDD='at!usbcomp=1,1,10d'
				;;
				"2" )
					ATCMDD='at!usbcomp=1,1,1009'
				;;
			esac
		;;
		"9090"|"9091"|"90b1" ) # EM7565
			case $NEWMOD in
				"1" )
					ATCMDD='at!usbcomp=1,3,10d'
				;;
				"2" )
					ATCMDD='AT!USBCOMP=1,3,1009'
				;;
			esac
		;;
	esac
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
	ATCMDD='AT!ENTERCND="AWRONG"'
	OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
 fi
 
 chkT77
 if [ $T77 = "1" ]; then
	DEVICE=$(uci get modem.modem$CURRMODEM.device)
	if [ $NEWMOD = "1" ]; then
		change_bconf $DEVICE 1 QMI
	else
		change_bconf $DEVICE 2 MBIM
	fi
	log "T77 $NEWMOD $DEVICE"
 fi
sleep 5

/usr/lib/rooter/luci/restart.sh $CURRMODEM "9"

