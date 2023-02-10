#!/bin/sh
INCLUDE_ONLY=1

. ../netifd-proto.sh
. ./ppp.sh
init_proto "$@"

ROOTER=/usr/lib/rooter
ROOTER_LINK="/tmp/links"

log() {
	logger -t "Create PPP Connection" "$@"
}

chcklog() {
	OOX=$1
	CLOG=$(uci get modem.modeminfo$CURRMODEM.log)
	if [ $CLOG = "1" ]; then
		log "$OOX"
	fi
}

proto_3x_init_config() {
	no_device=1
	available=1
	ppp_generic_init_config
	proto_config_add_string "device"
	proto_config_add_string "apn"
	proto_config_add_string "service"
	proto_config_add_string "pincode"
	proto_config_add_string "dialnumber"
}

proto_3x_setup() {
	local interface="$1"
	local chat

	if [ ! -f /tmp/bootend.file ]; then
		return 0
	fi

	json_get_var device device
	json_get_var apn apn
	json_get_var service service
	json_get_var pincode pincode
	
	CURRMODEM=$(uci get network.$interface.currmodem)

	uci set modem.modem$CURRMODEM.connected=0
	uci commit modem
	jkillall getsignal$CURRMODEM
	rm -f $ROOTER_LINK/getsignal$CURRMODEM
	jkillall con_monitor$CURRMODEM
	rm -f $ROOTER_LINK/con_monitor$CURRMODEM



	[ -e "$device" ] || {
		proto_set_available "$interface" 0
		return 1
	}

	if [ $service = "umts" ]; then
		idV=$(uci get modem.modem$CURRMODEM.idV)
		if [ $idV = 12d1 ]; then
			$ROOTER/gcom/gcom-locked "$device" "curc.gcom" "$CURRMODEM"
			log "Unsolicited Responses Disabled"
		fi
		chat="/etc/chatscripts/3g.chat"
		if [ -n "$pincode" ]; then
			OX=$($ROOTER/gcom/gcom-locked "$device" "setpin.gcom" "$CURRMODEM")
			ERROR="ERROR"
			if `echo ${OX} | grep "${ERROR}" 1>/dev/null 2>&1`
			then
				log "Modem $CURRMODEM Failed to Unlock SIM Pin"
				chcklog "$OX"
				$ROOTER/signal/status.sh $CURRMODEM "$MAN $MOD" "Failed to Connect : Pin Locked"
				proto_notify_error "$interface" PIN_FAILED
				proto_block_restart "$interface"
				return 1
			fi
		fi
		export SETUSER=$(uci get modem.modeminfo$CURRMODEM.user)
		export SETPASS=$(uci get modem.modeminfo$CURRMODEM.pass)
		export SETAUTH=$(uci get modem.modeminfo$CURRMODEM.auth)
		OX=$($ROOTER/gcom/gcom-locked "$device" "connect-ppp.gcom" "$CURRMODEM")
		ERROR="ERROR"
		if `echo ${OX} | grep "${ERROR}" 1>/dev/null 2>&1`
		then
			log "Error for Modem $CURRMODEM on Authorization"
			chcklog "$OX"
		fi

		if [ -z "$dialnumber" ]; then
			dialnumber="*99***1#"
		fi
	else
		chat="/etc/chatscripts/evdo.chat"
	fi

	connect="${apn:+USE_APN=$apn }DIALNUMBER=$dialnumber /usr/sbin/chat -t5 -v -E -f $chat"

	ppp_generic_setup "$interface" \
		noaccomp \
		nopcomp \
		novj \
		nobsdcomp \
		noauth \
		lock \
		crtscts \
		115200 "$device"

	sleep 20
	MAN=$(uci get modem.modem$CURRMODEM.manuf)
	MOD=$(uci get modem.modem$CURRMODEM.model)
	$ROOTER/log/logger "Modem #$CURRMODEM Connected ($MAN $MOD)"
	
	if [ -e $ROOTER/modem-led.sh ]; then
		$ROOTER/modem-led.sh $CURRMODEM 3
	fi
	
	PROT=$(uci get modem.modem$CURRMODEM.proto)
	if [ $service = "umts" ]; then
		ln -s $ROOTER/signal/modemsignal.sh $ROOTER_LINK/getsignal$CURRMODEM
		$ROOTER_LINK/getsignal$CURRMODEM $CURRMODEM $PROT &
	fi
	ln -s $ROOTER/connect/reconnect-ppp.sh $ROOTER_LINK/reconnect$CURRMODEM
	ln -s $ROOTER/connect/conmon.sh $ROOTER_LINK/con_monitor$CURRMODEM
	$ROOTER_LINK/con_monitor$CURRMODEM $CURRMODEM &
	uci set modem.modem$CURRMODEM.connected=1
	uci set modem.modem$CURRMODEM.interface="3x-"$interface
	uci commit modem
	CLB=$(uci get modem.modeminfo$CURRMODEM.lb)
	if [ -e /etc/config/mwan3 ]; then
		INTER=$(uci get modem.modeminfo$CURRMODEM.inter)
		if [ -z $INTER ]; then
			INTER=0
		else
			if [ $INTER = 0 ]; then
				INTER=$CURRMODEM
			fi
		fi
		
		if [ -e $ROOTER/timezone.sh ]; then
			TZ=$(uci -q get modem.modeminfo$CURRMODEM.tzone)
			if [ "$TZ" = "1" ]; then
				log "Set TimeZone"
				$ROOTER/timezone.sh &
			fi
		fi
	
		ENB=$(uci get mwan3.wan$CURRMODEM.enabled)
		if [ ! -z $ENB ]; then
			if [ $CLB = "1" ]; then
				uci set mwan3.wan$INTER.enabled=1
			else
				uci set mwan3.wan$INTER.enabled=0
			fi
			uci commit mwan3
			/usr/sbin/mwan3 restart
		fi
	fi
	rm -f /tmp/usbwait
	return 0
}

proto_3x_teardown() {
	proto_kill_command "$interface"
}

add_protocol 3x
