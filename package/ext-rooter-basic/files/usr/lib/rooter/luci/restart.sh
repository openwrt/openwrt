#!/bin/sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	modlog "Modem Restart/Diisconnect $CURRMODEM" "$@"
}

ifname1="ifname"
if [ -e /etc/newstyle ]; then
	ifname1="device"
fi

CURRMODEM=$1
CPORT=$(uci -q get modem.modem$CURRMODEM.commport)
INTER=$(uci get modem.modeminfo$CURRMODEM.inter)

if [ "$2" != "9" -a "$2" != "11" ]; then
	PROTO=$(uci get modem.modem$CURRMODEM.proto)
	if [ "$PROTO" = "3" -o "$PROTO" = "30" ]; then
		mdevice=$(uci -q get modem.modem$CURRMODEM.mdevice)
		mapn=$(uci -q get modem.modem$CURRMODEM.mapn)
		mipt=$(uci -q get modem.modem$CURRMODEM.mipt)
		nauth=$(uci -q get modem.modem$CURRMODEM.mauth)
		nusername=$(uci -q get modem.modem$CURRMODEM.musername)
		mpassword=$(uci -q get modem.modem$CURRMODEM.mpassword)
		log "MBIM Disconnect"
		umbim -t 1 -d "$mdevice" disconnect
		sleep 1
		modtype=$(uci -q get modem.modem$CURRMODEM.modtype)
		/usr/lib/rooter/connect/bandmask $CURRMODEM $modtype
		exit 0
	fi

	if [ "$PROTO" = "2" ]; then
		log "QMI Disconnect"
		mdevice=$(uci -q get modem.modem$CURRMODEM.mdevice)
		mapn=$(uci -q get modem.modem$CURRMODEM.mapn)
		mcid=$(uci -q get modem.modem$CURRMODEM.mcid)
		nauth=$(uci -q get modem.modem$CURRMODEM.mauth)
		nusername=$(uci -q get modem.modem$CURRMODEM.musername)
		mpassword=$(uci -q get modem.modem$CURRMODEM.mpassword)
		uqmi -s -d "$device" --stop-network 0xffffffff --autoconnect > /dev/null & sleep 1 ; kill -9 $!
		modtype=$(uci -q get modem.modem$CURRMODEM.modtype)
		/usr/lib/rooter/connect/bandmask $CURRMODEM $modtype
		exit 0
	fi
	if [ "$PROTO" = "5" ]; then
		log "Hostless Hard Reset by AT Command"
		jkillall getsignal$CURRMODEM
		jkillall processsms$CURRMODEM
		ATCMDD="AT+CFUN=4"
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		ATCMDD="AT+CFUN=1,1"
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		if [ -e $ROOTER/connect/chkconn.sh ]; then
			jkillall chkconn.sh
		fi
		exit 0
	fi
	if [ "$2" = "10" ]; then
		exit 0
	fi
fi

jkillall chkconn.sh
if [ "$2" != "9" -a "$2" != "11" ]; then # disconnect
	uci set modem.modem$CURRMODEM.connected=0
	uci commit modem
	jkillall getsignal$CURRMODEM
	rm -f $ROOTER_LINK/getsignal$CURRMODEM
	jkillall con_monitor$CURRMODEM
	rm -f $ROOTER_LINK/con_monitor$CURRMODEM
	ifdown wan$INTER
	$ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "reset.gcom" "$CURRMODEM"
else # restart
	uVid=$(uci get modem.modem$CURRMODEM.uVid)
	uPid=$(uci get modem.modem$CURRMODEM.uPid)
	#if [ $uVid != "2c7c" ]; then
		if [ ! -z "$CPORT" ]; then
			ATCMDD="AT+CFUN=1,1"
			OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		fi
		log "Hard modem reset done"
	#fi
	bn=$(cat /tmp/sysinfo/board_name)
	bn=$(echo "$bn" | grep "mk01k21")
	if [ ! -z "$bn" ]; then
		i=496
		echo $i > /sys/class/gpio/export
		echo "out" > /sys/class/gpio/gpio$i/direction
		echo "1" > /sys/class/gpio/gpio$i/value
		sleep 5
		echo "0" > /sys/class/gpio/gpio$i/value
		log "Power Toggle"
	fi
	ifdown wan$INTER
	uci delete network.wan$CURRMODEM
	uci set network.wan$CURRMODEM=interface
	uci set network.wan$CURRMODEM.proto=dhcp
	uci set network.wan$CURRMODEM.${ifname1}="wan"$CURRMODEM
	uci set network.wan$CURRMODEM.metric=$CURRMODEM"0"
	uci commit network
	/etc/init.d/network reload
	echo "1" > /tmp/modgone
	log "Hard USB reset done"
	if [ -e $ROOTER/connect/chkconn.sh ]; then
		jkillall chkconn.sh
	fi

	PORT="usb$CURRMODEM"
	echo $PORT > /sys/bus/usb/drivers/usb/unbind
	sleep 35
	echo $PORT > /sys/bus/usb/drivers/usb/bind
	if [ -e $ROOTER/modem-led.sh ]; then
		$ROOTER/modem-led.sh $CURRMODEM 0
	fi
fi
