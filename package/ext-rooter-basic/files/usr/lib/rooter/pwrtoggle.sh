#!/bin/sh

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
        modlog "Power Toggle $CURRMODEM" "$@"
}

ifname1="ifname"
if [ -e /etc/newstyle ]; then
	ifname1="device"
fi

waitfor() {
	CNTR=0
	while [ -e /tmp/modgone ]; do
		sleep 1
		CNTR=`expr $CNTR + 1`
		if [ $CNTR -gt 35 ]; then
			rm -f /tmp/modgone
			break
		fi
	done
}

rebind() {
	CFUNDONE=false
	ARG=$1
	CURRMODEM=$(echo "${ARG: -1}")
	PROT=$(uci -q get modem.modem$CURRMODEM.proto)
	CPORT=$(uci -q get modem.modem$CURRMODEM.commport)
	if [ -n "$CPORT" ]; then
		VENDOR=$(uci -q get modem.modem$CURRMODEM.idV)
		PRODUCT=$(uci -q get modem.modem$CURRMODEM.idP)
		# list vendors that do not fully support 3GPP +CFUN
		NOCFUN="2c7c"  # Quectel will stuck after CFUN, so not using that method at all
		case $VENDOR in
			"12d1" )
				ATCMDD="AT^RESET"
				;;
			"2c7c" )
				ATCMDD="AT"
                                ;;
			* )
				ATCMDD="AT+CFUN=1,1"
				;;
		esac
		OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
		if `echo ${OX} | grep "OK" 1>/dev/null 2>&1` && \
		[[ ! `echo $NOCFUN | grep -o "$VENDOR"` ]]; then
			CFUNDONE=true
			log "Hard modem reset done on /dev/ttyUSB$CPORT to reload drivers"
			ifdown wan$CURRMODEM
			uci delete network.wan$CURRMODEM
			uci set network.wan$CURRMODEM=interface
			uci set network.wan$CURRMODEM.proto=dhcp
			uci set network.wan$CURRMODEM.${ifname1}="wan"$CURRMODEM
			uci set network.wan$CURRMODEM.metric=$CURRMODEM"0"
			uci commit network
			/etc/init.d/network reload
			ifdown wan$CURRMODEM
			echo "1" > /tmp/modgone
			log "Setting Modem Removal flag (1)"
		fi
	fi
	if ! $CFUNDONE; then
		PORT=$1
		log "Re-binding USB driver on $PORT to reset modem"
		echo $PORT > /sys/bus/usb/drivers/usb/unbind
		sleep 35
		echo $PORT > /sys/bus/usb/drivers/usb/bind
		ifdown wan$CURRMODEM
		uci delete network.wan$CURRMODEM
		uci set network.wan$CURRMODEM=interface
		uci set network.wan$CURRMODEM.proto=dhcp
		uci set network.wan$CURRMODEM.${ifname1}="wan"$CURRMODEM
		uci set network.wan$CURRMODEM.metric=$CURRMODEM"0"
		uci commit network
		/etc/init.d/network reload
		ifdown wan$CURRMODEM
		echo "1" > /tmp/modgone
		log "Setting Modem Removal flag (2)"
		if [[ -n "$CPORT" ]] && [[ ! `echo $NOCFUN | grep -o "$VENDOR"` ]]; then
			OX=$($ROOTER/gcom/gcom-locked "/dev/ttyUSB$CPORT" "run-at.gcom" "$CURRMODEM" "$ATCMDD")
			sleep 30
		else
			if [ -f $ROOTER_LINK/reconnect$CURRMODEM ]; then
				$ROOTER_LINK/reconnect$CURRMODEM $CURRMODEM &
			fi
		fi
	fi
}

power_toggle() {
	MODE=$1
	REBIND=false
	if [ -f "/tmp/gpiopin" ]; then
		rm -f /tmp/modgone
		source /tmp/gpiopin
		if [ -f "/tmp/gpioname" ]; then
			source /tmp/gpioname
		else
			echo "$GPIOPIN" > /sys/class/gpio/unexport
			echo "$GPIOPIN" > /sys/class/gpio/export
			if [ $? -eq 0 ]; then
				echo "out" > /sys/class/gpio/gpio$GPIOPIN/direction
				if [ $? -ne 0 ]; then
					REBIND=true
				fi
			else
				REBIND=true
			fi
		fi
		if ! $REBIND; then
			if [ -z $GPIOPIN2 ]; then
				if [ -z $GPIONAME ]; then
					GPIOT="gpio$GPIOPIN"
				else
					GPIOT=$GPIONAME
				fi
				echo 0 > /sys/class/gpio/$GPIOT/value
				waitfor
				echo 1 > /sys/class/gpio/$GPIOT/value
			else
				if [ -z $GPIONAME2 ]; then
					echo "$GPIOPIN2" > /sys/class/gpio/unexport
					echo "$GPIOPIN2" > /sys/class/gpio/export
					echo "out" > /sys/class/gpio/gpio$GPIOPIN2/direction

					GPIOT2="gpio$GPIOPIN2"
				else
					GPIOT2=$GPIONAME2
				fi
				if [ -z $GPIONAME ]; then
					GPIOT="gpio$GPIOPIN"
				else
					GPIOT=$GPIONAME
				fi
				if [ $MODE = 1 ]; then
					echo 0 > /sys/class/gpio/$GPIOT/value
					waitfor
					echo 1 > /sys/class/gpio/$GPIOT/value
				fi
				if [ $MODE = 2 ]; then
					echo 0 > /sys/class/gpio/$GPIOT2/value
					waitfor
					echo 1 > /sys/class/gpio/$GPIOT2/value
				fi
				if [ $MODE = 3 ]; then
					echo 0 > /sys/class/gpio/$GPIOT/value
					echo 0 > /sys/class/gpio/$GPIOT2/value
					waitfor
					echo 1 > /sys/class/gpio/$GPIOT/value
					echo 1 > /sys/class/gpio/$GPIOT2/value
				fi
				sleep 2
			fi
			echo "1" > /tmp/modgone
			log "Power Toggle Modem"
			log "Setting Modem Removal flag (3)"
		fi
	else
		REBIND=true
	fi
	if $REBIND; then
		# unbind/bind driver from USB to reset modem when power toggle is selected, but not available
		if [ $MODE = 1 ]; then
			PORT="usb1"
			rebind $PORT
		fi
		if [ $MODE = 2 ]; then
			PORT="usb2"
			rebind $PORT
		fi
		echo "1" > /tmp/modgone
		log "Setting Modem Removal flag (4)"
	fi
}

power_toggle $1
